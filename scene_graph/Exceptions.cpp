#include "stdafx.h"

#include "Exceptions.h"

#include "SceneNode.h"

NoSuchChildException::NoSuchChildException(SceneNode& node) :
  m_node{&node}
{}

char const* NoSuchChildException::what() const throw()
{
  std::stringstream ss;
  ss << "No such child: 0x" << std::setw(8) << std::setfill('0') << std::hex << m_node;
}

OrphanException::OrphanException(SceneNode* node) :
  m_node{node}
{}

char const* OrphanException::what() const throw()
{
  std::stringstream ss;
  ss << "Node has no parent: 0x" << std::setw(8) << std::setfill('0') << std::hex << m_node;
}
