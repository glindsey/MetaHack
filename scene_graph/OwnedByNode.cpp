#include "stdafx.h"

#include "OwnedByNode.h"

#include "utilities/New.h"

OwnedByNode::OwnedByNode(SceneNode& node) :
  m_node{ node }
{

}

SceneNode& OwnedByNode::node()
{
  return m_node;
}

OwnedByNode::~OwnedByNode()
{

}
