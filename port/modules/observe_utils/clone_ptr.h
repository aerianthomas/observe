#pragma once

#include <memory>

namespace observe
{
  template <class T>
  class clone_ptr
  {
  public:
    static_assert(std::is_copy_constructible_v<T>, "T must be copy constructible for clone_ptr");

    clone_ptr() = default;
    clone_ptr(std::unique_ptr<T> p) : p_(std::move(p)) {}

    clone_ptr(const clone_ptr &other) : p_(other.p_ ? std::make_unique<T>(*other.p_) : nullptr) {}

    clone_ptr &operator=(const clone_ptr &other)
    {
      if (this != &other)
        p_ = other.p_ ? std::make_unique<T>(*other.p_) : nullptr;
      return *this;
    }

    clone_ptr(clone_ptr &&) noexcept = default;
    clone_ptr &operator=(clone_ptr &&) noexcept = default;

    void reset() { p_.reset(); }
    void reset(std::unique_ptr<T> p) { p_ = std::move(p); }
    void swap(clone_ptr &other) noexcept { p_.swap(other.p_); }
    T *get() const { return p_.get(); }
    T &operator*() const { return p_.operator*(); }
    T *operator->() const { return p_.operator->(); }
    explicit operator bool() const { return static_cast<bool>(p_); }

  private:
    std::unique_ptr<T> p_;
  };
}
