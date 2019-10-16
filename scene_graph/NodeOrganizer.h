#pragma once

#include "stdafx.h"

#include "OwnedByNode.h"

/// Class responsible for organizing child nodes into a particular pattern.
class NodeOrganizer : public OwnedByNode
{
public:
  NodeOrganizer(SceneNode& node);
  virtual ~NodeOrganizer();

protected:

private:
};
