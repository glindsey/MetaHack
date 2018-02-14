#include "stdafx.h"

#include "systems/SystemJanitor.h"

namespace Systems
{

  Janitor::Janitor(Components::ComponentManager& components) :
    CRTP<Janitor>({ EventEntityDestroyed::id,
                                EventEntityMarkedForDeletion::id }),
    m_components{ components }
  {}

  Janitor::~Janitor()
  {}

  void Janitor::doCycleUpdate()
  {
    while (!m_entitiesPendingDeletion.empty())
    {
      auto entityToDelete = m_entitiesPendingDeletion.front();
      m_components.erase(entityToDelete);
      EventEntityDestroyed event(entityToDelete);
      broadcast(event);
      m_entitiesPendingDeletion.pop_front();
    }
  }

  void Janitor::markForDeletion(EntityId entity)
  {
    if (std::find(std::cbegin(m_entitiesPendingDeletion),
                  std::cend(m_entitiesPendingDeletion),
                  entity) != std::cend(m_entitiesPendingDeletion))
    {
      m_entitiesPendingDeletion.push_back(entity);
      /// @todo Other systems should see the EventMarkedForDeletion event and
      ///       do whatever is necessary for that system to finalize the deletion.
      EventEntityMarkedForDeletion event(entity);
      broadcast(event);
    }
  }

  void Janitor::setMap_V(MapID newMap)
  {}

  bool Janitor::onEvent(Event const & event)
  {
    return false;
  }

} // end namespace Systems
