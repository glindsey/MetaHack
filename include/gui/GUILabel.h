#ifndef GUILABEL_H
#define GUILABEL_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

#include "gui/GUIObject.h"

class GUILabel :
  public GUIObject
{
  public:
    GUILabel(sf::IntRect dimensions, std::weak_ptr<std::string> text_ptr);
    virtual ~GUILabel();

    std::weak_ptr<std::string> const& get_text_pointer();
    void set_text_pointer(std::shared_ptr<std::string> text_ptr);

    virtual EventResult handle_event(sf::Event& event);


  protected:
    virtual bool _render_self(sf::RenderTarget& target, int frame) final;

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif // GUILABEL_H
