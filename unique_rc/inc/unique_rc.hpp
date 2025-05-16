// Defines template class for handles
//

#ifndef UNIQUE_RC_HPP
#define UNIQUE_RC_HPP

#include "raii_defs.hpp"

#include "concepts.hpp"

#include <cassert>
#include <concepts>
#include <functional>
#include <ostream>
#include <tuple>
#include <utility>

//----------------------------------------------------------------------
// Encapsulates handles and performs resource release
// before going out of scope
//----------------------------------------------------------------------

RAII_NS_BEGIN

namespace detail {

// hash helper classes
template<typename Urc, typename Handle> struct unique_rc_hash
{
  raii_inline constexpr std::size_t operator()(const Urc &r) const
    noexcept(noexcept(std::declval<std::hash<Handle>>()(std::declval<Handle>())))
  {
    return std::hash<Handle>{}(r.get());
  }
};

template<typename T, typename = void> constexpr bool is_hash_enabled_for = false;

template<typename T>
constexpr bool is_hash_enabled_for<T, std::void_t<decltype(std::hash<T>{}(std::declval<T>()))>> = true;

// Helper struct for defining disabled specializations of std::hash.
template<typename T> struct hash_not_enabled
{
  hash_not_enabled() = delete;
  hash_not_enabled(hash_not_enabled &) = delete;
  hash_not_enabled(hash_not_enabled &&) = delete;

  hash_not_enabled &operator=(hash_not_enabled &) = delete;
  hash_not_enabled &operator=(hash_not_enabled &&) = delete;

  // Commented to prevent implicitly deleted warning on MSVC 19.43 treated as error
  //~hash_not_enabled() = delete;
};

template<typename Urc, typename Handle>
using unique_rc_hash_base =
  std::conditional_t<is_hash_enabled_for<Handle>, unique_rc_hash<Urc, Handle>, hash_not_enabled<Handle>>;
}// namespace detail

template<typename Handle, typename Del_noref> struct resolve_handle_type
{
  using type = Handle;
};

template<typename Handle, typename Del_noref>
  requires has_handle_type<Del_noref>
struct resolve_handle_type<Handle, Del_noref>
{
  using type = typename Del_noref::handle;
};

// deleter requirements
template<typename T>
using is_not_pointer_default_constructable =
  std::conjunction<std::negation<std::is_pointer<T>>, std::is_default_constructible<T>>;

template<typename T>
concept is_not_pointer_default_constructable_v = is_not_pointer_default_constructable<T>::value;

template<typename D, typename Hr>
concept has_static_invalid_convertible_handle = requires {
  { D::invalid() } noexcept -> std::convertible_to<Hr>;
  //{ D::invalid() } noexcept -> std::equality_comparable_with<typename detail::resolve_handle_type<H, D>::type>;
} || requires {
  { D::invalid() } noexcept -> std::same_as<Hr>;
};


