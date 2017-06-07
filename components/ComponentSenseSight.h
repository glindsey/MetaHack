#pragma once

#include "boost/dynamic_bitset.hpp"

#include "json.hpp"
using json = ::nlohmann::json;

#include "types/Vec2.h"

using TilesSeen = boost::dynamic_bitset<size_t>; // size_t gets rid of 64-bit compile warning

namespace Components
{

  /// Component that allows the capability of sight.
  class ComponentSenseSight final
  {
  public:
    ComponentSenseSight();
    ~ComponentSenseSight();

    ComponentSenseSight(ComponentSenseSight const& other);
    ComponentSenseSight& operator=(ComponentSenseSight const& other);

    friend void from_json(json const& j, ComponentSenseSight& obj);
    friend void to_json(json& j, ComponentSenseSight const& obj);

    void resetSeen();
    void clearSeen();
    void resizeSeen(IntVec2 size);
    bool canSee(IntVec2 coords) const;
    void setSeen(IntVec2 coords);
    void unsetSeen(IntVec2 coords);

    boost::dynamic_bitset<size_t>::reference operator[](IntVec2 coords);

  protected:
    size_t index(IntVec2 coords) const;

  private:
    /// Tiles currently seen. Transient data, NOT saved to JSON.
    TilesSeen m_transientTilesSeen;

    /// Size of tiles seen map.
    IntVec2 m_transientTilesSeenSize;

  };

} // end namespace Components
