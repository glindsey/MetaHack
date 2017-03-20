#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "stdafx.h"

/// Everything in the game inherits from GameObject.
class GameObject
{
public:
  GameObject() {}
  GameObject(GameObject const&) = delete;
  GameObject(GameObject&&) = delete;
  GameObject& operator=(GameObject const&) = delete;
  GameObject& operator=(GameObject&&) = delete;

  virtual ~GameObject() {}

  /// Return this game object's description.
  virtual std::string get_display_name() const = 0;

protected:
private:
};

#endif // GAMEOBJECT_H
