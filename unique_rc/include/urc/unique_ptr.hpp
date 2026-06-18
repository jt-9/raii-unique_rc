#ifndef UNIQUE_PTR_HPP
#define UNIQUE_PTR_HPP

#include "raii_defs.hpp"

#include "concepts.hpp"
#include "unique_rc.hpp"

#include <cassert>
#include <compare>// std::equality_comparable_with
#include <concepts>// std::swappable
#include <cstddef>// std::nullptr_t
#include <functional>// std::hash
#include <type_traits>// std::conjunction


RAII_NS_BEGIN

template<typename T> struct default_delete
{
  constexpr default_delete() noexcept = default;

  template<typename U>
    requires std::is_convertible_v<U *, T *>
  // cppcheck-suppress noExplicitConstructor; intended converting constructor
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  raii_inline constexpr default_delete(const default_delete<U> & /*src*/) noexcept
  {}

#ifdef __cpp_static_call_operator
  // False poisitive, guarded by feature #ifdef __cpp_static_call_operator
  // NOLINTNEXTLINE(clang-diagnostic-c++23-extensions)
  raii_inline static constexpr void operator()(T *ptr) noexcept
#else
  raii_inline constexpr void operator()(T *ptr) const noexcept
#endif
  {
    static_assert(!std::is_void_v<T>, "can't delete pointer to incomplete type");
    // cppcheck-suppress sizeofVoid;
    // NOLINTNEXTLINE(bugprone-sizeof-expression)
    static_assert(sizeof(T) > 0, "can't delete pointer to incomplete type");

    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete ptr;
  }
};

// Specialization of default_delete for arrays, used by 'unique_ptr<T[]>'
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays)
template<typename T> struct default_delete<T[]>
{
  constexpr default_delete() noexcept = default;

  template<typename U>
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays)
    requires std::is_convertible_v<U (*)[], T (*)[]>
  // cppcheck-suppress noExplicitConstructor; intended converting constructor
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, hicpp-explicit-conversions)
  raii_inline constexpr default_delete(const default_delete<U[]> & /*src*/) noexcept
  {}

  template<typename U>
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays)
    requires std::is_convertible_v<U (*)[], T (*)[]>
#ifdef __cpp_static_call_operator
  // False poisitive, guarded by feature #ifdef __cpp_static_call_operator
  // NOLINTNEXTLINE(clang-diagnostic-c++23-extensions)
  raii_inline static constexpr void operator()(U *ptr) noexcept
#else
  raii_inline constexpr void operator()(U *ptr) const noexcept
#endif
  {
    // cppcheck-suppress sizeofVoid;
    // NOLINTNEXTLINE(bugprone-sizeof-expression)
    static_assert(sizeof(U) > 0, "can't delete pointer to incomplete type");

    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete[] ptr;
  }
};


template<typename Pointer, class Del_noref> struct resolve_pointer_type
{
  using type = Pointer;
};

template<typename Pointer, class Del_noref>
  requires has_pointer_type<Del_noref>
struct resolve_pointer_type<Pointer, Del_noref>
{
  using type = Del_noref::pointer;
};


/**
 * @brief raii::unique_ptr is a smart pointer that owns (is responsible for) and manages another object via a pointer
 * and subsequently disposes of that object when the unique_ptr goes out of scope.
 *
 * The object is disposed of, using the associated deleter, when either of the following happens:

  * the managing unique_ptr object is destroyed.
  * the managing unique_ptr object is assigned another pointer via operator= or reset().

 * The object is disposed of, using a potentially user-supplied deleter, by calling get_deleter()(ptr).
 * The default deleter (raii::default_delete) uses the delete operator, which destroys the object and deallocates the
 * memory.
 * @tparam T the type of the object managed by this unique_ptr
 * @tparam Deleter the function object or lvalue reference to function object, to be called from the destructor
 * @note This manages single object (e.g., allocated with new).
 * @note There is no class template argument deduction from pointer type
 * because it is impossible to distinguish a pointer obtained from array and single forms of new.
 **/
template<typename T, class Deleter = raii::default_delete<T>>
  requires can_form_pointer<T>
