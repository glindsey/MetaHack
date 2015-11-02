#ifndef THINGMANAGER_H
#define THINGMANAGER_H

#include <memory>
#include <string>
#include <unordered_map>

#include "ThingRef.h"

// Forward declarations
class MapTile;
class Thing;
class ThingMetadata;

/// ThingManager is a factory to create and manage all the Things in the game.
class ThingManager
{
	friend class ThingId;
	friend class ThingRef;

public:
	virtual ~ThingManager();

    /// Initialize the ThingManager.  Populates it with Mu.
    void initialize();

    /// Get the singleton instance of ThingManager.
  	static ThingManager& instance();

    /// Create a particular object given the type name.
    /// @param type The type name of the object to create.
    /// @return A ThingRef to the new object created.
    ThingRef create(std::string type);

    /// Create a floor object.
    /// @param map_tile Pointer to the map tile associated.
    /// @return A ThingRef to the new object created.
    ThingRef create_floor(MapTile* map_tile);

    /// Clone a particular object.
    /// @param original_ref Reference to the object to clone.
    /// @return A ThingRef to the new cloned object.
    ThingRef clone(ThingRef original_ref);

    /// Destroy an object given a ThingRef to the object.
    /// If the given ThingRef does not correspond to an object, does nothing.
    /// @param ref ThingRef of the object to destroy.
    void destroy(ThingRef ref);

    /// Returns whether a Thing with a particular ThingRef exists.
    bool exists(ThingRef ref);

    /// Set the game player.
    /// If the caller attempts to set a ThingRef of a Thing that does not exist,
    /// the program will abort with a FATAL_ERROR call.
    /// @note Changing the player ID has not been testing as of this writing,
    ///       and unpredictable results may occur!
    /// @param ref ThingRef of the Thing to set as the player.
    /// @return True if the set was successful, false otherwise.
    bool set_player(ThingRef ref);

    /// Get the ThingRef of the game player Thing.
    /// @return The player ThingRef.
    ThingRef get_player() const;

    /// Get the ThingRef of Mu (nothingness).
    /// @return The ThingRef of Mu.
    ThingRef get_mu() const;

protected:

  /// Get a pointer to the Thing associated with a particular ThingId.
	/// If the ID does not exist, returns Mu.
	Thing* get_ptr(ThingId data);

	/// Get a pointer to the Thing associated with a particular ThingId.
	/// If the ID does not exist, returns Mu.
	Thing const* get_ptr(ThingId data) const;

private:
  /// Constructor; private because ThingFactory is a singleton.
  ThingManager();

  struct Impl;
  /// Implementation pointer
  std::unique_ptr<Impl> pImpl;

  /// Unique pointer to singleton instance.
  static std::unique_ptr<ThingManager> instance_;
};

/// Shortcut to the singleton instance, to save on typing.
#define TM        ThingManager::instance()

#endif // THINGMANAGER_H
