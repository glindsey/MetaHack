#include "stdafx.h"
#include "Property.h"

bool operator==(Property const & lhs, Property const & rhs)
{
  return (lhs.m_type == rhs.m_type) && (lhs.m_data == rhs.m_data);
}
