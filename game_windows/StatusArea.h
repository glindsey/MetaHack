#ifndef STATUSAREA_H
#define STATUSAREA_H

#include "GUIWindow.h"

// Forward declarations
class GameState;

class StatusArea : public metagui::Window
{
public:
  explicit StatusArea(std::string name, sf::IntRect dimensions, GameState& gameState);
  virtual ~StatusArea();

protected:
  virtual void drawContents_(sf::RenderTexture& texture, int frame) override;
  virtual void doEventSubscriptions_V(Object& parent) override;
  virtual bool onEvent_V(Event const& event) override;

private:
  GameState& m_gameState;

  void render_attribute(sf::RenderTarget& target, 
                        std::string abbrev, 
                        std::string key, 
                        RealVec2 location);

  std::string get_test_label();
};

#endif // STATUSAREA_H
