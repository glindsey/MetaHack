#include "AttributeSet.h"

std::map<unsigned int, AttributeData const> AttributeSet::constants =
{
  { static_cast<unsigned int>(Attribute::XP),             {   0, 2147483647 } },
  { static_cast<unsigned int>(Attribute::HP),             {  10,      65535 } },
  { static_cast<unsigned int>(Attribute::MaxHP),          {  10,      65535 } },
  { static_cast<unsigned int>(Attribute::FoodTimeout),    { 512,      65535 } },
  { static_cast<unsigned int>(Attribute::DrinkTimeout),   { 512,      65535 } },
  { static_cast<unsigned int>(Attribute::Strength),       {  15,         31 } },
  { static_cast<unsigned int>(Attribute::Endurance),      {  15,         31 } },
  { static_cast<unsigned int>(Attribute::Vitality),       {  15,         31 } },
  { static_cast<unsigned int>(Attribute::Agility),        {  15,         31 } },
  { static_cast<unsigned int>(Attribute::Intelligence),   {  15,         31 } },
  { static_cast<unsigned int>(Attribute::MagicDefense),   {  15,         31 } },
  { static_cast<unsigned int>(Attribute::Charisma),       {  15,         31 } },
  { static_cast<unsigned int>(Attribute::Allure),         {  15,         31 } },
  { static_cast<unsigned int>(Attribute::Attentiveness),  {  15,         31 } },
  { static_cast<unsigned int>(Attribute::Luck),           {  15,         31 } }
};

AttributeSet::AttributeSet()
{
  for (unsigned int index = 0;
       index < static_cast<unsigned int>(Attribute::Count);
       ++index)
  {
    this->value[index] = AttributeSet::constants.at(index).default_value;
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
  this->set_to(attrib, new_value + value[static_cast<unsigned int>(attrib)]);
}

void AttributeSet::set_to(Attribute attrib, int new_value)
{
  if ((!negative_okay) && (new_value < 0))
  {
    new_value = 0;
  }
  if (attrib == Attribute::HP)
  {
    if (new_value > value[static_cast<unsigned int>(Attribute::MaxHP)])
    {
      new_value = value[static_cast<unsigned int>(Attribute::MaxHP)];
    }
  }
  else
  {
    unsigned int index = static_cast<unsigned int>(attrib);
    if (new_value > constants.at(index).maximum_value)
    {
      new_value = constants.at(index).maximum_value;
    }
  }
}

unsigned int AttributeSet::get(Attribute attrib)
{
  return value[static_cast<unsigned int>(attrib)];
}
