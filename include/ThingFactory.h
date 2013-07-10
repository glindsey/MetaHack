#ifndef THINGFACTORY_H
#define THINGFACTORY_H

#include <memory>
#include <vector>

#include "MapId.h"
#include "Thing.h"
#include "ThingId.h"

// Forward declarations
class Entity;
class MapTile;

// ThingFactory is both a Factory for Things and a store containing all of
// the Things currently in use.

class ThingFactory
{
  public:
    virtual ~ThingFactory();

    /// Initialize the ThingFactory.  Populates it with Limbo.
    void initialize();

    static ThingFactory& instance();

    /// Register a subclass with the factory.
    template<class Subclass> void register_class()
    {
      this->register_by_typename(typeid(Subclass).name(), &(Subclass::create));
    }

    /// Gets a reference to a Thing by ID.
    Thing& get(ThingId thing_id);

    /// Checks if a particular ID refers to a MapTile.
    bool is_a_tile(ThingId thing_id);

    /// Gets a reference to a MapTile by ID.
    MapTile& get_tile(ThingId thing_id);

    template<class T> ThingId create()
    {
      return create(typeid(T).name());
    }

    ThingId create(char const* type);

    ThingId create_tile(MapId mapId, int x, int y);

    bool destroy(ThingId thing_id);

    bool set_player_id(ThingId thing_id);
    ThingId get_player_id();
    Entity& get_player();
    Thing& get_limbo();

    static const ThingId limbo_id;

  protected:
    /// Register a subclass by typename.
    void register_by_typename(char const* type, ThingCreator creator);

  private:
    ThingFactory();

    struct Impl;
    std::unique_ptr<Impl> impl;

    static std::unique_ptr<ThingFactory> instance_;
};

// The following is a shortcut for something that will be typed A LOT.
#define TF        ThingFactory::instance()

#endif // THINGFACTORY_H