class unique_ptr : public unique_rc<T *, Deleter, resolve_pointer_type, std::nullptr_t>
{
private:
  using Base = unique_rc<T *, Deleter, resolve_pointer_type, std::nullptr_t>;
  using typename Base::handle;

public:
  using invalid_pointer_policy = Base::invalid_handle_policy;

  /// @brief std::remove_reference<Deleter>::type::pointer if that type exists, otherwise T*. Must satisfy
  /// `NullablePointer`
  using pointer = Base::handle;

  /// @brief T, the type of the object managed by this unique_ptr
  using element_type = T;

  /// @brief Deleter, the function object or lvalue reference or to function object, to be called from the destructor
  using typename Base::deleter_type;

  /// @brief Represents invalid handle type, whose value is returned by Deleter::invalid()
  /// is assigned to *this handle, when it goes out of scope, or upon reset(), usually std::nullptr_t
  using invalid_pointer = Base::invalid_handle;

  /// @brief static method returns invalid pointer of type `invalid_pointer`, usually `nullptr`
  using Base::invalid;

private:
  /// @brief helper template for detecting a safe conversion from another unique_ptr
  template<typename U, class D>
  using safe_conversion_from =
    std::conjunction<std::is_convertible<typename unique_ptr<U, D>::pointer, pointer>, std::negation<std::is_array<U>>>;

public:
  // False positive while calling base class constructor, it interprets as new function with the same name which hides
  // implementation in base class
  // cppcheck-suppress-begin [functionStatic, missingReturn, duplInheritedMember]
  raii_inline constexpr unique_ptr() noexcept
    requires not_pointer_and_is_default_constructable_v<Deleter>
    : Base()
  {}

  raii_inline explicit constexpr unique_ptr(pointer ptr) noexcept
    requires not_pointer_and_is_default_constructable_v<Deleter>
    : Base(ptr)
  {}

  raii_inline constexpr unique_ptr(pointer ptr, const Deleter &del) noexcept
    requires std::is_copy_constructible_v<Deleter>
    : Base(ptr, del)
  {}

  raii_inline constexpr unique_ptr(pointer ptr, Deleter &&del) noexcept
    requires std::conjunction_v<std::negation<std::is_reference<Deleter>>, std::is_move_constructible<Deleter>>
    : Base(ptr, std::move(del))
  {}
  // cppcheck-suppress-end [functionStatic, missingReturn, duplInheritedMember]

  template<class D = Deleter>
    requires std::conjunction_v<std::is_reference<D>, std::is_constructible<D, std::remove_reference_t<D>>>
  unique_ptr(pointer, std::remove_reference_t<Deleter> &&) = delete;

  constexpr unique_ptr(unique_ptr && /*src*/) noexcept = default;

  // Converting constructor from another type
  template<typename U, class D>
    requires std::conjunction_v<safe_conversion_from<U, D>,
      std::conditional_t<std::is_reference_v<Deleter>, std::is_same<D, Deleter>, std::is_convertible<D, Deleter>>>
  raii_inline
#ifndef RAII_URC_ENABLE_IMPLICIT_CONVERTING_CONSTRUCTOR
    explicit
#endif
    constexpr unique_ptr(unique_ptr<U, D> &&ptr) noexcept
    : Base(std::move(ptr))
  {}

  constexpr unique_ptr &operator=(unique_ptr && /*rhs*/) noexcept = default;

  // Assignment from another type
  template<typename U, class D>
    requires std::conjunction_v<safe_conversion_from<U, D>, std::is_assignable<deleter_type &, D &&>>
  raii_inline constexpr unique_ptr &operator=(unique_ptr<U, D> &&rhs) noexcept
  {
    Base::operator=(std::move(rhs));

    return *this;
  }

  raii_inline constexpr unique_ptr &operator=(std::nullptr_t) noexcept
    requires std::is_same_v<invalid_pointer, std::nullptr_t>
  {
    reset();
    return *this;
  }

  unique_ptr(const unique_ptr &) = delete;
  unique_ptr &operator=(const unique_ptr &) = delete;

  constexpr ~unique_ptr() noexcept = default;

  using Base::get;
  using Base::operator->;
  using Base::operator*;

