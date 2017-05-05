#include "stdafx.h"

#include "SystemSpacialRelationships.h"

SystemSpacialRelationships::SystemSpacialRelationships(ComponentMap<ComponentInventory>& inventory, 
                                                       ComponentMap<ComponentPosition>& position) :
  SystemCRTP<SystemSpacialRelationships>(),
  m_inventory{ inventory },
  m_position{ position }
{}

SystemSpacialRelationships::~SystemSpacialRelationships()
{}

void SystemSpacialRelationships::recalculate()
{}

void SystemSpacialRelationships::setMapNVO(MapId newMap)
{}
