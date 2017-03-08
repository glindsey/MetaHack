#pragma once

/// @file Templates implementing the Visitor Pattern in C++11.
///       Shamelessly copied from:
///       https://stackoverflow.com/questions/11796121/implementing-the-visitor-pattern-using-c-templates
///       with a few small modifications by me.

/// Visitor template declaration
template<typename... Types>
class Visitor;

/// Visitor template specialization for single type
template<typename T>
class Visitor<T>
{
public:
  virtual void visit(T& visitable) = 0;
};

/// Visitor template specialization for multiple types
template<typename T, typename... Types>
class Visitor<T, Types...> : public Visitor<Types...>
{
public:
  // promote the function(s) from the base class
  using Visitor<Types...>::visit;

  virtual void visit(T& visitable) = 0;
};

/// A template class declaring that a subclass is Visitable.
/// @tparam Types     A list of Visitor types this class can accept.
template<typename... Types>
class Visitable
{
public:
  virtual void be_visited_by(Visitor<Types...>& visitor) = 0;
};

/// The implementation of a Visitable class.
/// For example:
///     class Foo: public Base, public VisitableImpl<Foo, Foo, Bar>
///
/// This specifies that the class `Foo` is derived from the class `Base` and
/// can accept a Visitor<Foo, Bar> class instance.
///
/// @tparam Derived   The name of the class.
/// @tparam Types     The list of types that the Visitor can visit.
template<typename Derived, typename... Types>
class VisitableImpl : public Visitable<Types...>
{
public:
  virtual void be_visited_by(Visitor<Types...>& visitor) override
  {
    visitor.visit(static_cast<Derived&>(*this));
  }
};