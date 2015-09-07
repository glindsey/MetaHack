#include "MessageLog.h"

#include "App.h"
#include "ConfigSettings.h"
#include "KeyBuffer.h"

#include <boost/log/trivial.hpp>
#include <deque>

struct MessageLog::Impl
{
  /// Maximum number of history lines saved before they start falling off
  /// the back end of the queue.
  unsigned int history_lines_saved;

  /// Queue of previous messages.
  std::deque<std::string> message_queue;

  /// Key buffer for the current command.
  KeyBuffer buffer;
};

MessageLog::MessageLog(sf::IntRect dimensions)
  : GUIPane(dimensions),
    pImpl(new Impl())
{
  pImpl->history_lines_saved = 250;  ///< @todo Move to ConfigSettings
}

MessageLog::~MessageLog()
{
  //dtor
}

void MessageLog::add(std::string message)
{
  message[0] = toupper(message[0]);

  pImpl->message_queue.push_front(message);

  while (pImpl->message_queue.size() > pImpl->history_lines_saved)
  {
    pImpl->message_queue.pop_back();
  }
}

KeyBuffer& MessageLog::get_key_buffer()
{
  return pImpl->buffer;
}

EventResult MessageLog::handle_event(sf::Event& event)
{
  switch (event.type)
  {
  case sf::Event::EventType::KeyPressed:
    return pImpl->buffer.handle_key_press(event.key);
  default:
    break;
  }

  return GUIPane::handle_event(event);
}

std::string MessageLog::render_contents(int frame)
{
  // Dimensions of the pane.
  sf::IntRect pane_dims = get_dimensions();

  // Our render texture.
  sf::RenderTexture& bg_texture = get_bg_texture();

  int lineSpacing = the_default_font.getLineSpacing(Settings.text_default_size);

  // Text offsets relative to the background rectangle.
  float text_offset_x = 3;
  float text_offset_y = 3;

  // Start at the bottom, most recent text and work upwards.
  float text_coord_x = text_offset_x;
  float text_coord_y = pane_dims.height - (lineSpacing + text_offset_y);

  sf::Text render_text;

  render_text.setFont(the_default_font);
  render_text.setCharacterSize(Settings.text_default_size);

  // If we have the focus, put the current command at the bottom of the log.
  if (get_focus() == true)
  {
    pImpl->buffer.render(bg_texture,
                        sf::Vector2f(text_coord_x, text_coord_y),
                        frame,
                        the_default_font,
                        Settings.text_default_size,
                        Settings.text_highlight_color);

    text_coord_y -= lineSpacing;
  }

  // Draw each of the message_queue in the queue.
  /// @todo Split lines that are too long instead of truncating them.
  for (std::deque<std::string>::iterator iter = pImpl->message_queue.begin();
       iter != pImpl->message_queue.end(); ++iter)
  {
    render_text.setString(*iter);
    render_text.setPosition(text_coord_x, text_coord_y);
    render_text.setColor(Settings.text_color);
    bg_texture.draw(render_text);
    if (text_coord_y < text_offset_y) break;
    text_coord_y -= lineSpacing;
  }

  return "Message Log";
}
