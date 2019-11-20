#pragma once

#include "stdafx.h"

#include <iostream>
#include <exception>

// Forward Declarations
class SceneNode;

class NoSuchChildException: public std::exception
{
public:
  NoSuchChildException(SceneNode& node);

  virtual char const* what() const throw();

private:
  SceneNode* m_node;
};

class OrphanException: public std::exception
{
public:
  OrphanException(SceneNode* node);

  virtual char const* what() const throw();

private:
  SceneNode* m_node;
};
