#pragma once

#include <type_traits>

namespace freeisle::core {

/**
 * A simple wrapper around a raw pointer which behaves exactly
 * the same as one, except it propagates const-ness of the
 * object it points to:
 *
 * If the Ptr is non-const, it points to T.
 * If the Ptr is const, it points to const T.
 */
template <typename T> class Ptr {
public:
  Ptr() : ptr(nullptr) {}

  template <typename U> Ptr(U *p) : ptr(p) {}

  template <typename U> Ptr(Ptr<U> other) : ptr(&*other) {}

  template <typename U,
            typename std::enable_if<std::is_const<U>::value, U>::type = 0>
  Ptr(const U *p) : ptr(p) {}

  template <typename U,
            typename std::enable_if<std::is_const<U>::value, U>::type = 0>
  Ptr(const Ptr<U> &other) : ptr(other.ptr) {}

  /*Ptr& operator=(T* p) { ptr = p; return *this; }
  Ptr& operator=(Ptr<T>&& other) = default;

  template<typename std::enable_if<std::is_const<T>::value, T>::type = 0>
  Ptr& operator=(const T* p) { ptr = p; return *this; }

  template<typename std::enable_if<std::is_const<T>::value, T>::type = 0>
  Ptr& operator=(const Ptr<T>& other) { ptr = other.ptr; return *this; }*/

  T *operator->() { return ptr; }
  const T *operator->() const { return ptr; }

  T &operator*() { return *ptr; }
  const T &operator*() const { return *ptr; }

  operator T *() { return ptr; }
  operator const T *() const { return ptr; }

private:
  T *ptr;
};

} // namespace freeisle::core
