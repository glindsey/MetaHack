#include "AttributeSet.h"
#include "ErrorHandler.h"

// I have been TRYING to get a std::map to work here, but the STL seems to
// hate me, so I'm going to cheat and use a const array instead.

AttributeData const AttributeSet::constants[] =
{
//  def   == max ==
  {   0, 2147483647 }, // XP
  {  10,      65535 }, // HP
  {  10,      65535 }, // MaxHP
  { 512,      65535 }, // FoodTimeout
  { 512,      65535 }, // DrinkTimeout
  {  15,         31 }, // Strength
  {  15,         31 }, // Endurance
  {  15,         31 }, // Vitality
  {  15,         31 }, // Agility
  {  15,         31 }, // Intelligence
  {  15,         31 }, // MagicDefense
  {  15,         31 }, // Charisma
  {  15,         31 }, // Allure
  {  15,         31 }, // Attentiveness
  {  15,         31 }, // Luck
};

AttributeSet::AttributeSet()
{
  for (unsigned int index = 0;
       index < uint_cast(Attribute::Count);
       ++index)
  {
    this->value[index] = constants[index].default_value;
  }

  this->negative_okay = false;
}

AttributeSet::~AttributeSet()
{
  //dtor
}

void AttributeSet::set_negative_okay(bool okay)
{
  this->negative_okay = okay;
}

bool AttributeSet::get_negative_okay()
{
  return this->negative_okay;
}

void AttributeSet::add(Attribute attrib, int new_value)
{
  this->set_to(attrib, new_value + value[uint_cast(attrib)]);
}

void AttributeSet::set_to(Attribute attrib, int new_value)
{
  unsigned int index = uint_cast(attrib);

  if ((!negative_okay) && (new_value < 0))
  {
    new_value = 0;
  }
  if (attrib == Attribute::HP)
  {
    if (new_value > value[uint_cast(Attribute::MaxHP)])
    {
      new_value = value[uint_cast(Attribute::MaxHP)];
    }
  }
  else
  {
    if (new_value > constants[index].maximum_value)
    {
      new_value = constants[index].maximum_value;
    }
  }

  value[index] = new_value;
}

unsigned int AttributeSet::get(Attribute attrib) const
{
  return value[uint_cast(attrib)];
}
