// Defines template class for handles
//

#ifndef UNIQUE_RC_HPP
#define UNIQUE_RC_HPP

#include "raii_defs.hpp"

#include "concepts.hpp"

#include <cassert>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>


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


template<typename Handle, class Del_noref> struct resolve_handle_type
{
  using type = Handle;
};

template<typename Handle, class Del_noref>
  requires has_handle_type<Del_noref>
struct resolve_handle_type<Handle, Del_noref>
{
  using type = typename Del_noref::handle;
};


template<typename Handle, typename Invalid> struct default_invalid_handle_policy
{
  using invalid_type = Invalid;

  [[nodiscard]] raii_inline static constexpr invalid_type invalid() noexcept { return {}; }

  [[nodiscard]] raii_inline static constexpr bool is_owned(Handle h) noexcept { return h != invalid(); }


  /// @brief Disabled because policy provides only typedefs and static methods
  constexpr default_invalid_handle_policy() = delete;
  constexpr ~default_invalid_handle_policy() = delete;
};


// deleter requirements
template<typename T>
using not_pointer_and_is_default_constructable =
  std::conjunction<std::negation<std::is_pointer<T>>, std::is_default_constructible<T>>;

template<typename T>
concept not_pointer_and_is_default_constructable_v = not_pointer_and_is_default_constructable<T>::value;

template<typename Handle, typename Invalid, template<typename, typename> class InvalidHandlePolicy>
concept has_static_is_owned_and_invalid = requires {
  {
    InvalidHandlePolicy<Handle, Invalid>::invalid()
  } noexcept -> std::same_as<typename InvalidHandlePolicy<Handle, Invalid>::invalid_type>;
  { InvalidHandlePolicy<Handle, Invalid>::is_owned(std::declval<Handle>()) } noexcept -> std::same_as<bool>;
};


template<typename Handle,
  class Deleter,
  template<typename, typename> class TypeResolver,
  typename InvalidHandle,
  template<typename, typename> class InvalidHandlePolicy>
class unique_rc_holder_impl
{
public:
  // using _DeleterConstraint = enable_if<
  // __and_<__not_<is_pointer<Deleter>>,
  // 	is_default_constructible<Deleter>>::value>;

  using handle = typename TypeResolver<Handle, std::remove_reference_t<Deleter>>::type;

  using invalid_handle_policy = InvalidHandlePolicy<handle, InvalidHandle>;
  using invalid_handle = typename invalid_handle_policy::invalid_type;

  static_assert(!std::is_rvalue_reference_v<Deleter>,
    "unique_rc's deleter type must be a function object type"
    " or an lvalue reference type");

  // cppcheck-suppress passedByValueCallback - handle is not a heavy type, it is okay to pass by value
  raii_inline explicit constexpr unique_rc_holder_impl(handle h) noexcept : hdt_{ h, Deleter{} } {}

  template<class D> raii_inline constexpr unique_rc_holder_impl(handle h, D &&d) : hdt_{ h, std::forward<D>(d) } {}

  raii_inline constexpr unique_rc_holder_impl(unique_rc_holder_impl &&other) noexcept : hdt_{ std::move(other.hdt_) }
  {
    other.get_handle() = invalid_handle_policy::invalid();
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
    if (invalid_handle_policy::is_owned(old_h)) {
#ifdef RAII_ENABLE_SELF_RESET_ASSERT
      assert(old_h != h && "Failed self-reset check, like p.reset(p.get())");
#endif// RAII_ENABLE_SELF_RESET_ASSERT
      get_deleter()(old_h);
    }

    // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
  }

