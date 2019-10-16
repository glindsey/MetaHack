#pragma once

#include "stdafx.h"

#include "OwnedByNode.h"

/// An object that processes events for a scene node.
class EventProcessor : public OwnedByNode
{
public:
  EventProcessor(SceneNode& node);
  virtual ~EventProcessor();

protected:

private:
};