template<typename Handle, typename Deleter> class unique_rc_holder_impl
{
public:
  // using _DeleterConstraint = enable_if<
  // __and_<__not_<is_pointer<Deleter>>,
  // 	is_default_constructible<Deleter>>::value>;

  using handle = typename resolve_handle_type<Handle, std::remove_reference_t<Deleter>>::type;

  static_assert(!std::is_rvalue_reference_v<Deleter>,
    "unique_rc's deleter type must be a function object type"
    " or an lvalue reference type");

  // cppcheck-suppress passedByValueCallback - handle is not a heavy type, it is okay to pass by value
  raii_inline explicit constexpr unique_rc_holder_impl(handle h) noexcept : hdt_{ h, Deleter{} } {}

  template<typename D> raii_inline constexpr unique_rc_holder_impl(handle h, D &&d) : hdt_{ h, std::forward<D>(d) } {}

  raii_inline constexpr unique_rc_holder_impl(unique_rc_holder_impl &&other) noexcept : hdt_{ std::move(other.hdt_) }
  {
    other.get_handle() = get_deleter().invalid();
  }

  // cppcheck-suppress operatorEqVarError - false positive, member variable is assigned indirectly via get_handle() in
  // reset(), and get_deleter()
  raii_inline constexpr unique_rc_holder_impl &operator=(unique_rc_holder_impl &&other) noexcept
  {
    reset(other.release());
    get_deleter() = std::forward<Deleter>(other.get_deleter());

    // This is false positive, since reset(other.release()) calls deleter, and acquires ownership of other.release()
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
    return *this;
  }

  raii_inline constexpr handle &get_handle() noexcept { return std::get<0>(hdt_); }

  raii_inline constexpr const handle &get_handle() const noexcept { return std::get<0>(hdt_); }

  raii_inline constexpr Deleter &get_deleter() noexcept { return std::get<1>(hdt_); }

  raii_inline constexpr const Deleter &get_deleter() const noexcept { return std::get<1>(hdt_); }

  raii_inline constexpr void reset(handle h) noexcept
  {
    const handle old_h = std::exchange(get_handle(), h);
    // if (old_h != Deleter::invalid()) {
    if (get_deleter().is_owned(old_h)) {
#ifdef UNIQUE_RC_ENABLE_SELF_RESET_ASSERT
      assert(old_h != h && "Failed self-reset check, like p.reset(p.get())");
#endif// UNIQUE_RC_ENABLE_SELF_RESET_ASSERT
      get_deleter()(old_h);
    }

    // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
  }

  raii_inline constexpr handle release() noexcept
  {
    handle h = get_handle();
    get_handle() = get_deleter().invalid();

    return h;
  }

  raii_inline constexpr void swap(unique_rc_holder_impl &rhs) noexcept
  {
    std::ranges::swap(this->get_handle(), rhs.get_handle());
    std::ranges::swap(this->get_deleter(), rhs.get_deleter());
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
  requires has_static_invalid_convertible_handle<std::remove_reference_t<Deleter>,
    typename resolve_handle_type<std::decay_t<Handle>, std::remove_reference_t<Deleter>>::type>
class unique_rc
{
  static_assert(!std::is_array_v<Handle>, "unique_rc does not work with array, use raii::unique_ptr");

public:
  using handle = typename unique_rc_holder_impl<Handle, Deleter>::handle;
  using element_type = Handle;
  using deleter_type = Deleter;
  using invalid_handle_type = std::remove_cvref_t<decltype(std::remove_reference_t<Deleter>::invalid())>;

private:
  // helper template for detecting a safe conversion from another
  // unique_ptr
  template<typename H, typename D>
  using safe_conversion_from =
    std::conjunction<std::is_convertible<typename unique_rc<H, D>::handle, handle>, std::negation<std::is_array<H>>>;

public:
  /// Default constructor, creates a unique_rc that owns nothing.
  // template<typename D = Deleter>
  //   requires is_not_pointer_default_constructable_v<Deleter>
  raii_inline constexpr unique_rc() noexcept
    requires is_not_pointer_default_constructable_v<Deleter>
    : uh_{ invalid() }
  {}

  // template<typename D = Deleter>
  //   requires is_not_pointer_default_constructable_v<D>
  raii_inline explicit constexpr unique_rc(handle h) noexcept
    requires is_not_pointer_default_constructable_v<Deleter>
    : uh_{ h }
  {}

  // template<typename D = Deleter>
  //   requires std::is_copy_constructible_v<D>
  raii_inline constexpr unique_rc(handle h, const Deleter &d) noexcept
    requires std::is_copy_constructible_v<Deleter>
    : uh_{ h, d }
  {}

  // template<typename D = Deleter>
  //   requires std::conjunction_v<std::negation<std::is_reference<D>>, std::is_move_constructible<D>>
  raii_inline constexpr unique_rc(handle h, Deleter &&d) noexcept
    requires std::conjunction_v<std::negation<std::is_reference<Deleter>>, std::is_move_constructible<Deleter>>
    : uh_{ h, std::move(d) }
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
    get_deleter() = std::forward<D>(rhs.get_deleter());

    return *this;
  }

  unique_rc(const unique_rc &) = delete;
  unique_rc &operator=(const unique_rc &) = delete;


  // Destructor, invokes the deleter if the stored handle is valid
  raii_inline constexpr ~unique_rc() noexcept
  {
    static_assert(std::is_invocable_v<deleter_type &, handle>, "unique_rc's deleter must be invocable with a handle");

    auto &h = uh_.get_handle();
    // NOLINTNEXTLINE(clang-diagnostic-unused-result)
    if (get_deleter().is_owned(h)) {
      get_deleter()(std::move(h));
      h = invalid();
    }
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
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

  [[nodiscard]] raii_inline constexpr explicit operator bool() const noexcept
  {
    // NOLINTNEXTLINE(clang-diagnostic-unused-result)
    return get_deleter().is_owned(get());
  }

  [[nodiscard]] raii_inline static constexpr invalid_handle_type invalid() noexcept(noexcept(std::remove_reference_t<Deleter>::invalid()))
  {
    return std::remove_reference_t<Deleter>::invalid();
  }

  raii_inline constexpr void swap(unique_rc &other) noexcept(
    std::is_nothrow_swappable_v<Handle> && std::is_nothrow_swappable_v<Deleter>)
    requires std::is_swappable_v<Handle> && std::is_swappable_v<Deleter>
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
[[nodiscard]] raii_inline constexpr bool operator==(const unique_rc<H, D> &lhs, std::nullptr_t) noexcept
{
  return !lhs;
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
  operator<=>(const unique_rc<H, D> &lhs, std::nullptr_t) noexcept(noexcept(lhs.get()))
{
  using handle = typename unique_rc<H, D>::handle;
  return lhs.get() <=> static_cast<handle>(nullptr);
}

template<typename H, typename D>
  requires std::is_swappable_v<H> && std::is_swappable_v<D>
raii_inline constexpr void swap(unique_rc<H, D> &lhs, unique_rc<H, D> &rhs) noexcept(
  noexcept(std::is_nothrow_swappable_v<H> && std::is_nothrow_swappable_v<D>))
{
  lhs.swap(rhs);
}

template<typename H, typename D>
  requires(!std::is_swappable_v<H> || !std::is_swappable_v<D>)
void swap(unique_rc<H, D> &lhs, unique_rc<H, D> &rhs) = delete;

template<typename CharT, typename Traits, typename H, typename D>
raii_inline constexpr std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &ostream,
  const unique_rc<H, D> &rc) noexcept
  requires requires { ostream << rc.get(); }
{
  ostream << rc.get();
  return ostream;
}


RAII_NS_END

namespace std {

// std::hash specialization for unique_rc.
template<typename H, typename D>
struct hash<raii::unique_rc<H, D>>
  : public raii::detail::unique_rc_hash_base<raii::unique_rc<H, D>, typename raii::unique_rc<H, D>::handle>
{
};

}// namespace std

#endif// UNIQUE_RC_HPP
