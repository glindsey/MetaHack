#pragma once

#include "stdafx.h"

#include "Object.h"

// Forward declarations
class Artist;
class Canvas;
class DataTie;
class EventProcessor;
class NodeOrganizer;

/// A node contained within the scene graph.
class SceneNode : public Object
{
public:
  SceneNode(EventSet const events, SceneNode* parent);
  virtual ~SceneNode();

  /// \todo Probably need to implement copy ctors as well

  IntVec2 size();
  void setSize(IntVec2 size);

  IntVec2 position();
  void setPosition(IntVec2 position);

  /// Add a node to this node's children.
  /// If the added node already has a parent, attempts to remove the node
  /// from that parent before continuing; if it can't, the method does
  /// nothing and returns false.
  /// \warning This node _takes ownership_ of the child node.
  bool addChild(SceneNode* newChild);

  /// Checks whether this node owns a particular child.
  /// If it does, returns a pointer to that child. Otherwise, returns nullptr.
  SceneNode* getChild(SceneNode* childToFind);

  /// Gets a pointer to a specific child specified by index.
  /// If the index provided is out-of-bounds, returns nullptr.
  SceneNode* getChild(unsigned int idx);

  /// Syntactic sugar for `getChild(SceneNode* child) != nullptr`.
  bool hasChild(SceneNode* child);

  /// Gets the total number of children this node has.
  unsigned int getChildCount();

  /// Deletes a node from this node's children.
  /// If the requested node does not exist, the method does nothing and
  /// returns false. Otherwise, it deletes the target node and returns true.
  bool killChild(SceneNode* childToKill);

  /// Deletes all children from this node.
  void killAllChildren();

  /// Moves this node to a different parent.
  /// `x.moveTo(y)` is functionally equivalent to y.addChild(x).
  bool moveTo(SceneNode* newParent);

  /// Returns a pointer to this node's parent.
  SceneNode* parent();

  /// Returns a reference to this node's canvas.
  Canvas& canvas();

  /// Returns the static root node of the graph.
  static SceneNode& root();

protected:
  /// Handle incoming events by passing them through processors.
  virtual bool onEvent(Event const& event);

  /// Set this node's parent.
  /// Does _not_ update any previous or new parent's `m_children` member!
  /// That is handled by the public methods that call this one.
  void setParent(SceneNode* parent);

  /// Removes a child from this parent and returns it.
  /// If the child does not exist, returns nullptr.
  /// \warning Does _not_ delete the child! The caller assumes ownership of the child object.
  SceneNode* removeChild(SceneNode* childToRemove);

  /// Returns this node's list of children.
  std::vector<SceneNode*> const& children();

  /// Returns a reference to this node's organizer.
  NodeOrganizer& organizer();

  /// Returns this node's list of artists.
  std::vector<Artist*> const& artists();

  /// Returns this node's map of dataties.
  std::map<std::string, DataTie*> const& dataties();

  /// Returns this node's list of event processors.
  std::vector<EventProcessor*> const& processors();

private:
  /// Size of this node, in pixels.
  IntVec2 m_size;

  /// Position of this node, in pixels, relative to its parent.
  IntVec2 m_position;

  /// Raw pointer to this node's parent (if any).
  SceneNode* m_parent;

  /// Vector of pointers to scene nodes owned by this one.
  std::vector<SceneNode*> m_children;

  /// Unique pointer to the node organizer owned by this node.
  std::unique_ptr<NodeOrganizer> m_organizer;

  /// Vector of pointers to artist instances owned by this node.
  std::vector<Artist*> m_artists;

  /// Map of pointers to dataties owned by this node.
  std::map<std::string, DataTie*> m_dataties;

  /// Vector of pointers to event processors owned by this node.
  std::vector<EventProcessor*> m_processors;

  /// Unique pointer to the canvas owned by this node.
  std::unique_ptr<Canvas> m_canvas;

  // Mutex for this node.
  // \todo MAYBE. I'm unsure if adding a mutex for thread safety is needed here.
  //boost::shared_mutex m_mutex;
};

#define SCENEGRAPH  SceneNode::root()
