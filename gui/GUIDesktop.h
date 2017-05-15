#ifndef GUIDESKTOPPANE_H
#define GUIDESKTOPPANE_H

#include "stdafx.h"

#include "GUIObject.h"

// Forward declarations
class Subject;

namespace metagui
{
  class Desktop :
    public GUIObject,
    public GUIObjectVisitable<Desktop>
  {
  public:
    struct MouseButtonInfo
    {
      /// Whether this button is pressed.
      bool pressed;

      /// Whether a drag is currently in process with this button.
      bool dragging;

      /// Absolute location of the press or release.
      IntVec2 location;

      /// Time elapsed since the last button state change.
      sf::Clock elapsed;
    };

    explicit Desktop(Subject& event_parent,
                     std::string name, 
                     UintVec2 size);

    virtual ~Desktop();

    virtual std::unordered_set<EventID> registeredEvents() const;

  protected:
    virtual bool onEvent_V2(Event const& event) override;

    virtual void drawPreChildren_(sf::RenderTexture& texture, int frame) override final;

  private:
    /// The subject from which to receive input events.
    Subject& m_event_parent;

    /// An array of data for each possible mouse button.
    std::array< MouseButtonInfo, sf::Mouse::ButtonCount > m_button_info;

    /// Constantly updated mouse location.
    IntVec2 m_mouseLocation;
  };
}; // end namespace metagui

#endif // GUIDESKTOPPANE_H
