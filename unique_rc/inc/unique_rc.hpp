// unique_rc.h : Defines template class for handles
//

#ifndef UNIQUE_RC_HPP
#define UNIQUE_RC_HPP

#include "defs.hpp"

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
concept is_not_pointer_default_constructable_v =
  std::conjunction_v<std::negation<std::is_pointer<T>>, std::is_default_constructible<T>>;

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

  constexpr unique_rc_holder_impl() = default;

  raii_inline explicit constexpr unique_rc_holder_impl(handle h) noexcept : hdt_{ h, {} } {}

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
  requires has_static_invalid_convertible_and_comparable<Deleter, Handle>
class unique_rc
{
public:
  using handle = typename unique_rc_holder_impl<Handle, std::remove_reference<Deleter>>::handle;
  using element_type = Handle;
  using deleter_type = Deleter;

private:
  // helper template for detecting a safe conversion from another
  // unique_ptr
  template<typename H, typename D>
  using safe_conversion_from =
    std::conjunction<std::is_convertible<typename unique_rc<H, D>::handle, handle>, std::negation<std::is_array<H>>>;


public:
  /// Default constructor, creates a unique_rc that owns nothing.
  // template<typename D = Deleter>
  raii_inline constexpr unique_rc() noexcept
    requires is_not_pointer_default_constructable_v<Deleter>
    : uh_{}
  {}

  template<typename D = Deleter>
  raii_inline explicit constexpr unique_rc(handle h) noexcept
    requires is_not_pointer_default_constructable_v<D>
    : uh_{ h }
  {}

  template<typename D = Deleter>
  raii_inline constexpr unique_rc(handle h, const Deleter &d) noexcept
    requires std::is_copy_constructible_v<D>
    : uh_{ h, d }
  {}

  template<typename D = Deleter>
  raii_inline constexpr unique_rc(handle h, Deleter &&d) noexcept
    requires std::conjunction_v<std::negation<std::is_reference<D>>, std::is_constructible<D, D>>
    : uh_{ h, std::move(d) }
  {}

  template<typename D = Deleter,
    std::enable_if_t<std::conjunction_v<std::is_reference<D>, std::is_constructible<D, std::remove_reference_t<D>>>,
      int> = 0>
  unique_rc(handle, std::remove_reference_t<Deleter> &&) = delete;

  // template<typename D>
  // raii_inline constexpr unique_rc(handle h, D&& d) noexcept requires std::constructible_from<deleter_type, D>
  //	: uh_{ h, std::forward<D>(d) }
  //{}

  constexpr unique_rc(unique_rc &&) = default;

  // Converting constructor from another type
  template<typename H, typename D>
  //__safe_conversion_up<_Up, _Ep>,
  raii_inline constexpr unique_rc(unique_rc<H, D> &&src) noexcept
    requires std::is_convertible_v<typename unique_rc<H, D>::handle, handle>
             && (std::is_same_v<D, Deleter> || std::is_convertible_v<D, Deleter>)
    : uh_{ src.release(), std::forward<D>(src.get_deleter()) }
  {}

  constexpr unique_rc &operator=(unique_rc &&) = default;

  // Assignment from another type
  template<typename H, typename D>
  raii_inline constexpr unique_rc &operator=(unique_rc<H, D> &&rhs) noexcept
    requires std::conjunction_v<safe_conversion_from<H, D>, std::is_assignable<deleter_type &, D &&>>
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

  raii_inline static constexpr auto invalid() noexcept(noexcept(Deleter::invalid())) { return Deleter::invalid(); }

  raii_inline constexpr void swap(unique_rc &other) noexcept(
    std::is_nothrow_swappable_v<unique_rc_holder<Handle, Deleter>>)
  {
    uh_.swap(other.uh_);
  }

  // How can it be compiled and work?
  // operator bool_type() const throw()
  //{
  //	const auto t = &Bool_t::unused_;
  //	return (invalid() != handle_) ? &Bool_t::unused_ : nullptr;
  //}
private:
  unique_rc_holder<Handle, Deleter> uh_;
};

template<typename T, class Del>
  requires std::equality_comparable<typename unique_rc<T, Del>::Handle>
[[nodiscard]] raii_inline constexpr bool operator==(const unique_rc<T, Del> &lhs,
  const unique_rc<T, Del> &rhs) noexcept(noexcept(lhs.get() == rhs.get()))
{
  return lhs.get() == rhs.get();
}

template<typename T, class Del>
  requires std::three_way_comparable<typename unique_rc<T, Del>::Handle>
[[nodiscard]] raii_inline constexpr std::compare_three_way_result_t<typename unique_rc<T, Del>::Handle>
  operator<=>(const unique_rc<T, Del> &lhs, const unique_rc<T, Del> &rhs) noexcept(noexcept(lhs.get() <=> rhs.get()))
{
  return lhs.get() <=> rhs.get();
}

template<typename T, class Del>
  requires std::is_swappable_v<typename unique_rc<T, Del>::Handle>
raii_inline constexpr void swap(unique_rc<T, Del> &lhs, unique_rc<T, Del> &rhs) noexcept(noexcept(lhs.swap(rhs)))
{
  lhs.swap(rhs);
}

RAII_NS_END

#endif// UNIQUE_RC_HPP
