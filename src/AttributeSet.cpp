#include "AttributeSet.h"

std::map<Attribute, AttributeData const> AttributeSet::constants =
{
  { Attribute::XP,                {   0, 2147483647 } },
  { Attribute::HP,                {  10,      65535 } },
  { Attribute::MaxHP,             {  10,      65535 } },
  { Attribute::FoodTimeout,       { 512,      65535 } },
  { Attribute::DrinkTimeout,      { 512,      65535 } },
  { Attribute::Strength,          {  15,         31 } },
  { Attribute::Endurance,         {  15,         31 } },
  { Attribute::Vitality,          {  15,         31 } },
  { Attribute::Agility,           {  15,         31 } },
  { Attribute::Intelligence,      {  15,         31 } },
  { Attribute::MagicDefense,      {  15,         31 } },
  { Attribute::Charisma,          {  15,         31 } },
  { Attribute::Allure,            {  15,         31 } },
  { Attribute::Attentiveness,     {  15,         31 } },
  { Attribute::Luck,              {  15,         31 } }
};

AttributeSet::AttributeSet()
{
  for (unsigned int counter = 0;
       counter < static_cast<unsigned int>(Attribute::Count);
       ++counter)
  {
    Attribute attrib = static_cast<Attribute>(counter);
    this->value[attrib] = AttributeSet::constants[attrib].default_value;
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
  this->set_to(attrib, new_value + value[attrib]);
}

void AttributeSet::set_to(Attribute attrib, int new_value)
{
  if ((!negative_okay) && (new_value < 0))
  {
    new_value = 0;
  }
  if (attrib == Attribute::HP)
  {
    if (new_value > value[Attribute::MaxHP])
    {
      new_value = value[Attribute::MaxHP];
    }
  }
  else
  {
    if (new_value > constants[attrib].maximum_value)
    {
      new_value = constants[attrib].maximum_value;
    }
  }
}

unsigned int AttributeSet::get(Attribute attrib)
{
  return value[attrib];
}
