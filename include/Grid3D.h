#include "stdafx.h"

#include "Vec3.h"

/// Three-dimensional grid of some sort of object.
/// The grid is stored linearly in coordinate order (z, y, x).
/// The grid owns the objects inside it.
template <class Object>
class Grid3D
{
  /// Using declaration for a ctor function for objects.
  using ObjectCtor = std::function<Object*(Vec3i)>;
public:
  /// Constructor for the grid.
  /// @param size   The size of the grid to construct.
  /// @param ctor   A function that takes a Vec3i as an input and
  ///               returns a pointer to a new object of type Object.
  ///               The Grid2D class will take ownership of this object.
  Grid3D(Vec3i size, ObjectCtor ctor)
    :
    m_size(size)
  {
    for (int z = 0; z < size.z; ++z)
    {
      for (int y = 0; y < size.y; ++y)
      {
        for (int x = 0; x < size.x; ++x)
        {
          Object* new_object = ctor({ x, y, z });
          m_objects.push_back(new_object);
        }
      }
    }
  }

  /// Get a reference to an object in the grid.
  Object& get(Vec3i coords)
  {
    return m_objects[index(coords)];
  }

protected:
  /// Get the index to an object given x/y/z coords.
  unsigned int index(Vec3i coords)
  {
    return (coords.z * m_size.y * m_size.x) + (coords.y * m_size.x) + coords.x;
  }

private:
  /// Pointer vector of objects.
  boost::ptr_vector< Object > m_objects;

  /// Size of the grid.
  Vec3i m_size;

};