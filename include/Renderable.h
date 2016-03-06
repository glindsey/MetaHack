#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "stdafx.h"

/**
 * @brief      Interface for a class that can be rendered to a texture.
 */
class RenderableToTexture
{
public:
  virtual ~RenderableToTexture() {}
  virtual bool render(sf::RenderTexture& texture, int frame) = 0;
protected:
private:
};

#endif // RENDERABLE_H
