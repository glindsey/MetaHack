#include "MessageLog.h"

#include "App.h"
#include "ConfigSettings.h"
#include "ErrorHandler.h"
#include "KeyBuffer.h"

#include <deque>

struct MessageLog::Impl
{
  /// Boolean indicating whether log window has the focus.
  bool focus;

  /// Dimensions of the message log window.
  sf::IntRect dims;

  /// Maximum number of history lines saved before they start falling off
  /// the back end of the queue.
  unsigned int history_lines_saved;

  /// Queue of previous messages.
  std::deque<std::string> message_queue;

  /// Background texture used in rendering.
  std::unique_ptr<sf::RenderTexture> area_bg_texture;

  /// Rectangle shape used in rendering.
  sf::RectangleShape area_bg_shape;

  /// Key buffer for the current command.
  KeyBuffer buffer;
};

MessageLog::MessageLog(sf::IntRect dimensions)
  : impl(new Impl())
{
  impl->focus = false;
  impl->history_lines_saved = 250;  ///< @todo Move to ConfigSettings

  this->set_dimensions(dimensions);
}

MessageLog::~MessageLog()
{
  //dtor
}

void MessageLog::set_focus(bool focus)
{
  impl->focus = focus;
}

bool MessageLog::get_focus()
{
  return impl->focus;
}

sf::IntRect MessageLog::get_dimensions()
{
  return impl->dims;
}

void MessageLog::set_dimensions(sf::IntRect rect)
{
  impl->dims = rect;
  impl->area_bg_texture.reset(new sf::RenderTexture());
  impl->area_bg_texture->create(rect.width, rect.height);
}

void MessageLog::add(std::string message)
{
  message[0] = toupper(message[0]);

  impl->message_queue.push_front(message);

  while (impl->message_queue.size() > impl->history_lines_saved)
  {
    impl->message_queue.pop_back();
  }
}

KeyBuffer& MessageLog::get_key_buffer()
{
  return impl->buffer;
}

EventResult MessageLog::handle_event(sf::Event& event)
{
  switch (event.type)
  {
  case sf::Event::EventType::KeyPressed:
    return impl->buffer.handle_key_press(event.key);
  default:
    break;
  }

  return EventResult::Ignored;
}

bool MessageLog::render(sf::RenderTarget& target, int frame)
{
  int lineSpacing = the_default_font.getLineSpacing(Settings.text_default_size);

  // Text offsets relative to the background rectangle.
  float text_offset_x = 3;
  float text_offset_y = 3;

  // Start at the bottom, most recent text and work upwards.
  float text_coord_x = text_offset_x;
  float text_coord_y = impl->dims.height - (lineSpacing + text_offset_y);

  sf::Text render_text;

  render_text.setFont(the_default_font);
  render_text.setCharacterSize(Settings.text_default_size);

  // Clear background texture.
  impl->area_bg_texture->clear(Settings.window_bg_color);

  // If we have the focus, put the current command at the bottom of the log.
  if (impl->focus)
  {
    impl->buffer.render(*(impl->area_bg_texture.get()),
                        sf::Vector2f(text_coord_x, text_coord_y),
                        frame,
                        the_default_font,
                        Settings.text_default_size,
                        Settings.text_highlight_color);

    text_coord_y -= lineSpacing;
  }

  // Draw each of the message_queue in the queue.
  /// @todo Split lines that are too long instead of truncating them.
  for (std::deque<std::string>::iterator iter = impl->message_queue.begin();
       iter != impl->message_queue.end(); ++iter)
  {
    render_text.setString(*iter);
    render_text.setPosition(text_coord_x, text_coord_y);
    render_text.setColor(Settings.text_color);
    impl->area_bg_texture->draw(render_text);
    if (text_coord_y < text_offset_y) break;
    text_coord_y -= lineSpacing;
  }

  // Draw a little window title in the upper-left corner.
  sf::RectangleShape title_rect;
  sf::Text title_text;
  title_text.setString("Message Log");
  title_text.setFont(the_default_bold_font);
  title_text.setCharacterSize(Settings.text_default_size);

  sf::FloatRect const& text_bounds = title_text.getGlobalBounds();

  title_rect.setFillColor(Settings.window_bg_color);
  title_rect.setOutlineColor(impl->focus ?
                             Settings.window_focused_border_color :
                             Settings.window_border_color);
  title_rect.setOutlineThickness(Settings.window_border_width);
  title_rect.setPosition(sf::Vector2f(0, 0));
  title_rect.setSize(sf::Vector2f(text_bounds.width + (text_offset_x * 2),
                                  (text_bounds.height * 1.25) + (text_offset_y * 2)));

  impl->area_bg_texture->draw(title_rect);

  title_text.setColor(Settings.text_color);
  title_text.setPosition(sf::Vector2f(text_offset_x, text_offset_y));
  impl->area_bg_texture->draw(title_text);

  // Render to the message texture.
  impl->area_bg_texture->display();

  // Draw the rectangle.
  impl->area_bg_shape.setPosition(sf::Vector2f(impl->dims.left,
                                               impl->dims.top));
  impl->area_bg_shape.setSize(sf::Vector2f(impl->dims.width,
                                           impl->dims.height));
  impl->area_bg_shape.setTexture(&(impl->area_bg_texture->getTexture()));
  impl->area_bg_shape.setTextureRect(sf::IntRect(0, 0,
                                     impl->dims.width,
                                     impl->dims.height));
  impl->area_bg_shape.setOutlineColor(impl->focus ?
                                      Settings.window_focused_border_color :
                                      Settings.window_border_color);
  impl->area_bg_shape.setOutlineThickness(Settings.window_border_width);

  target.setView(sf::View(sf::FloatRect(0, 0,
                          target.getSize().x,
                          target.getSize().y)));

  target.draw(impl->area_bg_shape);

  return true;
}
