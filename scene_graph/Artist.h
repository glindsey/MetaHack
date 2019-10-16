#pragma once

#include "stdafx.h"

#include "OwnedByNode.h"

/// Forward declarations
class SceneNode;

/// An object that draws a part of a scene node.
class Artist : public OwnedByNode
{
public:
  Artist(SceneNode& node);
  virtual ~Artist();

protected:

private:
};