  using Base::get_deleter;
  using Base::operator bool;

  using Base::reset;
  using Base::release;

  using Base::swap;
};


/**
 * @brief raii::unique_ptr is a smart pointer that owns (is responsible for) and manages another object via a pointer
 * and subsequently disposes of that object when the unique_ptr goes out of scope.
 * @tparam T the type of the object, array of which is managed by this unique_ptr
 * @tparam Deleter the function object or lvalue reference to function object, to be called from the destructor
 * @note This specialisation manages dynamically-allocated array of objects (e.g., allocated with new[]).
 **/
template<typename T, class Deleter>
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions, cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays)
class unique_ptr<T[], Deleter> : public unique_rc<T *, Deleter, resolve_pointer_type, std::nullptr_t>
{
private:
  using Base = unique_rc<T *, Deleter, resolve_pointer_type, std::nullptr_t>;

  template<typename U>
  using is_derived_T =
    std::conjunction<std::is_base_of<T, U>, std::negation<std::is_same<std::remove_cv_t<T>, std::remove_cv_t<U>>>>;

public:
  using invalid_pointer_policy = Base::invalid_handle_policy;

  /// @brief std::remove_reference<Deleter>::type::pointer if that type exists, otherwise T*. Must satisfy
  /// `NullablePointer`
  using pointer = Base::handle;

  /// @brief T, the type of the object managed by this unique_ptr
  using element_type = T;

  /// @brief Deleter, the function object or lvalue reference or to function object, to be called from the destructor
  using typename Base::deleter_type;

  /// @brief Represents invalid handle type, whose value is returned by Deleter::invalid()
  /// is assigned to *this handle, when it goes out of scope, or upon reset(), usually std::nullptr_t
  using invalid_pointer = Base::invalid_handle;

  /// @brief static method returns invalid pointer of type `invalid_pointer`, usually `nullptr`
  using Base::invalid;

  /// @brief helper template for detecting a safe conversion from a raw pointer
  template<typename U>
  using safe_conversion_raw =
    std::conjunction<std::disjunction<std::disjunction<std::is_same<U, pointer>, std::is_same<U, std::nullptr_t>>,
      std::conjunction<std::is_pointer<U>,
        std::is_same<pointer, element_type *>,
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays)
        std::is_convertible<typename std::remove_pointer_t<U> (*)[], element_type (*)[]>>>>;

  /// @brief helper template for detecting a safe conversion from another unique_ptr
  template<typename U,
    class D,
    typename UP = unique_ptr<U, D>,
    typename UP_pointer = UP::pointer,
    typename UP_element_type = UP::element_type>
  using safe_conversion_from = std::conjunction<std::is_array<U>,
    std::is_same<pointer, element_type *>,
    std::is_same<UP_pointer, UP_element_type *>,
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays)
    std::is_convertible<UP_element_type (*)[], element_type (*)[]>>;

  // False positive while calling base class constructor, it interprets as new function with the same name which hides
  // implementation in base class
  // cppcheck-suppress-begin [functionStatic, missingReturn, duplInheritedMember]

  // Constructors
  raii_inline constexpr unique_ptr() noexcept
    requires not_pointer_and_is_default_constructable_v<Deleter>
    : Base()
  {}

  template<typename U>
  raii_inline constexpr explicit unique_ptr(U ptr) noexcept
    requires std::conjunction_v<not_pointer_and_is_default_constructable<Deleter>, safe_conversion_raw<U>>
    : Base(ptr)
  {}

  template<typename U>
  raii_inline constexpr unique_ptr(U ptr, const deleter_type &del) noexcept
    requires std::conjunction_v<safe_conversion_raw<U>, std::is_copy_constructible<deleter_type>>
    : Base(ptr, del)
  {}

  template<typename U, class D = deleter_type>
  raii_inline constexpr unique_ptr(U ptr, std::enable_if_t<!std::is_lvalue_reference_v<D>, D &&> del) noexcept
    requires std::conjunction_v<safe_conversion_raw<U>, std::is_move_constructible<D>>
    : Base(std::move(ptr), std::move(del))
  {}
  // cppcheck-suppress-end [functionStatic, missingReturn, duplInheritedMember]

  template<typename U, class D = deleter_type>
    requires std::conjunction_v<safe_conversion_raw<U>, std::is_lvalue_reference<D>>
  unique_ptr(U, std::remove_reference_t<D> &&) = delete;

  // False positive on move constructor
  // cppcheck-suppress noExplicitConstructor
  constexpr unique_ptr(unique_ptr && /*src*/) noexcept = default;

  /// Creates a unique_ptr that owns nothing.
  template<class D = Deleter>
    requires not_pointer_and_is_default_constructable_v<D>
  raii_inline explicit constexpr unique_ptr(std::nullptr_t) noexcept : Base()
  {}

  // Converting constructor from another type
  template<typename U, class D>
    requires std::conjunction_v<safe_conversion_from<U, D>,
      std::conditional_t<std::is_reference_v<Deleter>, std::is_same<D, Deleter>, std::is_convertible<D, Deleter>>>
  raii_inline
