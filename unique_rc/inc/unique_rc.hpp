// unique_rc.h : Defines template class for handles
//

#ifndef UNIQUE_RC_HPP
#define UNIQUE_RC_HPP

#include "raii_defs.hpp"

#include <cassert>
#include <concepts>
#include <tuple>
#include <utility>

//----------------------------------------------------------------------
// Encapsulates handles and performs resource release
// before going out of scope
//----------------------------------------------------------------------

RAII_NS_BEGIN

template<typename T>
using is_not_pointer_default_constructable =
  std::conjunction<std::negation<std::is_pointer<T>>, std::is_default_constructible<T>>;

template<typename T>
concept is_not_pointer_default_constructable_v = is_not_pointer_default_constructable<T>::value;

template<typename T, typename U>
concept has_static_invalid_convertible_and_comparable = requires {
  { T::invalid() } noexcept -> std::convertible_to<U>;
  { T::invalid() } noexcept -> std::equality_comparable_with<U>;
};

template<typename Handle, typename Deleter> class unique_rc_holder_impl
{

  template<typename H1, typename D1, typename = void> struct HandleResolver
  {
    using type = H1;
  };

  template<typename H1, typename D1>
  struct HandleResolver<H1, D1, std::void_t<typename std::remove_reference_t<D1>::handle>>
  {
    using type = typename std::remove_reference_t<D1>::handle;
  };

public:
  // using _DeleterConstraint = enable_if<
  // __and_<__not_<is_pointer<Deleter>>,
  // 	is_default_constructible<Deleter>>::value>;

  using handle = typename HandleResolver<Handle, Deleter>::type;

  static_assert(!std::is_rvalue_reference_v<Deleter>,
    "unique_rc's deleter type must be a function object type"
    " or an lvalue reference type");

  // cppcheck-suppress passedByValueCallback - handle is not a heavy type, it is okay to pass by value
  raii_inline explicit constexpr unique_rc_holder_impl(handle h) noexcept : hdt_{ h, Deleter{} } {}

  template<typename D> raii_inline constexpr unique_rc_holder_impl(handle h, D &&d) : hdt_{ h, std::forward<D>(d) } {}

  raii_inline constexpr unique_rc_holder_impl(unique_rc_holder_impl &&other) noexcept : hdt_{ std::move(other.hdt_) }
  {
    other.get_handle() = Deleter::invalid();
  }

  // cppcheck-suppress operatorEqVarError - false positive, member variable is assigned indirectly via get_handle() in
  // reset(), and get_deleter()
  raii_inline constexpr unique_rc_holder_impl &operator=(unique_rc_holder_impl &&other) noexcept
  {
    reset(other.release());
    get_deleter() = std::forward<Deleter>(other.get_deleter());

    return *this;
  }

  raii_inline constexpr handle &get_handle() noexcept { return std::get<0>(hdt_); }

  raii_inline constexpr const handle &get_handle() const noexcept { return std::get<0>(hdt_); }

  raii_inline constexpr Deleter &get_deleter() noexcept { return std::get<1>(hdt_); }

  raii_inline constexpr const Deleter &get_deleter() const noexcept { return std::get<1>(hdt_); }

  raii_inline constexpr void reset(handle h) noexcept
  {
    const handle old_h = std::exchange(get_handle(), h);
    if (old_h != Deleter::invalid()) {
      assert(old_h != h && "Failed self-reset check, like p.reset(p.get())");
      get_deleter()(old_h);
    }
  }

  raii_inline constexpr handle release() noexcept
  {
    handle h = get_handle();
    get_handle() = Deleter::invalid();

    return h;
  }

  raii_inline constexpr void swap(unique_rc_holder_impl &rhs) noexcept
  {
    using std::swap;

    // ADL
    swap(this->get_handle(), rhs.get_handle());
    swap(this->get_deleter(), rhs.get_deleter());
  }

private:
  std::tuple<handle, Deleter> hdt_;
};


// Defines move construction + assignment as either defaulted or deleted.
template<typename Handle,
  typename Deleter,
  bool = std::is_move_constructible_v<Deleter>,
  bool = std::is_move_assignable_v<Deleter>>
struct unique_rc_holder : unique_rc_holder_impl<Handle, Deleter>
{
  using unique_rc_holder_impl<Handle, Deleter>::unique_rc_holder_impl;

  unique_rc_holder(unique_rc_holder &&) = default;
  unique_rc_holder &operator=(unique_rc_holder &&) = default;
};

