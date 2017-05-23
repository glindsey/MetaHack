#pragma once

#include "systems/System.h"

/// CRTP class for all systems in the game.
template <class Subclass>
class SystemCRTP : public System
{
public:
  SystemCRTP(std::unordered_set<EventID> const events) :
    System(events) 
  {}

  virtual ~SystemCRTP() {}

  /// Get this system, cast to the appropriate subclass.
  Subclass* operator->() { return static_cast<Subclass*>(this); }

protected:

private:
};