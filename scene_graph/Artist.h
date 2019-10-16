#pragma once

/// Forward declarations
class SceneNode;

/// An object that draws a part of a scene node.
class Artist
{
public:
  Artist(SceneNode& node);
  virtual ~Artist();

protected:

private:
  SceneNode& m_node;
};
