#ifndef GUILABEL_H
#define GUILABEL_H

#include <functional>
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

#include "gui/GUIObject.h"

class GUILabel :
  public GUIObject
{
  public:
    GUILabel(sf::IntRect dimensions, std::function<std::string()> string_function);
    virtual ~GUILabel();

    std::function<std::string()> get_string_function();
    void set_string_function(std::function<std::string()> string_function);

    virtual EventResult handle_event(sf::Event& event);


  protected:
    virtual bool _render_self(sf::RenderTarget& target, int frame) final;

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif // GUILABEL_H
