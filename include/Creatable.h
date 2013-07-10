#ifndef CREATABLE_H
#define CREATABLE_H

/// CRTP template class which allows us to implement the
/// Curiously Recurring Template Pattern for Things.
template <typename Base, typename Derived>
class Creatable
{
  public:
    static Base* create()
    {
      return new Derived();
    }
};

#endif // CLONEABLE_H
