#pragma once

#include "SFML/Graphics.hpp"

#include "types/Vec2.h"

namespace MetaHack
{
	/// Definition of a rectangle.
	template <typename T>
	class Rectangle
	{
	public:
		Rectangle()
		{}

		Rectangle(T x_, T y_, T width_, T height_)
			:
			x{ x_ }, y{ y_ }, width{ width_ }, height{ height_ }
		{}

		Rectangle(sf::Rect<T> rect)
			:
			x{ rect.left }, y{ rect.top }, width{ rect.width }, height{ rect.height }
		{}

		virtual ~Rectangle() = default;
		Rectangle(Rectangle const& other) = default;
		Rectangle(Rectangle&& other) = default;
		Rectangle& operator=(Rectangle const& other) = default;
		Rectangle& operator=(Rectangle&& other) = default;

		friend bool operator==(Rectangle const& lhs, Rectangle const& rhs)
		{
			return ((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.width == rhs.width) && (lhs.height == rhs.height));
		}


		friend bool operator!=(Rectangle const& lhs, Rectangle const& rhs)
		{
			return !(lhs == rhs);
		}

		friend std::ostream& operator<<(std::ostream& os, Rectangle const& obj)
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
}

using Rectf = MetaHack::Rectangle<float>;
using Recti = MetaHack::Rectangle<int>;
