#ifndef USESPIMPL_H
#define USESPIMPL_H

#include <memory>

#include "New.h"

/// A template that helps classes that use the PIMPL idiom by getting rid of
/// much of the boilerplate. This template disallows PIMPL copying, and so
/// should be used in non-copyable classes.
template<typename P>
class Pimpl
{
  std::unique_ptr<P> p;

public:
  /// Default constructor.
  Pimpl() : p{ NEW P{} }  {}

  /// Pointer constructor.
  Pimpl(P* ptr) : p{ ptr }  {}

  /// Templated pImpl initializer.
  template<typename ...Args> Pimpl(Args&& ...) : p{ NEW P{ std::forward<Args>(args)... } }  {}

  /// Destructor
  ~Pimpl()  { }

  /// Copy constructor
  Pimpl(const Pimpl& other) = delete;

  /// Move constructor
  Pimpl(Pimpl<P>&&) = default;

  /// Copy assignment operator
  Pimpl<P>& operator= (const Pimpl<P>& other) = delete;

  /// Move assignment operator
  Pimpl<P>& operator= (Pimpl<P>&&) = default;

  /// Get a pointer to the underlying pImpl data.
  P* operator->() { return p.get(); }

  /// Get a pointer to the underlying pImpl data.
  P const* operator->() const { return p.get(); }

  /// Get a reference to the underlying pImpl data.
  P& operator*() { return *(p.get()); }

  /// Get a reference to the underlying pImpl data.
  P const& operator*() const { return *(p.get()); }
};

/// A template that helps classes that use the PIMPL idiom by getting rid of
/// much of the boilerplate. This template allows for PIMPL copying.
template<typename P>
class CopyablePimpl
{
  std::unique_ptr<P> p;

public:
  /// Default constructor.
  CopyablePimpl() : p{ NEW P{} } {}

  /// Pointer constructor.
  CopyablePimpl(P* ptr) : p{ ptr } {}

  /// Templated pImpl initializer.
  template<typename ...Args> CopyablePimpl(Args&& ...) : p{ NEW P{ std::forward<Args>(args)... } } {}

  /// Destructor
  ~CopyablePimpl() { }

  /// Copy constructor
  CopyablePimpl(const CopyablePimpl& other) : p{ NEW P{ *other.p } } {}

  /// Move constructor
  CopyablePimpl(CopyablePimpl<P>&&) = default;

  /// Copy assignment operator
  CopyablePimpl<P>& operator= (const CopyablePimpl<P>& other) { p.reset(NEW P(*other.p)); return *this; }

  /// Move assignment operator
  CopyablePimpl<P>& operator= (CopyablePimpl<P>&&) = default;

  /// Get a pointer to the underlying pImpl data.
  P* operator->() { return p.get(); }

  /// Get a pointer to the underlying pImpl data.
  P const* operator->() const { return p.get(); }

  /// Get a reference to the underlying pImpl data.
  P& operator*() { return *(p.get()); }

  /// Get a reference to the underlying pImpl data.
  P const& operator*() const { return *(p.get()); }
};

#endif // USESPIMPL_H
