#include "Floor.h"

#include <memory>

#include "Entity.h"
#include "MapTile.h"

struct Floor::Impl
{
  MapTile* map_tile;
};

Floor::Floor(MapTile* map_tile)
  : Thing(-1), impl(new Impl())
{
  impl->map_tile = map_tile;
}

Floor::~Floor()
{
  //dtor
}

bool Floor::is_movable() const
{
  return false;
}

bool Floor::is_opaque() const
{
  return false;
}

bool Floor::readable_by(Entity const& entity) const
{
  return true;
}

void Floor::be_lit_by(LightSource& light)
{
  _get_maptile()->be_lit_by(light);
}

MapTile* Floor::_get_maptile() const
{
  return impl->map_tile;
}

ActionResult Floor::_perform_action_read_by(Entity& entity)
{
  /// @todo Implement perform_action_read_by (e.g. writing/engraving on walls).
  return ActionResult::Failure;
}

std::string Floor::_get_description() const
{
  return "floor";
}

ActionResult Floor::_can_contain(Thing& thing) const
{
  /// @todo Implement can_contain.  Right now a Floor can contain any Thing.
  return ActionResult::Success;
}
