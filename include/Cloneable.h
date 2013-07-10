#ifndef CLONEABLE_H
#define CLONEABLE_H

/// CRTP template class which allows us to implement the
/// Curiously Recurring Template Pattern for Things.
template <typename Base, typename Derived>
class Cloneable
{
  public:
    virtual Base* clone() const
    {
      return new Derived(static_cast<Derived const &>(*this));
    }
};

#endif // CLONEABLE_H
