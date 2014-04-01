#ifndef THINGFACTORY_H
#define THINGFACTORY_H

#include <memory>
#include <vector>

#include "MapId.h"
#include "Thing.h"

// Forward declarations
class Container;
class Entity;
class MapTile;

/// ThingFactory is a Factory to create Things and return shared_ptr instances
/// to them.
class ThingFactory
{
  public:
    virtual ~ThingFactory();

    /// Initialize the ThingFactory.  Populates it with Limbo.
    void initialize();

    /// Get the singleton instance of ThingFactory.
    static ThingFactory& instance();

    /// Register a subclass with the factory.
    /// This template function is called by the Registered constructor,
    /// and stores an entry in an unordered_map which links the class typeid
    /// with its own create() function.  This is later used by
    /// ThingFactory::create to make new instances of a particular class.
    template<class Subclass> void register_class()
    {
      this->register_by_typename(typeid(Subclass).name(),
                                 &(Subclass::create_thing));
    }

    /// Create a particular class given the typeid.
    /// @param type The typeid of the class to create.  The typeid must have
    ///             been registered first with ThingFactory::register_class<>().
    /// @return A shared_ptr<Thing> to the new Thing created, or an empty
    ///         shared_ptr if the Thing could not be created for any reason.
    std::shared_ptr<Thing> create(char const* type);

    /// Set the game player.
    /// If the caller attempts to set an ID of a Thing that does not exist,
    /// the program will abort with a FATAL_ERROR call.
    /// @note Changing the player pointer has not been testing as of this writing,
    ///       and unpredictable results may occur!
    /// @param entity Entity to set as the player.
    /// @return True if the set was successful, false otherwise.
    bool set_player(std::shared_ptr<Entity> entity);

    /// Get a pointer to the game player.
    /// @return The player ID.
    std::shared_ptr<Entity> get_player() const;

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
