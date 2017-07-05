#pragma once

#include "GUIObject.h"

// Forward declarations
class Object;

namespace metagui
{
  class Desktop : public GUIObject
  {
  public:
    struct MouseButtonInfo
    {
      /// Whether this button is pressed.
      bool pressed = false;

      /// Whether a drag is currently in process with this button.
      bool dragging = false;

      /// Absolute location of the press or release.
      IntVec2 location{ 0, 0 };

      /// Time elapsed since the last button state change.
      /// @todo decouple from SFML
      sf::Clock elapsed;
    };

    explicit Desktop(Object& event_parent,
                     std::string name, 
                     UintVec2 size);

    virtual ~Desktop();

  protected:
    virtual bool onEvent_V(Event const& event) override;

    virtual void drawPreChildren_(sf::RenderTexture& texture, int frame) override final;

  private:
    /// The subject from which to receive input events.
    Object& m_event_parent;

    /// An array of data for each possible mouse button.
    std::array< MouseButtonInfo, sf::Mouse::ButtonCount > m_button_info;

    /// Constantly updated mouse location.
    IntVec2 m_mouseLocation;

    /// Static set of events provided by GUIDesktop.
    static std::unordered_set<EventID> const s_eventsEmitted;
  };
}; // end namespace metagui
