#ifndef MESSAGELOGVIEW_H
#define MESSAGELOGVIEW_H

#include "stdafx.h"

#include "events/UIEvents.h"
#include "gui/GUIWindow.h"
#include "Observer.h"


/// Forward declarations
class IKeyBuffer;
class IMessageLog;

/// A class that acts as a View for a MessageLog model instance.
class MessageLogView 
  : 
  public metagui::Window
{
public:
  /// Create a message log view tied to the specified model.
  /// @param  name        The name of the view.
  /// @param  model       The MessageLog instance to tie the view to.
  /// @param  key_buffer  The KeyBuffer instance to tie the view to.
  /// @param  dimensions  The initial dimensions of the view.
  /// @warning Lifetime of this object MUST outlast the MessageLog/KeyBuffer passed in!
  MessageLogView(std::string name, 
                 IMessageLog& model,
                 IKeyBuffer& key_buffer,
                 sf::IntRect dimensions);

  virtual ~MessageLogView();

protected:
  /// Render the contents of this view to the specified target.
  /// @param  target  RenderTarget to render onto.
  /// @param  frame   Current frame counter.
  virtual void drawContents_(sf::RenderTexture& texture, int frame) override;

  virtual bool onEvent_V(Event const& event) override;

  /// Handle a KeyPressed event.
  bool handleKeyPress(UIEvents::EventKeyPressed const& event);

private:

  /// Reference to the associated MessageLog model.
  IMessageLog& m_model;

  /// Reference to the associated KeyBuffer model.
  IKeyBuffer& m_key_buffer;
};

#endif // MESSAGELOGVIEW_H
