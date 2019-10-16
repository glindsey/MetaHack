#pragma once

/// Forward declarations
class SceneNode;

/// An object that processes events for a scene node.
class EventProcessor
{
public:
  EventProcessor(SceneNode& node);
  virtual ~EventProcessor();

protected:

private:
  SceneNode& m_node;
};
