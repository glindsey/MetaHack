#include "stdafx.h"

#include "MessageLogView.h"

#include "App.h"
#include "ConfigSettings.h"
#include "ErrorHandler.h"
#include "MessageLog.h"
#include "New.h"

struct MessageLogView::Impl
{
  /// Constructor.
  explicit Impl(MessageLog& model_)
    :
    model(model_)
  {}

  /// Reference to the associated MessageLog model.
  MessageLog& model;
};

MessageLogView::MessageLogView(MessageLog& model,
                               sf::IntRect dimensions)
  :
  GUIPane(dimensions),
  pImpl(NEW Impl(model))
{}

MessageLogView::~MessageLogView()
{}

EventResult MessageLogView::handle_event(sf::Event& event)
{
  switch (event.type)
  {
    case sf::Event::EventType::KeyPressed:
      return pImpl->model.get_key_buffer().handle_key_press(event.key);
    default:
      break;
  }

  return GUIPane::handle_event(event);
}

std::string MessageLogView::_render_contents(sf::RenderTexture& texture, int frame)
{
  // Dimensions of the pane.
  sf::IntRect pane_dims = get_relative_dimensions();

  float lineSpacing = the_default_font.getLineSpacing(Settings.get<unsigned int>("text_default_size"));

  // Text offsets relative to the background rectangle.
  float text_offset_x = 3;
  float text_offset_y = 3;

  // Start at the bottom, most recent text and work upwards.
  float text_coord_x = text_offset_x;
  float text_coord_y = pane_dims.height - (lineSpacing + text_offset_y);

  sf::Text render_text;

  render_text.setFont(the_default_font);
  render_text.setCharacterSize(Settings.get<unsigned int>("text_default_size"));

  // If we have the focus, put the current command at the bottom of the log.
  if (get_focus() == true)
  {
    pImpl->model.get_key_buffer().render(
      texture,
      sf::Vector2f(text_coord_x, text_coord_y),
      frame,
      the_default_font,
      Settings.get<unsigned int>("text_default_size"),
      Settings.get<sf::Color>("text_highlight_color"));

    text_coord_y -= lineSpacing;
  }

  // Draw each of the message_queue in the queue.
  /// @todo Split lines that are too long instead of truncating them.
  auto& message_queue = pImpl->model.get_message_queue();

  for (auto iter = message_queue.begin(); iter != message_queue.end(); ++iter)
  {
    render_text.setString(*iter);
    render_text.setPosition(text_coord_x, text_coord_y);
    render_text.setColor(Settings.get<sf::Color>("text_color"));
    texture.draw(render_text);
    if (text_coord_y < text_offset_y) break;
    text_coord_y -= lineSpacing;
  }

  return "Message Log";
}