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

/// ThingFactory is both a Factory for Things and a store containing all of
/// the Things currently in use.  All Things are created, referenced, and
/// destroyed via ThingId identifiers; this allows (or will allow) for easy
/// serialization during game save/load.
class ThingFactory
{
  public:
    virtual ~ThingFactory();

    /// Initialize the ThingFactory.  Populates it with Limbo.
    void initialize();

    /// Get the singleton instance of ThingFactory.
    static ThingFactory& instance();

    /// Register a subclass with the factory.
    /// This template function is called by the ThingRegistrator construtor,
    /// and stores an entry in an unordered_map which links the class typeid
    /// with its own create() function.  This is later used by
    /// ThingFactory::create to make new instances of a particular class.
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

    /// Create a particular class and return it as a ThingId.
    /// This template function gets the typeid of the class to be created,
    /// and passes it to the other create() function.
    /// @return The ID of the new Thing created.
    template<class T> ThingId create()
    {
      return create(typeid(T).name());
    }

    /// Create a particular class given the typeid.
    /// @param type The typeid of the class to create.  The typeid must have
    ///             been registered first with ThingFactory::register_class<>().
    /// @return The ID of the new Thing created, or limbo_id if the Thing
    ///         could not be created for any reason.
    ThingId create(char const* type);

    /// Specifically create a MapTile instance.
    /// Used when constructing Map objects, this generates a MapTile and
    /// returns its ThingId.
    /// @param mapId MapID of the Map this tile is a part of.
    /// @param x X coordinate of the tile on the Map.
    /// @param y Y coordinate of the tile on the Map.
    /// @return The ID of the new MapTile.
    ThingId create_tile(MapId mapId, int x, int y);

    /// Destroy a Thing, given its ID.
    /// This function removes the specified Thing from the ThingFactory's
    /// library, and calls the destructor on the associated Thing.  Any
    /// subsequent attempts to call ThingFactory::get() on that ID will return
    /// the Limbo object, until that ID is used again.
    /// @todo Currently an ID will never be reused, as the "next ID" variable
    ///       just continues to increment.  This is a problem as the library
    ///       (a simple std::vector) will continue to get larger and larger as
    ///       the game goes on.  However any solution will require that
    ///       ThingFactory::get() remains an O(1) (constant-time) function;
    ///       even an unordered map results in drastically slower run speeds
    ///       since every single MapTile is stored in the ThingFactory library!
    /// @param thing_id ThingID to destroy.
    /// @return True if destroy was successful; false if the Thing didn't exist.
    bool destroy(ThingId thing_id);

    /// Set the ID associated with the game player.
    /// If the caller attempts to set an ID of a Thing that isn't a subclass of
    /// Entity, or a Thing that does not exist (was never created or was
    /// previously destroyed), the program will abort with a FATAL_ERROR call.
    /// @note Changing the player ID has not been testing as of this writing,
    ///       and unpredictable results may occur!
    /// @param thing_id ThingID to set as the player.
    /// @return True if the set was successful, false otherwise.
    bool set_player_id(ThingId thing_id);

    /// Get the ID associated with the game player.
    /// @return The player ID.
    ThingId get_player_id();

    /// Get a reference to the player Entity.
    /// This function is faster than ThingFactory::get(get_player_id()) because
    /// a reference to the player Entity is cached.
    /// @return A reference to the player Entity.
    Entity& get_player();

    /// Get a reference to Limbo.
    /// This function is faster than ThingFactory::get(limbo_id) because
    /// a reference to Limbo is cached.
    /// @return A reference to Limbo.
    Thing& get_limbo();

    /// The Limbo ID.
    /// The implementation sets this to 0; however, using limbo_id is both
    /// easier and faster than writing static_cast<ThingId>(0) every time.
    static const ThingId limbo_id;

  protected:
    /// Register a subclass by typename.
    /// Used by the register_class() template function, this adds an entry
    /// to the unordered map associating subclass typeids with create() methods.
    /// @param type String corresponding to a subclass typeid.
    /// @param creator Creator function that returns a reference to a new,
    ///                dynamically-allocated Thing.
    void register_by_typename(char const* type, ThingCreator creator);

  private:
    /// Constructor; private because ThingFactory is a singleton.
    ThingFactory();

    struct Impl;
    /// Implementation pointer
    std::unique_ptr<Impl> impl;

    /// Unique pointer to singleton instance.
    static std::unique_ptr<ThingFactory> instance_;
};

/// Shortcut to the singleton instance, to save on typing.
#define TF        ThingFactory::instance()

#endif // THINGFACTORY_H
