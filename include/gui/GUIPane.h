#ifndef GUIPANE_H
#define GUIPANE_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "EventHandler.h"
#include "Renderable.h"

class GUIPane :
  public EventHandler,
  public Renderable
{
  public:
    GUIPane(sf::IntRect dimensions);
    virtual ~GUIPane();

    void set_focus(bool focus);
    bool get_focus();

    sf::IntRect get_dimensions();
    void set_dimensions(sf::IntRect dimensions);

    virtual EventResult handle_event(sf::Event& event);

    virtual bool render(sf::RenderTarget& target, int frame) final;

  protected:
    virtual std::string render_contents(int frame);

    sf::RenderTexture& get_bg_texture();

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif // GUIPANE_H
