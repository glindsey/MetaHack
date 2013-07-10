#ifndef STATUSAREA_H
#define STATUSAREA_H

#include <memory>
#include <SFML/Graphics.hpp>

#include "EventHandler.h"
#include "Renderable.h"

class StatusArea :
  public EventHandler,
  public Renderable
{
  public:
    StatusArea(sf::IntRect dimensions);
    virtual ~StatusArea();

    void set_focus(bool focus);
    bool get_focus();
    sf::IntRect get_dimensions();
    void set_dimensions(sf::IntRect dimensions);

    virtual EventResult handle_event(sf::Event& event);

    virtual bool render(sf::RenderTarget& target, int frame);

  protected:
  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // STATUSAREA_H
