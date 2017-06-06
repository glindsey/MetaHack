#include "systems/SystemJanitor.h"

SystemJanitor::SystemJanitor(ComponentManager & components) :
  SystemCRTP<SystemJanitor>({ EventEntityDestroyed::id,
                              EventEntityMarkedForDeletion::id }),
  m_components{ components }  
{}

SystemJanitor::~SystemJanitor()
{}

void SystemJanitor::doCycleUpdate()
{
  while (!m_entitiesPendingDeletion.empty())
  {
    auto entityToDelete = m_entitiesPendingDeletion.front();
    m_components.erase(entityToDelete);
    broadcast(EventEntityDestroyed(entityToDelete));
    m_entitiesPendingDeletion.pop_front();
  }
}

void SystemJanitor::markForDeletion(EntityId entity)
{
  if (std::find(std::cbegin(m_entitiesPendingDeletion), 
                std::cend(m_entitiesPendingDeletion), 
                entity) != std::cend(m_entitiesPendingDeletion))
  {
    m_entitiesPendingDeletion.push_back(entity);
    /// @todo Other systems should see the EventMarkedForDeletion event and
    ///       do whatever is necessary for that system to finalize the deletion.
    broadcast(EventEntityMarkedForDeletion(entity));
  }
}

void SystemJanitor::setMap_V(MapID newMap)
{}

bool SystemJanitor::onEvent(Event const & event)
{
  return false;
}

