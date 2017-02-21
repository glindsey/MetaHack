#include "stdafx.h"

#include "MessageLogView.h"

#include "App.h"
#include "ErrorHandler.h"
#include "IConfigSettings.h"
#include "MessageLog.h"
#include "Service.h"

MessageLogView::MessageLogView(std::string name,
                               MessageLog& model,
                               sf::IntRect dimensions)
  :
  metagui::Window(name, dimensions),
  m_model(model)
{
  set_text("Message Log");
}

MessageLogView::~MessageLogView()
{}

metagui::Event::Result MessageLogView::handle_event_before_children_(metagui::EventKeyPressed& event)
{
  CLOG(TRACE, "GUI") << "MessageLogView::handle_event_before_children_(EventKeyPressed&) called";

  /// @todo This is ugly, fix later
  if (get_global_focus() == true)
  {
    return static_cast<metagui::Event::Result>(m_model.get_key_buffer().handle_key_press(event));
  }
  else
  {
    return metagui::Event::Result::Acknowledged;
  }
}

void MessageLogView::render_contents_(sf::RenderTexture& texture, int frame)
{
  auto& config = Service<IConfigSettings>::get();
  auto text_default_size = config.get<unsigned int>("text_default_size");

  // Dimensions of the pane.
  sf::IntRect pane_dims = get_relative_dimensions();

  float lineSpacing = the_default_font.getLineSpacing(text_default_size);

  // Text offsets relative to the background rectangle.
  float text_offset_x = config.get<float>("window_text_offset_x");
  float text_offset_y = config.get<float>("window_text_offset_y");

  // Start at the bottom, most recent text and work upwards.
  float text_coord_x = text_offset_x;
  float text_coord_y = pane_dims.height - (lineSpacing + text_offset_y);

  sf::Text render_text;

  render_text.setFont(the_default_font);
  render_text.setCharacterSize(text_default_size);

  // If we have the focus, put the current command at the bottom of the log.
  if (get_focus() == true)
  {
    m_model.get_key_buffer().render(
      texture,
      Vec2f(text_coord_x, text_coord_y),
      frame,
      the_default_font,
      text_default_size,
      config.get<sf::Color>("text_highlight_color"));

    text_coord_y -= lineSpacing;
  }

  // Draw each of the message_queue in the queue.
  /// @todo Split lines that are too long instead of truncating them.
  auto& message_queue = m_model.get_message_queue();

  for (auto iter = message_queue.begin(); iter != message_queue.end(); ++iter)
  {
    render_text.setString(*iter);
    render_text.setPosition(text_coord_x, text_coord_y);
    render_text.setColor(config.get<sf::Color>("text_color"));
    texture.draw(render_text);
    if (text_coord_y < text_offset_y) break;
    text_coord_y -= lineSpacing;
  }

  return;
}