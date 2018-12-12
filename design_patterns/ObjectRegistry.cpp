#include "stdafx.h"

#include "ObjectRegistry.h"

#include "design_patterns/Object.h"
#include "utilities/New.h"

ObjectRegistry::ObjectRegistry()
{}

ObjectRegistry::~ObjectRegistry()
{}

bool ObjectRegistry::add(Object* obj)
{
  if (contains(obj)) return false;

  boost::upgrade_lock<boost::shared_mutex> lock(m_registryMutex);
  boost::upgrade_to_unique_lock<boost::shared_mutex> writeLock(lock);

  m_registry.insert(obj);
  return true;
}

bool ObjectRegistry::remove(Object* obj)
{
  if (!contains(obj)) return false;

  boost::upgrade_lock<boost::shared_mutex> lock(m_registryMutex);
  boost::upgrade_to_unique_lock<boost::shared_mutex> writeLock(lock);

  m_registry.erase(obj);
  return true;
}

bool ObjectRegistry::contains(Object* obj)
{
  boost::shared_lock<boost::shared_mutex> readLock(m_registryMutex);

  return (m_registry.count(obj) != 0);
}

ObjectRegistry& ObjectRegistry::instance()
{
  static std::unique_ptr<ObjectRegistry> s_instance{ NEW ObjectRegistry() };

  return *(s_instance.get());
}
