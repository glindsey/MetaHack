#pragma once

#include "types/common.h"

class InventorySlot
{
public:
  InventorySlot()
    :
    m_slot{ 0 }
  {}

  InventorySlot(unsigned int slot)
    :
    m_slot{ slot }
  {}

  InventorySlot(std::string str)
    :
    m_slot{ stringToSlot(str) }
  {}

  operator std::string() const
  {
    return slotToString(m_slot);
  }

  explicit operator unsigned int() const
  {
    return m_slot;
  }

  InventorySlot& operator++()
  {
    ++m_slot;
    return *this;
  }

  InventorySlot operator++(int)
  {
    InventorySlot tmp(*this);
    operator++();
    return tmp;
  }

  InventorySlot& operator--()
  {
    --m_slot;
    return *this;
  }

  InventorySlot operator--(int)
  {
    InventorySlot tmp(*this);
    operator--();
    return tmp;
  }

  friend std::ostream& operator<<(std::ostream& os, InventorySlot const& obj)
  {
    os << static_cast<std::string>(obj);
    return os;
  }

  friend bool operator==(InventorySlot const& first, InventorySlot const& second)
  {
    return first.m_slot == second.m_slot;
  }

  friend bool operator!=(InventorySlot const& first, InventorySlot const& second)
  {
    return !operator==(first, second);
  }

  friend bool operator<(InventorySlot const& first, InventorySlot const& second)
  {
    return first.m_slot < second.m_slot;
  }

  friend bool operator>=(InventorySlot const& first, InventorySlot const& second)
  {
    return !operator<(first, second);
  }

  friend bool operator>(InventorySlot const& first, InventorySlot const& second)
  {
    return operator<(second, first);
  }

  friend bool operator<=(InventorySlot const& first, InventorySlot const& second)
  {
    return !operator>(first, second);
  }

  friend void to_json(json& j, InventorySlot const& obj);
  friend void from_json(json const& j, InventorySlot& obj);

  // Static instances
  static InventorySlot Zero;
  static InventorySlot Min;
  static InventorySlot Max;
  static InventorySlot Invalid;

protected:
  static unsigned int stringToSlot(std::string str);
  static std::string slotToString(unsigned int slot);

private:
  /// Slot, as an unsigned integer.
  unsigned int m_slot;

};
