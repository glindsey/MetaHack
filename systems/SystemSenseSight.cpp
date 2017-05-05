#include "stdafx.h"

#include "systems/SystemSenseSight.h"

#include "components/ComponentInventory.h"
#include "components/ComponentPosition.h"
#include "components/ComponentSenseSight.h"
#include "utilities/MathUtils.h"

SystemSenseSight::SystemSenseSight(ComponentMap<ComponentInventory>& inventory,
                                   ComponentMap<ComponentPosition>& position,
                                   ComponentMap<ComponentSenseSight>& senseSight) :
  SystemCRTP<SystemSenseSight>(),
  m_inventory{ inventory },
  m_position{ position },
  m_senseSight{ senseSight }
{}

SystemSenseSight::~SystemSenseSight()
{}

void SystemSenseSight::recalculate()
{}

void SystemSenseSight::setMapNVO(MapId newMap)
{}
