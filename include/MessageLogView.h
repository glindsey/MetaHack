#ifndef MESSAGELOGVIEW_H
#define MESSAGELOGVIEW_H

#include "stdafx.h"

#include "gui/GUIWindowPane.h"

/// Forward declarations
class MessageLog;

/// A class that acts as a View for a MessageLog model instance.
class MessageLogView : public GUIWindowPane
{
public:
  /// Create a message log view tied to the specified model.
  /// @param  model       The MessageLog model to tie the view to.
  /// @param  dimensions  The initial dimensions of the view.
  MessageLogView(MessageLog& model,
                 sf::IntRect dimensions);

  virtual ~MessageLogView();

  /// Handle an SFML event.
  virtual EventResult handle_event(sf::Event& event) override;

protected:
  /// Render the contents of this view to the specified target.
  /// @param  target  RenderTarget to render onto.
  /// @param  frame   Current frame counter.
  virtual std::string render_contents_(sf::RenderTexture& texture, int frame) override;

private:

  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // MESSAGELOGVIEW_H
