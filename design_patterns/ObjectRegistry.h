#pragma once

#include <boost/thread.hpp>
#include <memory>
#include <unordered_set>

// Forward declarations
class Object;

/// A static class that keeps track of all Objects that are created/deleted.
/// Since the registry stores Objects by pointer, it is designed to be
/// ephemeral, existing only for a single session, and *not* serialized
/// and/or saved.
class ObjectRegistry
{
public:
  ObjectRegistry();
  virtual ~ObjectRegistry();

  /// Adds an object to the registry.
  /// Returns true if the object was added, false if it already existed.
  bool add(Object* obj);

  /// Removes an object from the registry.
  /// Returns true if the object was removed, false if it didn't exist.
  bool remove(Object* obj);

  /// Returns true if the object exists in the registry, false otherwise.
  bool contains(Object* obj);

  /// Returns a static instance of the registry.
  static ObjectRegistry& instance();

private:
  // Unordered set of registered objects.
  std::unordered_set<Object*> m_registry;

  // Mutex for the registry.
  boost::shared_mutex m_registryMutex;
};

#define REGISTRY  ObjectRegistry::instance()
