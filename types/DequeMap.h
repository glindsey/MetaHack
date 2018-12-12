#pragma once

#include <deque>
#include <unordered_map>
#include <utility>

#include "json.hpp"
using json = ::nlohmann::json;

/// Template class representing a map of type K to a deque of type V.
/// And serializable to JSON, too! It's a Christmas miracle!

template<typename K, typename V>
class DequeMap
{
public:
  DequeMap()
  {}

  virtual ~DequeMap() = default;
  DequeMap(DequeMap const& other) = default;
  DequeMap(DequeMap&& other) = default;
  DequeMap& operator=(DequeMap const& other) = default;
  DequeMap& operator=(DequeMap&& other) = default;

  // DequeMap& operator+=(DequeMap const& rhs)
  // {
  //   x += rhs.x;
  //   y += rhs.y;
  //   return *this;
  // }

  // DequeMap& operator-=(DequeMap const& rhs)
  // {
  //   x -= rhs.x;
  //   y -= rhs.y;
  //   return *this;
  // }

  // friend DequeMap operator+(DequeMap lhs, DequeMap const& rhs)
  // {
  //   lhs += rhs;
  //   return lhs;
  // }

  // friend DequeMap operator-(DequeMap lhs, DequeMap const& rhs)
  // {
  //   lhs -= rhs;
  //   return lhs;
  // }

  // friend bool operator==(DequeMap const& lhs, DequeMap const& rhs)
  // {
  //   return ((lhs.x == rhs.x) && (lhs.y == rhs.y));
  // }

  // friend bool operator!=(DequeMap const& lhs, DequeMap const& rhs)
  // {
  //   return !(lhs == rhs);
  // }

  // friend std::ostream& operator<<(std::ostream& os, DequeMap const& obj)
  // {
  //   os << "(" << obj.x << ", " << obj.y << ")";
  //   return os;
  // }

  std::deque<V>& operator[](K key)
  {
    if (m_dequeMap.count(key) == 0UL)
    {
      m_dequeMap.emplace(std::pair< K, std::deque<V> >());
    }
    return m_dequeMap[key];
  }

  std::deque<V>& at(K key)
  {
    return m_dequeMap[key];
  }

  std::deque<V> const& at(K key) const
  {
    return m_dequeMap[key];
  }

  V const& back(K key) const
  {
    return m_dequeMap[key].back();
  }

  size_t count(K key) const
  {
    return m_dequeMap.count(key);
  }

  void eraseAll(K key)
  {
    m_dequeMap.erase(key);
  }

  V const& front(K key) const
  {
    return m_dequeMap[key].front();
  }

  V const& popBack(K key)
  {
    V value = m_dequeMap[key].back();
    m_dequeMap[key].pop_back();
    return value;
  }

  V const& popFront(K key)
  {
    V value = m_dequeMap[key].front();
    m_dequeMap[key].pop_front();
    return value;
  }

  void pushBack(K key, V value)
  {
    m_dequeMap[key].push_back(value);
  }

  void pushFront(K key, V value)
  {
    m_dequeMap[key].push_front(value);
  }

  friend void to_json(json& j, DequeMap<K, V> const& obj)
  {
    LOG(WARNING) << "to_json(json&, DequeMap const&) is not yet implemented";
    /// @todo WRITE ME
  }

  friend void from_json(json const& j, DequeMap& obj)
  {
    LOG(WARNING) << "from_json(json const&, DequeMap&) is not yet implemented";
    /// @todo WRITE ME
    // if (j.is_number())
    // {
    //   T number = j;
    //   LOG(WARNING) << "Converting number " << number <<
    //     " into a Vec2<>(" << number << ", " << number <<
    //     "); this will work but may not be what you were looking for!";
    //   obj.x = number;
    //   obj.y = number;
    // }
    // else if (j.is_array() && (j[0] == "realvec2" || j[0] == "uintvec2" || j[0] == "intvec2"))
    // {
    //   obj.x = j[1];
    //   obj.y = j[2];
    // }
    // else
    // {
    //   LOG(FATAL) << "Attempted to create a Vec2<> out of an invalid JSON object: " << j.dump();
    // }
  }

protected:

private:
  /// Actual data.
  std::unordered_map< K, std::deque<V> > m_dequeMap;
};
