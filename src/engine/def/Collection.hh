#pragma once

#include <cassert>
#include <map>
#include <optional>
#include <set>
#include <string>

namespace freeisle::def {

/**
 * A collection of objects of type T. The collection is keyed by object
 * IDs of type string.
 */
template <typename T> using Collection = std::map<std::string, T>;

/**
 * A reference to an object in a collection. It can only be copied from
 * non-const reference for const correctness. Use a Ref<const T> if you have
 * a const Ref and need to make a copy.
 */
template <typename T> class Ref {
public:
  /**
   * Construct an object reference from an iterator into the collection.
   */
  Ref(typename Collection<T>::iterator iter) : iter(iter) {}

  Ref(Ref &other) noexcept : iter(other.iter) {}
  Ref(Ref &&other) noexcept : iter(other.iter) {}
  Ref(const Ref &) = delete;

  Ref &operator=(Ref<T> &other) noexcept {
    iter = other.iter;
    return *this;
  }
  Ref &operator=(Ref<T> &&other) noexcept {
    iter = other.iter;
    return *this;
  }
  Ref &operator=(const Ref<T> &) = delete;

  /**
   * Return the object ID of the referred object.
   */
  const std::string &id() const { return iter->first; }

  T &operator*() { return iter->second; }
  const T &operator*() const { return iter->second; }
  T *operator->() { return &iter->second; }
  const T *operator->() const { return &iter->second; }

  /**
   * Compare to other references. A reference compares less than another
   * reference if its object ID compares less than the other reference's
   * object ID.
   */
  struct Compare {
    bool operator()(const Ref &a, const Ref &b) const {
      return a.iter->first < b.iter->first;
    }

    bool operator()(const Ref &a,
                    const typename Collection<T>::iterator &b) const {
      return a.iter->first < b->first;
    }

    bool operator()(const Ref &a,
                    const typename Collection<T>::const_iterator &b) const {
      return a.iter->first < b->first;
    }

    bool operator()(const typename Collection<T>::const_iterator &a,
                    const Ref &b) const {
      return a->first < b.iter->first;
    }

    bool operator()(const typename Collection<T>::iterator &a,
                    const Ref &b) const {
      return a->first < b.iter->first;
    }

    typedef Ref is_transparent;
  };

private:
  typename Collection<T>::iterator iter;
};

/**
 * Same as a Ref<T>, but only provides const access to the underlying
 * object.
 */
template <typename T> class Ref<const T> {
public:
  Ref(typename Collection<T>::const_iterator iter) : iter(iter) {}

  /**
   * Return the object ID of the referred object.
   */
  const std::string &id() const { return iter->first; }

  const T &operator*() const { return iter->second; }
  const T *operator->() const { return &iter->second; }

  /**
   * Augment this reference to a non-const ref. This needs a non-const
   * collection.
   */
  Ref<T> augment(Collection<T> &collection) {
    assert(collection.find(iter->first) == iter);
    return collection.find(iter->first);
  }

private:
  typename Collection<T>::const_iterator iter;
};

/**
 * A reference to an object in a collection which is allowed to not point to
 * any object.
 */
template <typename T> class NullableRef {
public:
  NullableRef() noexcept = default;
  NullableRef(typename Collection<T>::iterator iter) noexcept : ref_(iter) {}
  NullableRef(Ref<T> ref) noexcept : ref_(std::move(ref)) {}

  /**
   * Returns whether the NullableRef points to a valid object or not.
   */
  explicit operator bool() const { return ref_.has_value(); }

  /**
   * Returns whether the NullableRef is invalid or not.
   */
  bool operator!() const { return !ref_.has_value(); }

  /**
   * Return the object ID of the referred object.
   */
  const std::string &id() const {
    assert(ref_);
    return ref_->id();
  }

  const T &operator*() const {
    assert(ref_);
    return **ref_;
  }
  const T *operator->() const {
    assert(ref_);
    return &**ref_;
  }

  /**
   * Return a non-const reference to the referenced object. This is only
   * safe if the caller can provide a reference to the (non-const) collection
   * containing the referenced object.
   */
  T &get(Collection<T> &collection) { return ref_->get(collection); }

private:
  std::optional<Ref<T>> ref_;
};

/**
 * A RefMap is a mapping of object references to some type T. This is used
 * when there exists collection of type T, and for each element in the
 * collection, additional data needs to be maintained. In this case, every
 * entry in the original collection typically has one corresponding entry
 * in the RefMap.
 */
template <typename C, typename T>
using RefMap = std::map<Ref<C>, T, typename Ref<C>::Compare>;

/**
 * A set of object references. Typically used to create a subset of a
 * collection.
 */
template <typename T> using RefSet = std::set<Ref<T>, typename Ref<T>::Compare>;

/**
 * Create a collection from a fixed number of pairs. This can be used instead
 * of construction with an initializer list if the type T is not
 * copy-constructible.
 */
template <typename T, typename... Args>
Collection<T> make_collection(Args &&... args) {
  Collection<T> collection;
  (collection.insert(std::forward<Args>(args)), ...);
  return collection;
}

/**
 * Create a refset from a fixed number of elements T. This can be used
 * instead of construction with an initilalizer list if the type T is not
 * copy-constructible.
 */
template <typename T, typename... Args>
RefSet<T> make_ref_set(Args &&... args) {
  RefSet<T> set;
  (set.insert(std::forward<Args>(args)), ...);
  return set;
}

} // namespace freeisle::def
