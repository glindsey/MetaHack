#ifndef GUIDESKTOPPANE_H
#define GUIDESKTOPPANE_H

#include "stdafx.h"

#include "gui/GUIObject.h"

namespace metagui
{
  class DesktopPane :
    public Object,
    public ObjectVisitable<DesktopPane>
  {
  public:
    struct MouseButtonInfo
    {
      /// Whether this button is pressed.
      bool pressed;

      /// Whether a drag is currently in process with this button.
      bool dragging;

      /// Absolute location of the press or release.
      sf::Vector2i location;

      /// Time elapsed since the last button state change.
      sf::Clock elapsed;
    };

    explicit DesktopPane(StringKey name, sf::Vector2u size);
    virtual ~DesktopPane();

    /// Handles an SFML event and translates it into a GUI event if necessary.
    SFMLEventResult handle_sfml_event(sf::Event& sfml_event);

  protected:
    virtual metagui::Event::Result handle_event_before_children_(EventResized& event) final;

    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;

  private:
    /// An array of data for each possible mouse button.
    std::array< MouseButtonInfo, sf::Mouse::ButtonCount > m_button_info;

    /// Constantly updated mouse location.
    sf::Vector2i m_mouse_location;
  };
}; // end namespace metagui

#endif // GUIDESKTOPPANE_H
