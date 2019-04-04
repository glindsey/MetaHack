#ifndef MESSAGELOGVIEW_H
#define MESSAGELOGVIEW_H

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

#include "design_patterns/Object.h"
#include "events/UIEvents.h"
#include "sfgui_extensions/Widgets.hpp"

/// Forward declarations
class GameLog;

/// A class that acts as a View for a MessageLog model instance.
class MessageLogView
  :
  public Object
{
public:
  struct EventCommandReady : public ConcreteEvent<EventCommandReady>
  {
    EventCommandReady(std::string command_)
      :
      command{ command_ }
    {}

    std::string const command;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | string: \"" << command << "\"";
    }
  };

  /// Create a message log view tied to the specified model.
  /// @param  sfgui       Reference to the SFGUI API.
  /// @param  desktop     Reference to the SFGUI Desktop object.
  /// @param  name        The name of the view.
  /// @param  model       The GameLog instance to tie the view to.
  /// @param  dimensions  The initial dimensions of the view.
  /// @warning Lifetime of this object MUST outlast the GameLog passed in!
  MessageLogView(sf::RenderWindow& renderWindow,
                 sfg::SFGUI& sfgui,
                 sfg::Desktop& desktop,
                 std::string name,
                 GameLog& model);

  virtual ~MessageLogView();

protected:
  virtual bool onEvent(Event const& event) override;

  /// Handle an SFGUI KeyPressed event.
  void handleKeyPressed();

  /// Handle an SFGUI TextChanged event.
  void handleTextChanged();

  /// Handle when the containing window is resized.
  void handleWindowResized();

private:
  /// Reference to the render window.
  sf::RenderWindow& m_renderWindow;

  /// Reference to the SFGUI API.
  sfg::SFGUI& m_sfgui;

  /// Reference to the SFGUI desktop.
  sfg::Desktop& m_desktop;

  /// The SFGUI window.
  sfg::Window::Ptr m_window;

  /// The SFGUI box layout widget.
  sfg::Box::Ptr m_layout;

  /// The listbox for viewing the message log. Actually a ScrolledWindow.
  sfg::ScrolledWindow::Ptr m_listbox;

  /// The box layout widget for the listbox.
  sfg::Box::Ptr m_listboxLayout;

  /// A vector of label pointers for the messages in the box.
  std::vector<sfg::Label::Ptr> m_labels;

  /// The input box for entering commands.
  /// Might not even want this here, honestly; debug commands may be better input via
  /// the console window.
  sfg::EntryPlus::Ptr m_inputbox;

  /// Reference to the associated GameLog model.
  GameLog& m_model;
};

#endif // MESSAGELOGVIEW_H
