#include "stdafx.h"

#include "Vec2.h"

/// Two-dimensional grid of some sort of object.
/// The grid is stored linearly in coordinate order (y, x).
/// The grid owns the objects inside it.
template <class Object>
class Grid2D
{
  /// Using declaration for a ctor function for objects.
  using ObjectCtor = std::function<Object*(IntegerVec2)>;
public:
  /// Constructor for the grid.
  /// @param size   The size of the grid to construct.
  /// @param ctor   A function that takes a IntegerVec2 as an input and
  ///               returns a pointer to a new object of type Object.
  ///               The Grid2D class will take ownership of this object.
  Grid2D(IntegerVec2 size, ObjectCtor ctor)
    :
    m_size(size)
  {
    for (int y = 0; y < size.y; ++y)
    {
      for (int x = 0; x < size.x; ++x)
      {
        Object* new_object = ctor({ x, y });
        m_objects.push_back(new_object);
      }
    }
  }

  /// Get a reference to an object in the grid.
  Object& get(IntegerVec2 coords)
  {
    return m_objects[index(coords)];
  }

protected:
  /// Get the index to an object given x/y coords.
  unsigned int index(IntegerVec2 coords)
  {
    return (coords.y * m_size.x) + coords.x;
  }

private:
  /// Pointer vector of objects.
  boost::ptr_vector< Object > m_objects;

  /// Size of the grid.
  IntegerVec2 m_size;

};