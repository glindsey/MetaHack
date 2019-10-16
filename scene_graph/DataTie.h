#pragma once

#include "stdafx.h"

#include "OwnedByNode.h"

/// Forward declarations
class SceneNode;

/// An object that represents a tie to a piece of model data.
class DataTie : public OwnedByNode
{
public:
  DataTie(SceneNode& node);
  virtual ~DataTie();

protected:

private:
};
