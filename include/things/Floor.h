#ifndef FLOOR_H
#define FLOOR_H

#include "MapId.h"
#include "Thing.h"

/// A non-movable Thing that represents a Maptile's Floor.
class Floor :
  public Thing
{
  public:
    Floor(MapTile* map_tile);
    virtual ~Floor();

    virtual bool is_movable() const override;
    virtual bool is_opaque() const override;
    virtual bool readable_by(Entity const& entity) const override;

    /// Receive light from the specified LightSource.
    /// Gets the Map this tile belongs to and does a recursive
    /// raycasting algorithm on it.
    virtual void be_lit_by(LightSource& light) override;

  protected:

  private:
    virtual MapTile* _get_maptile() const override;
    virtual ActionResult _perform_action_read_by(Entity& entity) override;
    virtual std::string _get_description() const override;

    virtual ActionResult _can_contain(Thing& thing) const override;

    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // FLOOR_H
