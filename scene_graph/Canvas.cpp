#include "stdafx.h"

#include "Canvas.h"

#include "utilities/New.h"

Canvas::Canvas(SceneNode& node) :
  OwnedByNode(node)
{

}

Canvas::~Canvas()
{

}

IntVec2 Canvas::size()
{
  return m_size;
}

void Canvas::setSize(IntVec2 size)
{
  m_size = size;
  recreateTexture();
}

bool Canvas::withTexture(std::function<bool (sf::RenderTexture&)> func)
{
  if (!m_texture || !func)
  {
    return false;
  }

  return func(*(m_texture.get()));
}

void Canvas::recreateTexture()
{
  if ((m_size.x > 0) && (m_size.y > 0))
  {
    m_texture.reset(NEW sf::RenderTexture());
    m_texture->create(m_size.x, m_size.y);
  }
  else
  {
    m_texture.reset();
  }
}
