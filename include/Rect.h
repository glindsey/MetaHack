#pragma once

#include "SFML/Graphics.hpp"

#include "Vec2.h"

/// Definition of a rectangle.
template <typename T>
class Rect
{
public:
  Rect()
  {}

  Rect(T x_, T y_, T width_, T height_)
    :
    x{ x_ }, y{ y_ }, width{ width_ }, height{ height_ }
  {}


  virtual ~Rect() = default;
  Rect(Rect const& other) = default;
  Rect(Rect&& other) = default;
  Rect& operator=(Rect const& other) = default;
  Rect& operator=(Rect&& other) = default;

  friend bool operator==(Rect const& lhs, Rect const& rhs)
  {
    return ((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.width == rhs.width) && (lhs.height == rhs.height));
  }


  friend bool operator!=(Rect const& lhs, Rect const& rhs)
  {
    return !(lhs == rhs);
  }

  friend std::ostream& operator<<(std::ostream& os, Rect const& obj)
  {
    os << "(" << obj.x << ", " << obj.y << ")+(" << obj.width << ", " << obj.height << ")";
    return os;
  }

  T area()
  {
    return (x + width) * (y + height);
  }

  T left()
  {
    return x;
  }

  T right()
  {
    return x + width;
  }

  T top()
  {
    return y;
  }

  T bottom()
  {
    return y + height;
  }

  Vec2<T> top_left()
  {
    return{ top(), left() };
  }

  Vec2<T> bottom_right()
  {
    return{ bottom(), right() };
  }

  T x, y, width, height;
};

class Rectf : public Rect<float>
{
  Rectf()
    :
    Rect()
  {}

  Rectf(float x_, float y_, float width_, float height_)
    :
    Rect(x_, y_, width_, height_)
  {}


  virtual ~Rectf() = default;
  Rectf(Rectf const& other) = default;
  Rectf(Rectf&& other) = default;
  Rectf& operator=(Rectf const& other) = default;
  Rectf& operator=(Rectf&& other) = default;

  operator sf::FloatRect() const
  {
    return{ x, y, width, height };
  }

};

class Recti : public Rect<int>
{
  Recti()
    :
    Rect()
  {}

  Recti(int x_, int y_, int width_, int height_)
    :
    Rect(x_, y_, width_, height_)
  {}


  virtual ~Recti() = default;
  Recti(Recti const& other) = default;
  Recti(Recti&& other) = default;
  Recti& operator=(Recti const& other) = default;
  Recti& operator=(Recti&& other) = default;

  operator sf::IntRect() const
  {
    return{ x, y, width, height };
  }
};