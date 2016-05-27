#ifndef STATUSAREA_H
#define STATUSAREA_H

#include "stdafx.h"

#include "GUIWindow.h"

class StatusArea : public metagui::Window
{
public:
  explicit StatusArea(sf::IntRect dimensions);
  virtual ~StatusArea();

protected:
  virtual void render_contents_(sf::RenderTexture& texture, int frame) override;

private:
  void render_attribute(sf::RenderTarget& target, StringDisplay abbrev, StringKey name, sf::Vector2f location);

  StringDisplay get_test_label();

  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // STATUSAREA_H
