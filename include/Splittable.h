#ifndef SPLITTABLE_H
#define SPLITTABLE_H

/// CRTP template class which allows us to implement the
/// Curiously Recurring Template Pattern for Things.
template <typename Base, typename Derived>
class Splittable
{
  public:
    virtual Base* split(unsigned int number) const
    {
      return new Derived(static_cast<Derived const &>(*this, number));
    }
};

#endif // SPLITTABLE_H
