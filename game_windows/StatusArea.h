#ifndef STATUSAREA_H
#define STATUSAREA_H

#include "stdafx.h"

#include "GUIWindow.h"

class StatusArea : public metagui::Window
{
public:
  explicit StatusArea(std::string name, sf::IntRect dimensions);
  virtual ~StatusArea();

protected:
  virtual void drawContents_(sf::RenderTexture& texture, int frame) override;

private:
  void render_attribute(sf::RenderTarget& target, 
                        std::string abbrev, 
                        std::string key, 
                        RealVec2 location);

  std::string get_test_label();
};

#endif // STATUSAREA_H
