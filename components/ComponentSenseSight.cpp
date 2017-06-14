#include "components/ComponentSenseSight.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

namespace Components
{
  void from_json(json const& j, ComponentSenseSight& obj)
  {
    obj = ComponentSenseSight();

    // *** add Component-specific assignments here ***
    //JSONUtils::doIfPresent(j, "member", [&](auto& value) { obj.m_member = value; });
  }

  void to_json(json& j, ComponentSenseSight const& obj)
  {
    j = json::object();
    // *** add Component-specific assignments here ***
    //j["member"] = obj.m_member;
  }


  ComponentSenseSight::ComponentSenseSight()
    :
    m_transientTilesSeen{},
    m_transientTilesSeenSize{}
  {}

  ComponentSenseSight::~ComponentSenseSight()
  {}

  ComponentSenseSight::ComponentSenseSight(ComponentSenseSight const& other)
    :
    m_transientTilesSeen{},       // do NOT copy!
    m_transientTilesSeenSize{}    // do NOT copy!
  {}

  ComponentSenseSight& ComponentSenseSight::operator=(ComponentSenseSight const& other)
  {
    if (this != &other)
    {
      /// @todo Any non-transient data we want to copy goes here
    }
    return *this;
  }

  void ComponentSenseSight::resetSeen()
  {
    m_transientTilesSeen.reset();
  }

  void ComponentSenseSight::clearSeen()
  {
    m_transientTilesSeen.clear();
    m_transientTilesSeenSize = IntVec2(0, 0);
  }

  void ComponentSenseSight::resizeSeen(IntVec2 size)
  {
    m_transientTilesSeen.clear();
    m_transientTilesSeen.resize(size.x * size.y);
    m_transientTilesSeenSize = size;
  }

  bool ComponentSenseSight::canSee(IntVec2 coords) const
  {
    if (index(coords) < m_transientTilesSeen.size())
    {
      return m_transientTilesSeen[index(coords)];
    }
    else
    {
      CLOG(WARNING, "SenseSight") << "Requested coordinates were out-of-bounded";
      return false;
    }
  }

  void ComponentSenseSight::setSeen(IntVec2 coords)
  {
    m_transientTilesSeen[index(coords)] = true;
  }

  void ComponentSenseSight::unsetSeen(IntVec2 coords)
  {
    m_transientTilesSeen[index(coords)] = false;
  }

  boost::dynamic_bitset<size_t>::reference ComponentSenseSight::operator[](IntVec2 coords)
  {
    return m_transientTilesSeen[index(coords)];
  }

  size_t ComponentSenseSight::index(IntVec2 coords) const
  {
    return (m_transientTilesSeenSize.x * coords.y) + coords.x;
  }

} // end namespace
