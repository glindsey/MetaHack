#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <SFML/Graphics.hpp>

/**
 * @brief      Interface for a class that can be rendered on-screen.
 */
class Renderable
{
public:
  virtual ~Renderable() {}
  virtual bool render(sf::RenderTarget& target, int frame) = 0;
protected:
private:
};

#endif // RENDERABLE_H
