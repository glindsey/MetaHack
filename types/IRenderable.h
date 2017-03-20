#pragma once

#include <SFML/Graphics.hpp>

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