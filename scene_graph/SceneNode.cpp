#include "stdafx.h"

#include "SceneNode.h"

#include "utilities/New.h"

#include "Artist.h"
#include "EventProcessor.h"

SceneNode::SceneNode(EventSet const events, SceneNode* parent) :
  Object(events),
  m_parent{parent}
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

std::vector<Artist*> const& SceneNode::artists()
{
  return m_artists;
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