#ifndef RAII_URC_ENABLE_IMPLICIT_CONVERTING_CONSTRUCTOR
    explicit
#endif
    constexpr unique_ptr(unique_ptr<U, D> &&ptr) noexcept
    : Base(std::move(ptr))
  {}

  constexpr unique_ptr &operator=(unique_ptr && /*rhs*/) noexcept = default;

  template<typename U, class D>
    requires std::conjunction_v<safe_conversion_from<U, D>, std::is_assignable<deleter_type &, D &&>>
  raii_inline constexpr unique_ptr &operator=(unique_ptr<U, D> &&rhs) noexcept
  {
    Base::operator=(std::move(rhs));
    return *this;
  }

  raii_inline constexpr unique_ptr &operator=(std::nullptr_t) noexcept
    requires std::is_same_v<invalid_pointer, std::nullptr_t>
  {
    reset();
    return *this;
  }

  /// @brief Destructor, invokes the deleter if the stored pointer is not invalid
  constexpr ~unique_ptr() noexcept = default;

  using Base::get;

  /// @brief deleted for array specialisation, single object version only
  constexpr pointer operator->() const noexcept(noexcept(get())) = delete;

  /// @brief deleted for array specialisation, single object version only
  constexpr std::add_lvalue_reference_t<element_type> operator*() const = delete;

  /// @brief Provides access to elements of an array managed by a unique_ptr.
  /// @note Parameter index shall be less than the number of elements in the array;
  /// otherwise, the behavior is undefined.
  /// @param index the index of the element to be returned
  /// @return lvalue reference the element at index, i.e. get()[index]
  raii_inline constexpr std::add_lvalue_reference_t<element_type> operator[](std::size_t index) const
  {
    assert(invalid_pointer_policy::is_owned(get()) && "Error subscript operator on invalid pointer");
    return get()[index];
  }

  using Base::get_deleter;
  using Base::operator bool;

  using Base::release;

  template<typename U>
    requires std::disjunction_v<std::is_same<U, pointer>,
      std::conjunction<std::is_same<pointer, element_type *>,
        std::is_pointer<U>,
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays)
        std::is_convertible<typename std::remove_pointer_t<U> (*)[], element_type (*)[]>>>
  /// @brief Replaces the managed array, delegates to `unique_rc::reset`
  /// @param ptr pointer to a new array to manage
  /// @note To replace the managed object while supplying a new deleter as well, move assignment operator may be used
  raii_inline constexpr void reset(U ptr) noexcept
  { Base::reset(std::move(ptr)); }

  /// @brief Replaces the managed array. Equivalent to `reset(pointer())`
  /// @param nullptr or none
  raii_inline constexpr void reset(std::nullptr_t = nullptr) noexcept
    requires std::is_same_v<invalid_pointer, std::nullptr_t>
  { Base::reset(invalid()); }

  using Base::swap;
};// unique_ptr<T[], Deleter>


template<typename T1, class D1, typename T2, class D2>
  requires std::equality_comparable_with<T1, T2>
