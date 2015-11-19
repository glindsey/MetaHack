#ifndef GUILABEL_H
#define GUILABEL_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

#include "EventHandler.h"
#include "Renderable.h"

class GUILabel :
  public EventHandler,
  public Renderable
{
  public:
    GUILabel(sf::IntRect dimensions, std::weak_ptr<std::string> text_ptr);
    virtual ~GUILabel();

    void set_focus(bool focus);
    bool get_focus();

    sf::IntRect get_dimensions();
    void set_dimensions(sf::IntRect dimensions);

    std::weak_ptr<std::string> const& get_text_pointer();
    void set_text_pointer(std::shared_ptr<std::string> text_ptr);

    virtual EventResult handle_event(sf::Event& event);

    virtual bool render(sf::RenderTarget& target, int frame) final;

  protected:
    sf::RenderTexture& get_bg_texture();

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif // GUILABEL_H