template<typename Handle, typename Deleter>
struct unique_rc_holder<Handle, Deleter, true, false> : unique_rc_holder_impl<Handle, Deleter>
{
  using unique_rc_holder_impl<Handle, Deleter>::unique_rc_holder_impl;

  unique_rc_holder(unique_rc_holder &&) = default;
  unique_rc_holder &operator=(unique_rc_holder &&) = delete;
};

template<typename Handle, typename Deleter>
struct unique_rc_holder<Handle, Deleter, false, true> : unique_rc_holder_impl<Handle, Deleter>
{
  using unique_rc_holder_impl<Handle, Deleter>::unique_rc_holder_impl;

  unique_rc_holder(unique_rc_holder &&) = delete;
  unique_rc_holder &operator=(unique_rc_holder &&) = default;
};

template<typename Handle, typename Deleter>
struct unique_rc_holder<Handle, Deleter, false, false> : unique_rc_holder_impl<Handle, Deleter>
{
  using unique_rc_holder_impl<Handle, Deleter>::unique_rc_holder_impl;

  unique_rc_holder(unique_rc_holder &&) = delete;
  unique_rc_holder &operator=(unique_rc_holder &&) = delete;
};


// unique_rc non-copyable handle to a resource, implements RAII

// There is no class template argument deduction from pointer type
// because it is impossible to distinguish a pointer obtained from array and non - array forms of new.
template<typename Handle, typename Deleter>
  requires has_static_invalid_convertible_and_comparable<Deleter, std::decay_t<Handle>>
class unique_rc
{
  static_assert(!std::is_array_v<Handle>, "unique_rc does not work with array, use raii::unique_ptr");

public:
  using handle = typename unique_rc_holder_impl<Handle, std::remove_reference<Deleter>>::handle;
  using element_type = Handle;
  using deleter_type = Deleter;
  using invalid_handle_type = std::remove_cvref_t<decltype(Deleter::invalid())>;

private:
  // helper template for detecting a safe conversion from another
  // unique_ptr
  template<typename H, typename D>
  using safe_conversion_from =
    std::conjunction<std::is_convertible<typename unique_rc<H, D>::handle, handle>, std::negation<std::is_array<H>>>;

public:
  /// Default constructor, creates a unique_rc that owns nothing.
  template<typename D = Deleter>
    requires is_not_pointer_default_constructable_v<Deleter>
  raii_inline constexpr unique_rc() noexcept : uh_{ invalid() }
  {}

  template<typename D = Deleter>
    requires is_not_pointer_default_constructable_v<D>
  raii_inline explicit constexpr unique_rc(handle h) noexcept : uh_{ h }
  {}

  template<typename D = Deleter>
    requires std::is_copy_constructible_v<D>
  raii_inline constexpr unique_rc(handle h, const Deleter &d) noexcept : uh_{ h, d }
  {}

  template<typename D = Deleter>
    requires std::conjunction_v<std::negation<std::is_reference<D>>, std::is_move_constructible<D>>
  raii_inline constexpr unique_rc(handle h, Deleter &&d) noexcept : uh_{ h, std::move(d) }
  {}

  template<typename D = Deleter>
    requires std::conjunction_v<std::is_reference<D>, std::is_constructible<D, std::remove_reference_t<D>>>
  unique_rc(handle, std::remove_reference_t<Deleter> &&) = delete;

  // template<typename D>
  // raii_inline constexpr unique_rc(handle h, D&& d) noexcept requires std::constructible_from<deleter_type, D>
  //	: uh_{ h, std::forward<D>(d) }
  //{}

  constexpr unique_rc(unique_rc &&) = default;

  // Converting constructor from another type
  template<typename H, typename D>
    requires std::conjunction_v<safe_conversion_from<H, D>,
      std::conditional_t<std::is_reference_v<Deleter>, std::is_same<D, Deleter>, std::is_convertible<D, Deleter>>>
  // cppcheck-suppress noExplicitConstructor intended converting constructor
  raii_inline constexpr unique_rc(unique_rc<H, D> &&src) noexcept
    : uh_{ src.release(), std::forward<D>(src.get_deleter()) }
  {}

  constexpr unique_rc &operator=(unique_rc &&) = default;

