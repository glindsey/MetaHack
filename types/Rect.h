#pragma once

#include "SFML/Graphics.hpp"

#include "types/Vec2.h"

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

  Rect(sf::Rect<T> rect)
    :
    x{ rect.left }, y{ rect.top }, width{ rect.width }, height{ rect.height }
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
    return (x + width) - 1;
  }

  T top()
  {
    return y;
  }

  T bottom()
  {
    return (y + height) - 1;
  }

  Vec2<T> top_left()
  {
    return{ top(), left() };
  }

  Vec2<T> bottom_right()
  {
    return{ bottom(), right() };
  }

  operator sf::Rect<T>() const
  {
    return{ x, y, width, height };
  }

  T x, y, width, height;
};

using Rectf = Rect<float>;
using Recti = Rect<int>;