  raii_inline constexpr handle release() noexcept
  {
    handle h = get_handle();
    get_handle() = invalid_handle_policy::invalid();

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
  class Deleter,
  template<typename, typename> class TypeResolver,
  typename InvalidHandle,
  template<typename, typename> class InvalidHandlePolicy,
  bool = std::is_move_constructible_v<Deleter>,
  bool = std::is_move_assignable_v<Deleter>>
struct unique_rc_holder : unique_rc_holder_impl<Handle, Deleter, TypeResolver, InvalidHandle, InvalidHandlePolicy>
{
  using unique_rc_holder_impl<Handle, Deleter, TypeResolver, InvalidHandle, InvalidHandlePolicy>::unique_rc_holder_impl;

  unique_rc_holder(unique_rc_holder &&) = default;
  unique_rc_holder &operator=(unique_rc_holder &&) = default;
};

template<typename Handle,
  class Deleter,
  template<typename, typename> typename TypeResolver,
  typename InvalidHandle,
  template<typename, typename> class InvalidHandlePolicy>
struct unique_rc_holder<Handle, Deleter, TypeResolver, InvalidHandle, InvalidHandlePolicy, true, false>
  : unique_rc_holder_impl<Handle, Deleter, TypeResolver, InvalidHandle, InvalidHandlePolicy>
{
  using unique_rc_holder_impl<Handle, Deleter, TypeResolver, InvalidHandle, InvalidHandlePolicy>::unique_rc_holder_impl;

  unique_rc_holder(unique_rc_holder &&) = default;
  unique_rc_holder &operator=(unique_rc_holder &&) = delete;
};

template<typename Handle,
  class Deleter,
  template<typename, typename> typename TypeResolver,
  typename InvalidHandle,
  template<typename, typename> class InvalidHandlePolicy>
struct unique_rc_holder<Handle, Deleter, TypeResolver, InvalidHandle, InvalidHandlePolicy, false, true>
  : unique_rc_holder_impl<Handle, Deleter, TypeResolver, InvalidHandle, InvalidHandlePolicy>
{
  using unique_rc_holder_impl<Handle, Deleter, TypeResolver, InvalidHandle, InvalidHandlePolicy>::unique_rc_holder_impl;

  unique_rc_holder(unique_rc_holder &&) = delete;
  unique_rc_holder &operator=(unique_rc_holder &&) = default;
};

template<typename Handle,
  class Deleter,
  template<typename, typename> typename TypeResolver,
  typename InvalidHandle,
  template<typename, typename> class InvalidHandlePolicy>
struct unique_rc_holder<Handle, Deleter, TypeResolver, InvalidHandle, InvalidHandlePolicy, false, false>
  : unique_rc_holder_impl<Handle, Deleter, TypeResolver, InvalidHandle, InvalidHandlePolicy>
{
  using unique_rc_holder_impl<Handle, Deleter, TypeResolver, InvalidHandle, InvalidHandlePolicy>::unique_rc_holder_impl;

  unique_rc_holder(unique_rc_holder &&) = delete;
  unique_rc_holder &operator=(unique_rc_holder &&) = delete;
};

/**
 * @brief raii::unique_rc is a smart handle that owns (is responsible for) and manages another object via a handle
 * and subsequently disposes of that object when the unique_rc goes out of scope.
 *
 * The object is disposed of, using the associated deleter, when either of the following happens:

  * the managing unique_rc object is destroyed.
  * the managing unique_rc object is assigned another handle via operator= or reset().

 * The object is disposed of, using a potentially user-supplied deleter, by calling get_deleter()(handle).
 * @tparam Handle the type of the handle managed by this unique_rc
 * @tparam Deleter the function object or lvalue reference to function object, to be called from the destructor
 * @tparam TypeResolver std::remove_reference<Deleter>::type::handle if that type exists, otherwise Handle
 * @tparam InvalidHandle represents invalid handle which is assigned when resource is released or empty constructed
 unique_rc
 * @tparam InvalidHandlePolicy provides static methods `is_owned(handle)` and `invalid()`
 * @note raii::unique_rc does not work with dynamically-allocated array of objects T[], raii::unique_ptr does
 **/
template<typename Handle,
  class Deleter,
  template<typename, typename> typename TypeResolver = resolve_handle_type,
  typename InvalidHandle = typename TypeResolver<std::decay_t<Handle>, std::remove_reference_t<Deleter>>::type,
  template<typename, typename> typename InvalidHandlePolicy = default_invalid_handle_policy>
  requires has_static_is_owned_and_invalid<
    typename TypeResolver<std::decay_t<Handle>, std::remove_reference_t<Deleter>>::type,
    InvalidHandle,
    InvalidHandlePolicy>
class unique_rc
{
  static_assert(!std::is_array_v<Handle>, "raii::unique_rc is not specialised for plain array, use raii::unique_ptr");

  using storage_type = unique_rc_holder<Handle, Deleter, TypeResolver, InvalidHandle, InvalidHandlePolicy>;

public:
  using invalid_handle_policy = typename storage_type::invalid_handle_policy;

  /// @brief std::remove_reference<Deleter>::type::handle if that type exists, otherwise T
  using handle = typename storage_type::handle;

  /// @brief Handle, the type of the resource managed by this unique_rc
  using element_type = Handle;

  /// @brief Deleter, the function object or lvalue reference to function object, to be called from the destructor
  using deleter_type = Deleter;

  /// @brief Represents invalid handle type, whose value is returned by invalid_handle_policy::invalid()
  /// is assigned to *this handle, when it goes out of scope, or upon reset()
  using invalid_handle = typename storage_type::invalid_handle;

private:
  /// @brief Helper template for detecting a safe conversion from another unique_rc
  template<typename H,
    class D,
    template<typename, typename> class TR,
    typename IH,
    template<typename, typename> class IHPolicy>
  using safe_conversion_from =
    std::conjunction<std::is_convertible<typename unique_rc<H, D, TR, IH, IHPolicy>::handle, handle>,
      std::negation<std::is_array<H>>>;

public:
  /// @brief Default constructor, creates a unique_rc that owns nothing
  raii_inline constexpr unique_rc() noexcept
    requires not_pointer_and_is_default_constructable_v<Deleter>
    : uh_{ invalid() }
  {}

  /// @brief Constructs a unique_rc object which owns h, initialising the stored handle with h
  /// and value - initialising the stored deleter.
  /// @param h a handle to a resource to manage
  /// @note Requires that Deleter is DefaultConstructible and that construction does not throw an exception
  raii_inline explicit constexpr unique_rc(handle h) noexcept
    requires not_pointer_and_is_default_constructable_v<Deleter>
    : uh_{ h }
  {}

  /// @brief Constructs a unique_rc object which owns h, initialising the stored handle with h
  /// and initializing a Deleter d, requires that Deleter is nothrow-CopyConstructible
  /// @param h a handle to a resource to manage
  /// @param d a deleter to use to close/destroy the object
  raii_inline constexpr unique_rc(handle h, const Deleter &d) noexcept
    requires std::is_copy_constructible_v<Deleter>
    : uh_{ h, d }
  {}

  /// @brief Constructs a unique_rc object which owns h, initialising the stored handle with h
  /// and initializing a Deleter d, requires that Deleter is nothrow-MoveConstructible
  /// @param h a handle to a resource to manage
  /// @param d a deleter to use to close/destroy the object
  raii_inline constexpr unique_rc(handle h, Deleter &&d) noexcept
    requires std::conjunction_v<std::negation<std::is_reference<Deleter>>, std::is_move_constructible<Deleter>>
    : uh_{ h, std::move(d) }
  {}

  template<class D = Deleter>
    requires std::conjunction_v<std::is_reference<D>, std::is_constructible<D, std::remove_reference_t<D>>>
  unique_rc(handle, std::remove_reference_t<Deleter> &&) = delete;

  /// @brief Constructs a unique_rc by transferring ownership from src to *this and stores the invalid handle in src
  /// @param src other unique_rc transferring ownership from
  constexpr unique_rc(unique_rc && /*src*/) noexcept = default;

  /// @brief Constructs a unique_rc by transferring ownership from u to *this, where u is constructed with a specified
  /// deleter (D)
  /// @tparam H type of a u's handle
  /// @tparam D u's deleter type
  /// @param u unique_rc transferring ownership from
  /// @note Depends upon whether D is a reference type, as following: a) if D is a reference type, this deleter is
  /// copy constructed from u's deleter (requires that this construction does not throw), b) if D is a non-reference
  /// type, this deleter is move constructed from u's deleter (requires that this construction does not throw).
  template<typename H,
    class D,
    template<typename, typename> class TR,
    typename IH,
    template<typename, typename> class IHPolicy>
    requires std::conjunction_v<safe_conversion_from<H, D, TR, IH, IHPolicy>,
      std::conditional_t<std::is_reference_v<Deleter>, std::is_same<D, Deleter>, std::is_convertible<D, Deleter>>>
  // cppcheck-suppress noExplicitConstructor intended converting constructor
  raii_inline constexpr unique_rc(unique_rc<H, D, TR, IH, IHPolicy> &&u) noexcept
    : uh_{ u.release(), std::forward<D>(u.get_deleter()) }
  {}

  /// @brief Move assignment operator. Transfers ownership from rhs to *this as if by calling reset(rhs.release())
  /// followed by assigning get_deleter() from std::forward<Deleter>(rhs.get_deleter())
  /// @param rhs unique_rc transferring ownership from
  /// @return reference to *this unique_rc
  constexpr unique_rc &operator=(unique_rc && /*rhs*/) noexcept = default;

  /// @brief Converting move assignment operator. Transfers ownership from rhs to *this as if by calling
  /// reset(rhs.release()) followed by assigning get_deleter() from std::forward<D>(rhs.get_deleter())
  /// @tparam H type of a rhs handle
  /// @tparam D rhs deleter type
  /// @param rhs unique_rc transfers ownership from
  /// @return reference to *this unique_rc
  template<typename H,
    class D,
    template<typename, typename> class TR,
    typename IH,
    template<typename, typename> class IHPolicy>
    requires std::conjunction_v<safe_conversion_from<H, D, TR, IH, IHPolicy>, std::is_assignable<deleter_type &, D &&>>
  raii_inline constexpr unique_rc &operator=(unique_rc<H, D, TR, IH, IHPolicy> &&rhs) noexcept
  {
    reset(rhs.release());
    get_deleter() = std::forward<D>(rhs.get_deleter());

    return *this;
  }

  /// @brief Since unique_rc is move only type, copy constructor is explicitly deleted
  unique_rc(const unique_rc &) = delete;

  /// @brief Since unique_rc is move only type, copy assignment operator is explicitly deleted
  unique_rc &operator=(const unique_rc &) = delete;

  /// @brief If invalid_handle_policy::is_owned(h) is false there are no effects.
  /// Otherwise, the owned object is destroyed via get_deleter()(h).
  /// Requires that get_deleter()(h) does not throw exceptions.
  raii_inline constexpr ~unique_rc() noexcept
  {
    static_assert(std::is_invocable_v<deleter_type &, handle>, "unique_rc's deleter must be invocable with a handle");

    auto &h = uh_.get_handle();
    // NOLINTNEXTLINE(clang-diagnostic-unused-result)
    if (invalid_handle_policy::is_owned(h)) {
      get_deleter()(std::move(h));
      h = invalid();
    }
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
  }

  /// @brief provides access to object owned by *this
  /// @return handle to managed resource
  [[nodiscard]] raii_inline constexpr handle operator->() const noexcept
#ifndef RAII_NO_REQUIRE_CLASS_FOR_MEMBER_ACCESS_OPERATOR
    requires is_class_or_union<std::remove_pointer_t<handle>>
#endif// RAII_NO_REQUIRE_CLASS_FOR_MEMBER_ACCESS_OPERATOR
  {
    return get();
  }

  /// @brief operator* provides access to object owned by *this,
  /// provided it can be dereferenced
  /// @return dereferenced handle
  /// @note If get() is invalid, the behaviour is undefined
  [[nodiscard]] raii_inline constexpr typename std::add_lvalue_reference_t<std::remove_pointer_t<element_type>>
    operator*() const noexcept(noexcept(*std::declval<handle>()))
    requires can_reference<handle>
  {
#ifdef __cpp_lib_reference_from_temporary
    // 4148. unique_rc::operator* should not allow dangling references
    using ElemRefT = typename std::add_lvalue_reference_t<std::remove_pointer_t<element_type>>;
    using DerefT = decltype(*get());
    static_assert(
      !std::reference_converts_from_temporary_v<ElemRefT, DerefT>, "operator* must not return a dangling reference");
#endif
    assert(invalid_handle_policy::is_owned(get()) && "Cannot dereference invalid pointer");
    return *get();
  }

  /// @brief Returns a handle to the managed resource or invalid() if no resource is owned
  /// @return handle to the managed resource or invalid() if no resource is owned
  [[nodiscard]] raii_inline constexpr handle get() const noexcept { return uh_.get_handle(); }

  /// @brief Returns the deleter object which would be used for destruction of the managed resource
  /// @return reference to deleter
  [[nodiscard]] raii_inline constexpr deleter_type &get_deleter() noexcept { return uh_.get_deleter(); }

  /// @brief Returns the deleter object which would be used for destruction of the managed resource
  /// @return const reference to deleter object
  [[nodiscard]] raii_inline constexpr const deleter_type &get_deleter() const noexcept { return uh_.get_deleter(); }

  /// @brief Releases the ownership of the managed resource, if any. get() returns invalid after the call.
  /// @return handle to the managed resource or invalid if there was no managed resource
  /// @note The caller is responsible for cleaning up the object (e.g. by use of get_deleter())
  raii_inline constexpr handle release() noexcept { return uh_.release(); }

  /// @brief Replaces the managed resource.
  /// To replace the managed resource while supplying a new deleter as well, move assignment operator may be used.
  /// @param h handle to a new resource to manage
  /// @note A test for self-reset, i.e. whether handle points to an object already managed by *this, is not performed,
  /// except where provided as a compiler extension or as a debugging assert.
  /// @note code such as p.reset(p.release()) does not involve self-reset, only code like p.reset(p.get()) does
  raii_inline constexpr void reset(handle h = invalid()) noexcept
  {
    static_assert(std::is_invocable_v<deleter_type &, handle>, "unique_rc's deleter must be invocable with a handle");
    uh_.reset(std::move(h));
  }

  /// @brief Checks whether *this owns a resource, i.e. whether invalid_handle_policy::is_owned(get())
  /// @return true if *this owns a resource, false otherwise
  [[nodiscard]] raii_inline constexpr explicit operator bool() const noexcept
  {
    return invalid_handle_policy::is_owned(get());
  }

  /// @brief Represents invalid handle, may differ from handle, but must be assignable to one
  /// @return invalid handle which is assignable to handle
  [[nodiscard]] raii_inline static constexpr invalid_handle invalid() noexcept(
    noexcept(invalid_handle_policy::invalid()))
  {
    return invalid_handle_policy::invalid();
  }

  /// @brief Swaps the managed resource and associated deleters of *this and another unique_rc object other
  /// @param other another unique_rc object to swap the managed resource and the deleter with
  raii_inline constexpr void swap(unique_rc &other) noexcept(
    std::is_nothrow_swappable_v<Handle> && std::is_nothrow_swappable_v<Deleter>)
    requires std::is_swappable_v<Handle> && std::is_swappable_v<Deleter>
  {
    uh_.swap(other.uh_);
  }

private:
  unique_rc_holder<Handle, Deleter, TypeResolver, InvalidHandle, InvalidHandlePolicy> uh_;
};


/// @brief Compares if two unique_rc are equal, by comparing stored handles. Conditionally noexcept, provided operator==
/// for handles is itself noexcept
/// @tparam H1 the type of the object managed by lhs
/// @tparam D1 lhs' function object or lvalue reference to function or to function object, to be called from the
/// destructor
/// @tparam TR1 type which resolves lhs' handle
/// @tparam IH1 lhs' invalid handle type
/// @tparam IHPolicy1 lhs' invalid handle policy, provides is_owned(handle) and invalid() static methods
/// @tparam H2 the type of the object managed by rhs
/// @tparam D2 rhs' function object or lvalue reference to function or to function object, to be called from the
/// destructor
/// @tparam TR2 type which resolves rhs' handle
/// @tparam IH2 rhs' invalid handle type
/// @tparam IHPolicy2 rhs' invalid handle policy, provides is_owned(handle) and invalid() static methods
/// @param lhs left hand side unique_rc
/// @param rhs right hand side unique_rc
template<typename H1,
  class D1,
  template<typename, typename> class TR1,
  typename IH1,
  template<typename, typename> class IHPolicy1,
  typename H2,
  class D2,
  template<typename, typename> class TR2,
  typename IH2,
  template<typename, typename> class IHPolicy2>
  requires std::equality_comparable_with<typename unique_rc<H1, D1, TR1, IH1, IHPolicy1>::handle,
    typename unique_rc<H2, D2, TR2, IH2, IHPolicy2>::handle>
[[nodiscard]] raii_inline constexpr bool operator==(const unique_rc<H1, D1, TR1, IH1, IHPolicy1> &lhs,
  const unique_rc<H2, D2, TR2, IH2, IHPolicy2> &rhs) noexcept(noexcept(lhs.get() == rhs.get()))
{
  return lhs.get() == rhs.get();
}

// unique_rc comparison with nullptr
template<typename H,
  class D,
  template<typename, typename> class TR,
  typename IH,
  template<typename, typename> class IHPolicy>
  requires std::equality_comparable_with<typename unique_rc<H, D, TR, IH, IHPolicy>::handle, std::nullptr_t>
[[nodiscard]] raii_inline constexpr bool operator==(const unique_rc<H, D, TR, IH, IHPolicy> &lhs,
  std::nullptr_t) noexcept
{
  return !lhs;
}


/// @brief Performs three-way-comparison of two unique_rc, by three-way-comparing stored handles. Conditionally
/// noexcept, provided operator<=> for handles is itself noexcept
/// @tparam H1 the type of the object managed by lhs
/// @tparam D1 lhs' function object or lvalue reference to function or to function object, to be called from the
/// destructor
/// @tparam TR1 type which resolves lhs' handle
/// @tparam IH1 lhs' invalid handle type
/// @tparam IHPolicy1 lhs' invalid handle policy, provides is_owned(handle) and invalid() static methods
/// @tparam H2 the type of the object managed by rhs
/// @tparam D2 rhs' function object or lvalue reference to function or to function object, to be called from the
/// destructor
/// @tparam TR2 type which resolves rhs' handle
/// @tparam IH2 rhs' invalid handle type
/// @tparam IHPolicy2 rhs' invalid handle policy, provides is_owned(handle) and invalid() static methods
/// @param lhs left hand side unique_rc
/// @param rhs right hand side unique_rc
template<typename H1,
  class D1,
  template<typename, typename> class TR1,
  typename IH1,
  template<typename, typename> class IHPolicy1,
  typename H2,
  class D2,
  template<typename, typename> class TR2,
  typename IH2,
  template<typename, typename> class IHPolicy2>
  requires std::three_way_comparable_with<typename unique_rc<H1, D1, TR1, IH1, IHPolicy1>::handle,
    typename unique_rc<H2, D2, TR2, IH2, IHPolicy2>::handle>
[[nodiscard]] raii_inline constexpr std::compare_three_way_result_t<
  typename unique_rc<H1, D1, TR1, IH1, IHPolicy1>::handle,
  typename unique_rc<H2, D2, TR2, IH2, IHPolicy2>::handle>
  operator<=>(const unique_rc<H1, D1, TR1, IH1, IHPolicy1> &lhs,
    const unique_rc<H2, D2, TR2, IH2, IHPolicy2> &rhs) noexcept(noexcept(lhs.get() <=> rhs.get()))
{
  return lhs.get() <=> rhs.get();
}

template<typename H,
  class D,
  template<typename, typename> class TR,
  typename IH,
  template<typename, typename> class IHPolicy>
  requires std::three_way_comparable<typename unique_rc<H, D, TR, IH, IHPolicy>::handle>
[[nodiscard]] raii_inline constexpr std::compare_three_way_result_t<typename unique_rc<H, D, TR, IH, IHPolicy>::handle>
  operator<=>(const unique_rc<H, D, TR, IH, IHPolicy> &lhs, std::nullptr_t) noexcept(noexcept(lhs.get()))
{
  using handle = typename unique_rc<H, D, TR, IH, IHPolicy>::handle;
  return lhs.get() <=> static_cast<handle>(nullptr);
}

template<typename H,
  class D,
  template<typename, typename> class TR,
  typename IH,
  template<typename, typename> class IHPolicy>
  requires std::is_swappable_v<H> && std::is_swappable_v<D>
raii_inline constexpr void
  swap(unique_rc<H, D, TR, IH, IHPolicy> &lhs, unique_rc<H, D, TR, IH, IHPolicy> &rhs) noexcept(
    noexcept(std::is_nothrow_swappable_v<H> && std::is_nothrow_swappable_v<D>))
{
  lhs.swap(rhs);
}

template<typename H,
  class D,
  template<typename, typename> class TR,
  typename IH,
  template<typename, typename> class IHPolicy>
  requires(!std::is_swappable_v<H> || !std::is_swappable_v<D>)
void swap(unique_rc<H, D, TR, IH, IHPolicy> &lhs, unique_rc<H, D, TR, IH, IHPolicy> &rhs) = delete;


RAII_NS_END

namespace std {

/// @brief std::hash specialization for unique_rc.
/// @tparam H the type of the object managed by unique_rc
/// @tparam D function object or lvalue reference to function or to function object, to be called from the
/// destructor
/// @tparam TR type which resolves handle
/// @tparam IH invalid handle type, assignable to `handle`
/// @tparam IHPolicy invalid handle policy, provides is_owned(handle) and invalid() static methods
template<typename H,
  class D,
  template<typename, typename> class TR,
  typename IH,
  template<typename, typename> class IHPolicy>
struct hash<raii::unique_rc<H, D, TR, IH, IHPolicy>>
  : public raii::detail::unique_rc_hash_base<raii::unique_rc<H, D, TR, IH, IHPolicy>,
      typename raii::unique_rc<H, D, TR, IH, IHPolicy>::handle>
{
};

}// namespace std

#endif// UNIQUE_RC_HPP
