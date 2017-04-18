#include "stdafx.h"

#include "AssertHelper.h"
#include "inventory/InventorySlot.h"

/// Static instances
InventorySlot InventorySlot::Zero = InventorySlot(0);
InventorySlot InventorySlot::Min = InventorySlot(1);
InventorySlot InventorySlot::Max = InventorySlot(UINT_MAX - 1);
InventorySlot InventorySlot::Invalid = InventorySlot(UINT_MAX);

void to_json(json& j, InventorySlot const& obj)
{
  j = static_cast<std::string>(obj);
}

void from_json(json const& j, InventorySlot& obj)
{
  obj = InventorySlot(j.get<std::string>());
}

unsigned int InventorySlot::stringToSlot(std::string str)
{
  unsigned int slot;

  Assert("Inventory", str.size() >= 1, "String \"" << str << "\" does not correspond to a slot number");

  /// Fancy processing...
  ///     0 <--> @
  ///  1-26 <--> a-z
  /// 27-52 <--> A-Z
  /// 53+   <--> 52+
  char const& firstCharacter = str.at(0);
  if (firstCharacter == '@')
  {
    slot = 0;
  }
  if (firstCharacter >= 'a' && firstCharacter <= 'z')
  {
    slot = (firstCharacter - static_cast<unsigned int>('a')) + 1;
  }
  else if (firstCharacter >= 'A' && firstCharacter <= 'Z')
  {
    slot = (firstCharacter - static_cast<unsigned int>('A')) + 27;
  }
  else
  {
    slot = std::stoul(str);
  }

  return slot;
}

std::string InventorySlot::slotToString(unsigned int slot)
{
  std::string str;

  char slotCharacter;
  if (slot == 0)
  {
    str = "@";
  }
  if (slot <= 26)
  {
    slotCharacter = static_cast<char>(slot - 1) + 'a';
    str += slotCharacter;
  }
  else if (slot <= 52)
  {
    slotCharacter = static_cast<char>(slot - 27) + 'a';
    str += slotCharacter;
  }
  else
  {
    str = std::to_string(slot);
  }

  return str;
}