[[nodiscard]] raii_inline constexpr bool operator==(const unique_ptr<T1, D1> &lhs,
  const unique_ptr<T2, D2> &rhs) noexcept(noexcept(lhs.get() == rhs.get()))
{
  // cppcheck-suppress mismatchingContainers - false positive guarded by concept std::equality_comparable_with
  return lhs.get() == rhs.get();
}

// unique_ptr comparison with nullptr
template<typename T, class D>
[[nodiscard]] raii_inline constexpr bool operator==(const unique_ptr<T, D> &lhs, std::nullptr_t) noexcept
{ return !lhs; }

template<typename T1, class D1, typename T2, class D2>
  requires std::three_way_comparable_with<typename unique_ptr<T1, D1>::pointer, typename unique_ptr<T2, D2>::pointer>
[[nodiscard]] raii_inline constexpr std::compare_three_way_result_t<typename unique_ptr<T1, D1>::pointer,
  typename unique_ptr<T2, D2>::pointer>
  operator<=>(const unique_ptr<T1, D1> &lhs, const unique_ptr<T2, D2> &rhs) noexcept(noexcept(lhs.get() <=> rhs.get()))
{
  // cppcheck-suppress mismatchingContainers - false positive guarded by concept std::three_way_comparable_with
  return lhs.get() <=> rhs.get();
}

template<typename T, class D>
  requires std::three_way_comparable<typename unique_ptr<T, D>::pointer>
[[nodiscard]] raii_inline constexpr std::compare_three_way_result_t<typename unique_ptr<T, D>::pointer>
  operator<=>(const unique_ptr<T, D> &lhs, std::nullptr_t) noexcept(noexcept(lhs.get()))
{
  using pointer = unique_ptr<T, D>::pointer;
  return lhs.get() <=> static_cast<pointer>(nullptr);
}

template<typename H, class D>
  requires /*std::is_swappable_v<D> ||*/ std::swappable<D>
raii_inline constexpr void swap(unique_ptr<H, D> &lhs, unique_ptr<H, D> &rhs) noexcept(
  noexcept(std::is_nothrow_swappable_v<D>))
{ lhs.swap(rhs); }

template<typename H, class D>
  requires std::negation_v<std::is_swappable<D>>
void swap(unique_ptr<H, D> &lhs, unique_ptr<H, D> &rhs) = delete;


// make_unique and make_unique_for_overwrite

template<class T, class... Types>
  requires(!std::is_array_v<T>)
[[nodiscard]] raii_inline constexpr raii::unique_ptr<T> make_unique(Types &&...Args)
{
  // NOLINTNEXTLINE(clang-diagnostic-missing-field-initializers)
  return raii::unique_ptr<T>{ new T{ std::forward<Types>(Args)... } };
}

template<class T>
// requires std::is_array_v<T> && (std::extent_v<T> == 0)
  requires std::is_unbounded_array_v<T>
[[nodiscard]] raii_inline constexpr raii::unique_ptr<T> make_unique(const std::size_t size)
{
  using Elem = std::remove_extent_t<T>;
  return raii::unique_ptr<T>{ new Elem[size]{} };
}

template<class T, class... Types>
  requires std::is_bounded_array_v<T>
void make_unique(Types &&...) = delete;

template<typename T>
  requires(!std::is_array_v<T>)
[[nodiscard]] raii_inline constexpr raii::unique_ptr<T> make_unique_for_overwrite()
{
  // with default initialization
  return raii::unique_ptr<T>{ new T };
}

template<typename T>
  requires std::is_unbounded_array_v<T>
[[nodiscard]] raii_inline constexpr raii::unique_ptr<T> make_unique_for_overwrite(const std::size_t size)
{
  // only memory allocation
  using Elem = std::remove_extent_t<T>;
  return raii::unique_ptr<T>{ new Elem[size] };
}

template<typename T, class... Types>
  requires std::is_bounded_array_v<T>
void make_unique_for_overwrite(Types &&...) = delete;

RAII_NS_END


// std::hash specialization for unique_rc.
template<typename H, class D>
struct std::hash<raii::unique_ptr<H, D>>
  : public raii::detail::unique_rc_hash_base<raii::unique_ptr<H, D>, typename raii::unique_ptr<H, D>::pointer>
{
};

#endif// UNIQUE_PTR_HPP
