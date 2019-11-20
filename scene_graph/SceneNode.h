#pragma once

#include "stdafx.h"

#include "Object.h"

// Forward declarations
class Artist;
class Canvas;
class DataTie;
class EventProcessor;
class NodeOrganizer;
class SceneNode;

// Using declarations
using ArtistPtr = std::unique_ptr<Artist>;
using ArtistVector = std::vector<ArtistPtr>;
using CanvasPtr = std::unique_ptr<Canvas>;
using DataTiePtr = std::unique_ptr<DataTie>;
using DataTieMap = std::map<std::string, DataTiePtr>;
using EventProcessorPtr = std::unique_ptr<EventProcessor>;
using EventProcessorVector = std::vector<EventProcessorPtr>;
using NodeOrganizerPtr = std::unique_ptr<NodeOrganizer>;
using SceneNodePtr = std::unique_ptr<SceneNode>;
using NodeVector = std::vector<SceneNodePtr>;

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
  ///
  /// If the added node already has a parent, attempts to remove the node
  /// from that parent before continuing; if it can't, throws
  /// SceneNode::CantRemoveChildException.
  ///
  /// If this node already owns the child node, simply returns.
  /// Returns a reference to the child node, whether it was moved or not.
  ///
  /// \warning This node _takes ownership_ of the child node.
  SceneNode& addChild(SceneNodePtr newChild);

  /// Returns a reference to a child.
  /// If the child does not exist, throws SceneNode::NoSuchChildException.
  SceneNode& getChild(SceneNode& childToFind);

  /// Gets a reference to a specific child specified by index.
  SceneNode& getChild(unsigned int idx);

  /// Returns whether an object owns the specified child.
  bool hasChild(SceneNode& child);

  /// Gets the total number of children this node has.
  unsigned int getChildCount();

  /// Deletes a node from this node's children.
  /// If the requested node does not exist, the method does nothing and
  /// returns false. Otherwise, it deletes the target node and returns true.
  bool killChild(SceneNode& childToKill);

  /// Deletes all children from this node.
  void killAllChildren();

  /// Moves this node to a different parent.
  /// `x.moveTo(y)` is functionally equivalent to y.addChild(x).
  void moveTo(SceneNode& newParent);

  /// Returns a reference to this node's parent.
  /// If this node does not have a parent, throws SceneNode::OrphanException.
  SceneNode& parent();

  /// Removes a child from this parent and returns it.
  /// If the child does not exist, throws SceneNode::NoSuchChildException.
  /// \warning The caller assumes ownership of the child object.
  SceneNodePtr removeChild(SceneNode& childToRemove);

  /// Returns a reference to this node's canvas.
  Canvas& canvas();

  /// Returns a reference to the static root node of the graph.
  static SceneNode& root();

protected:
  /// Handle incoming events by passing them through processors.
  virtual bool onEvent(Event const& event);

  NodeVector::iterator findChild(SceneNode& childToFind);

  /// Set this node's parent.
  /// Does _not_ update any previous or new parent's `m_children` member!
  /// That is handled by the public methods that call this one.
  void setParent(SceneNode* parent);

  /// Returns a const reference to this node's list of children.
  NodeVector const& children();

  /// Returns a reference to this node's organizer.
  NodeOrganizer& organizer();

  /// Returns a const reference to this node's list of artists.
  ArtistVector const& artists();

  /// Returns a const reference to this node's map of dataties.
  DataTieMap const& dataties();

  /// Returns a const reference to this node's list of event processors.
  EventProcessorVector const& processors();

private:
  /// Size of this node, in pixels.
  IntVec2 m_size;

  /// Position of this node, in pixels, relative to its parent.
  IntVec2 m_position;

  /// Raw pointer to this node's parent (if any).
  SceneNode* m_parent;

  /// Vector of pointers to scene nodes owned by this one.
  NodeVector m_children;

  /// Unique pointer to the node organizer owned by this node.
  NodeOrganizerPtr m_organizer;

  /// Vector of pointers to artist instances owned by this node.
  ArtistVector m_artists;

  /// Map of pointers to dataties owned by this node.
  DataTieMap m_dataties;

  /// Vector of pointers to event processors owned by this node.
  EventProcessorVector m_processors;

  /// Unique pointer to the canvas owned by this node.
  CanvasPtr m_canvas;

  // Mutex for this node.
  // \todo MAYBE. I'm unsure if adding a mutex for thread safety is needed here.
  //boost::shared_mutex m_mutex;
};

#define SCENEGRAPH  SceneNode::root()