  // Assignment from another type
  template<typename H, typename D>
    requires std::conjunction_v<safe_conversion_from<H, D>, std::is_assignable<deleter_type &, D &&>>
  raii_inline constexpr unique_rc &operator=(unique_rc<H, D> &&rhs) noexcept
  {
    reset(rhs.release());
    get_deleter() = std::forward<Deleter>(rhs.get_deleter());

    return *this;
  }

  unique_rc(const unique_rc &) = delete;
  unique_rc &operator=(const unique_rc &) = delete;


  // Destructor, invokes the deleter if the stored handle is valid
  raii_inline constexpr ~unique_rc() noexcept
  {
    static_assert(std::is_invocable_v<deleter_type &, handle>, "unique_rc's deleter must be invocable with a handle");

    auto &h = uh_.get_handle();
    if (h != invalid()) {
      get_deleter()(std::move(h));
      h = invalid();
    }
  }

  [[nodiscard]] raii_inline constexpr handle operator->() const noexcept(noexcept(get())) { return get(); }

  [[nodiscard]] raii_inline constexpr handle get() const noexcept { return uh_.get_handle(); }

  [[nodiscard]] raii_inline constexpr deleter_type &get_deleter() noexcept { return uh_.get_deleter(); }

  [[nodiscard]] raii_inline constexpr const deleter_type &get_deleter() const noexcept { return uh_.get_deleter(); }

  raii_inline constexpr handle release() noexcept { return uh_.release(); }

  raii_inline constexpr void reset(handle new_h = invalid()) noexcept
  {
    static_assert(std::is_invocable_v<deleter_type &, handle>, "unique_rc's deleter must be invocable with a handle");
    uh_.reset(std::move(new_h));
  }

  [[nodiscard]] raii_inline constexpr explicit operator bool() const noexcept { return invalid() != get(); }

  raii_inline static constexpr invalid_handle_type invalid() noexcept(noexcept(Deleter::invalid()))
  {
    return Deleter::invalid();
  }

  raii_inline constexpr void swap(unique_rc &other) noexcept(
    std::is_nothrow_swappable_v<unique_rc_holder<Handle, Deleter>>)
    requires std::is_swappable_v<Deleter>
  {
    uh_.swap(other.uh_);
  }

private:
  unique_rc_holder<Handle, Deleter> uh_;
};

template<typename H1, class D1, typename H2, class D2>
  requires std::equality_comparable_with<H1, H2>
[[nodiscard]] raii_inline constexpr bool operator==(const unique_rc<H1, D1> &lhs,
  const unique_rc<H2, D2> &rhs) noexcept(noexcept(lhs.get() == rhs.get()))
{
  return lhs.get() == rhs.get();
}

// unique_ptr comparison with nullptr
template<typename H, typename D>
  requires std::same_as<typename unique_rc<H, D>::invalid_handle_type, std::nullptr_t>
[[nodiscard]] raii_inline constexpr bool operator==(const unique_rc<H, D> &r, std::nullptr_t) noexcept
{
  return !r;
}

template<typename H1, class D1, typename H2, class D2>
  requires std::three_way_comparable_with<typename unique_rc<H1, D1>::handle, typename unique_rc<H2, D2>::handle>
[[nodiscard]] raii_inline constexpr std::compare_three_way_result_t<typename unique_rc<H1, D1>::handle,
  typename unique_rc<H2, D2>::handle>
  operator<=>(const unique_rc<H1, D1> &lhs, const unique_rc<H2, D2> &rhs) noexcept(noexcept(lhs.get() <=> rhs.get()))
{
  return lhs.get() <=> rhs.get();
}

template<typename H, typename D>
  requires std::same_as<typename unique_rc<H, D>::invalid_handle_type, std::nullptr_t>
           && std::three_way_comparable<typename unique_rc<H, D>::handle>
[[nodiscard]] raii_inline constexpr std::compare_three_way_result_t<typename unique_rc<H, D>::handle>
  operator<=>(const unique_rc<H, D> &r, std::nullptr_t) noexcept(noexcept(r.get()))
{
  using handle = typename unique_rc<H, D>::handle;
  return r.get() <=> static_cast<handle>(nullptr);
}

template<typename H, typename D>
  requires std::is_swappable_v<typename unique_rc<H, D>::Handle>
raii_inline constexpr void swap(unique_rc<H, D> &lhs, unique_rc<H, D> &rhs) noexcept(noexcept(lhs.swap(rhs)))
{
  lhs.swap(rhs);
}

RAII_NS_END

#endif// UNIQUE_RC_HPP
