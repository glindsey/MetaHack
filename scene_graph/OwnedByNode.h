#pragma once

#include "stdafx.h"

/// Forward declarations
class SceneNode;

/// Virtual superclass for objects owned by a particular SceneNode.
class OwnedByNode
{
public:
  OwnedByNode(SceneNode& node);
  virtual ~OwnedByNode();

protected:
  SceneNode& node();

private:
  SceneNode& m_node;
};
