#ifndef GUIPANE_H
#define GUIPANE_H

#include "stdafx.h"

#include "gui/GUIObject.h"

class GUIPane :
  public GUIObject
{
public:
  explicit GUIPane(sf::IntRect dimensions);
  virtual ~GUIPane();

  virtual EventResult handle_event(sf::Event& event);

protected:
  virtual bool _render_self(sf::RenderTexture& texture, int frame) override final;
  virtual std::string _render_contents(sf::RenderTexture& texture, int frame);

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // GUIPANE_H
