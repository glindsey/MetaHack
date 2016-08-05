#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "stdafx.h"

/// Everything in the game inherits from GameObject.
class GameObject
  :
  public boost::noncopyable
{
public:
  GameObject();
  virtual ~GameObject();

  /// Return this game object's description.
  virtual StringDisplay get_display_name() const = 0;

protected:
private:
};

#endif // GAMEOBJECT_H
