#pragma once

#include "stdafx.h"

#include "OwnedByNode.h"

/// Forward declarations
class Canvas;
class SceneNode;

/// An object that draws a part of a scene node.
class Artist : public OwnedByNode
{
public:
  Artist(SceneNode& node);
  virtual ~Artist();

  /// Draw onto the specified canvas.
  /// Returns false if drawing should proceed, true if not.
  /// (Although this seems backward, it mirrors how event processors work.)
  virtual bool drawOn(Canvas& canvas);

protected:

private:
};
