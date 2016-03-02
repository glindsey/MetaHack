#ifndef STATUSAREA_H
#define STATUSAREA_H

#include <memory>
#include <SFML/Graphics.hpp>

#include "gui/GUIPane.h"

class StatusArea : public GUIPane
{
public:
  explicit StatusArea(sf::IntRect dimensions);
  virtual ~StatusArea();

  virtual EventResult handle_event(sf::Event& event) override;

protected:
  virtual std::string _render_contents(sf::RenderTexture& texture, int frame) override;

private:
  void render_attribute(sf::RenderTarget& target, std::string abbrev, std::string name, sf::Vector2f location);

  std::string get_test_label();

  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // STATUSAREA_H
