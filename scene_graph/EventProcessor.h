#pragma once

#include "stdafx.h"

#include "OwnedByNode.h"

/// Forward declarations
class Event;

/// An object that processes events for a scene node.
class EventProcessor : public OwnedByNode
{
public:
  EventProcessor(SceneNode& node);
  virtual ~EventProcessor();

  /// Handle an event.
  /// Returns true if the event was consumed, or false if processing of
  /// this event should continue by other processors.
  virtual bool handle(Event const& event);

protected:

private:
};
