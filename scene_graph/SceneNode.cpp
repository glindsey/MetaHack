#include "stdafx.h"

#include "SceneNode.h"

#include "utilities/New.h"

#include "Artist.h"
#include "Canvas.h"
#include "EventProcessor.h"
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
  for (auto& processor : m_processors)
  {
    delete processor;
  }
  m_processors.clear();

  // Delete all artist instances.
  for (auto& artist : m_artists)
  {
    delete artist;
  }
  m_artists.clear();

  // Delete all children.
  for (auto& child : m_children)
  {
    delete child;
  }
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

bool SceneNode::addChild(SceneNode* newChild)
{
  if ((newChild == nullptr) || hasChild(newChild))
  {
    return false;
  }

  // Remove from old parent, if any.
  auto oldParent = newChild->parent();
  if (oldParent != nullptr)
  {
    Assert("SceneNode", oldParent->hasChild(newChild), "SceneNode " << this << " disowned by former parent!?");
    auto child = oldParent->removeChild(newChild);
  }

  m_children.push_back(newChild);
  newChild->setParent(this);
  return true;
}

SceneNode* SceneNode::getChild(SceneNode* childToFind)
{
  auto iter = std::find(std::cbegin(m_children), std::cend(m_children), childToFind);
  return (iter == std::cend(m_children) ? nullptr : *iter);
}

SceneNode* SceneNode::getChild(unsigned int idx)
{
  return (idx >= m_children.size()) ? nullptr : m_children[idx];
}

bool SceneNode::hasChild(SceneNode* child)
{
  return getChild(child) != nullptr;
}

unsigned int SceneNode::getChildCount()
{
  return m_children.size();
}

bool SceneNode::killChild(SceneNode* childToKill)
{
  auto child = removeChild(childToKill);

  if (child == nullptr)
  {
    return false;
  }

  delete child;
  return true;
}

void SceneNode::killAllChildren()
{
  for (auto& child : m_children)
  {
    delete child;
  }
  m_children.clear();
}

bool SceneNode::moveTo(SceneNode* newParent)
{
  if (newParent == nullptr)
  {
    return false;
  }

  newParent->addChild(this);
}

SceneNode* SceneNode::parent()
{
  return m_parent;
}

Canvas& SceneNode::canvas()
{
  return *(m_canvas.get());
}

void SceneNode::setParent(SceneNode* parent)
{
  m_parent = parent;
}

SceneNode* SceneNode::removeChild(SceneNode* childToRemove)
{
  auto iter = std::find(std::cbegin(m_children), std::cend(m_children), childToRemove);
  if (iter == std::end(m_children))
  {
    return nullptr;
  }

  m_children.erase(iter);
  return *iter;
}

std::vector<SceneNode*> const& SceneNode::children()
{
  return m_children;
}

NodeOrganizer& SceneNode::organizer()
{
  return *(m_organizer.get());
}

std::vector<Artist*> const& SceneNode::artists()
{
  return m_artists;
}

std::map<std::string, DataTie*> const& SceneNode::dataties()
{
  return m_dataties;
}

std::vector<EventProcessor*> const& SceneNode::processors()
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
  for (auto processor : m_processors)
  {
    if (processor->handle(event))
    {
      // Processor consumed the event, so return.
      return true;
    }
  }
  return false;
}
