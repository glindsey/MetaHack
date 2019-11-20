#include "stdafx.h"

#include "SceneNode.h"

#include "utilities/New.h"

#include "Artist.h"
#include "Canvas.h"
#include "DataTie.h"
#include "EventProcessor.h"
#include "Exceptions.h"
#include "NodeOrganizer.h"

SceneNode::SceneNode(EventSet const events, SceneNode* parent) :
  Object(events),
  m_parent{ parent },
  m_canvas{ NEW Canvas(*this) }
{

}

SceneNode::~SceneNode()
{
  // Delete all event processors.
  m_processors.clear();

  // Delete all artist instances.
  m_artists.clear();

  // Delete all children.
  m_children.clear();
}

IntVec2 SceneNode::size()
{
  return m_size;
}

void SceneNode::setSize(IntVec2 size)
{
  m_size = size;

  m_canvas->setSize(size);

  /// \todo Tell all child nodes that we've been resized and/or re-organize the child nodes as needed.
}

IntVec2 SceneNode::position()
{
  return m_position;
}

void SceneNode::setPosition(IntVec2 position)
{
  m_position = position;

  /// \todo Tell all child nodes that our position has changed (?)
}

SceneNode& SceneNode::addChild(SceneNodePtr newChild)
{
  Assert("SceneNode", newChild, "Empty pointer passed into addChild");

  auto& childRef = *(newChild.get());

  if (hasChild(childRef))
  {
    return childRef;
  }

  // Remove from old parent, if any.
  auto& oldParent = newChild->parent();
  auto child = oldParent.removeChild(childRef);
  m_children.push_back(std::move(newChild));
  newChild->setParent(this);
  return childRef;
}

SceneNode& SceneNode::getChild(SceneNode& childToFind)
{
  auto const& iter = findChild(childToFind);

  if (iter == std::cend(m_children))
  {
    throw NoSuchChildException(childToFind);
  }

  return *((*iter).get());  // gosh I love C++ syntax
}

SceneNode& SceneNode::getChild(unsigned int idx)
{
  Assert("SceneNode", idx < m_children.size(), "Requested child index out of range");

  return *(m_children[idx].get());
}

bool SceneNode::hasChild(SceneNode& child)
{
  return findChild(child) != std::cend(m_children);
}

unsigned int SceneNode::getChildCount()
{
  return m_children.size();
}

bool SceneNode::killChild(SceneNode& childToKill)
{
  if(!hasChild(childToKill))
  {
    return false;
  }

  removeChild(childToKill);
  return true;
}

void SceneNode::killAllChildren()
{
  m_children.clear();
}

void SceneNode::moveTo(SceneNode& newParent)
{
  auto ptrToSelf = m_parent->removeChild(*this);

  newParent.addChild(std::move(ptrToSelf));
}

SceneNode& SceneNode::parent()
{
  if (m_parent == nullptr)
  {
    throw OrphanException(this);
  }

  return *m_parent;
}

Canvas& SceneNode::canvas()
{
  return *(m_canvas.get());
}

NodeVector::iterator SceneNode::findChild(SceneNode& childToFind)
{
  return std::find_if(
    std::begin(m_children),
    std::end(m_children), [&](std::unique_ptr<SceneNode>& p) {
      return p.get() == &childToFind;
    });
}

void SceneNode::setParent(SceneNode* parent)
{
  m_parent = parent;
}

SceneNodePtr SceneNode::removeChild(SceneNode& childToRemove)
{
  auto const& iter = findChild(childToRemove);

  if (iter == std::cend(m_children))
  {
    throw NoSuchChildException(childToRemove);
  }

  auto child = std::move(*iter);
  m_children.erase(iter);
  child->setParent(nullptr);

  return child;
}

NodeVector const& SceneNode::children()
{
  return m_children;
}

NodeOrganizer& SceneNode::organizer()
{
  return *(m_organizer.get());
}

ArtistVector const& SceneNode::artists()
{
  return m_artists;
}

DataTieMap const& SceneNode::dataties()
{
  return m_dataties;
}

EventProcessorVector const& SceneNode::processors()
{
  return m_processors;
}

SceneNode& SceneNode::root()
{
  EventSet events;

  static std::unique_ptr<SceneNode> s_root{ NEW SceneNode(events, nullptr) };

  return *(s_root.get());
}

bool SceneNode::onEvent(Event const& event)
{
  for (auto& processor : m_processors)
  {
    if (processor->handle(event))
    {
      // Processor consumed the event, so return.
      return true;
    }
  }
  return false;
}
