#include "MessageLog.h"

#include "App.h"
#include "ErrorHandler.h"

#include <deque>

struct MessageLog::Impl
{
    bool focus;
    unsigned int font_size;
    sf::IntRect dims;
    sf::Color area_bg_color;
    unsigned int history_lines_saved;

    std::deque<std::string> message_queue;
    std::unique_ptr<sf::RenderTexture> area_bg_texture;
    sf::RectangleShape area_bg_shape;
};

MessageLog::MessageLog(sf::IntRect dimensions)
  : impl(new Impl())
{
  impl->focus = false;
  impl->font_size = 16;
  impl->area_bg_color = the_window_bg_color;
  impl->history_lines_saved = 50;

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

EventResult MessageLog::handle_event(sf::Event& event)
{
  return EventResult::Ignored;
}

bool MessageLog::render(sf::RenderTarget& target, int frame)
{
  int lineSpacing = the_default_font.getLineSpacing(impl->font_size);

  // Text offsets relative to the background rectangle.
  float text_offset_x = 3;
  float text_offset_y = 3;

  // Start at the bottom, most recent text and work upwards.
  float text_coord_x = text_offset_x;
  float text_coord_y = impl->dims.height - (lineSpacing + text_offset_y);

  sf::Text renderText;

  renderText.setFont(the_default_font);
  renderText.setCharacterSize(impl->font_size);

  // Clear background texture.
  impl->area_bg_texture->clear(impl->area_bg_color);

  // Draw each of the message_queue in the queue.
  // TODO: At the moment this does not split lines that are too long, instead
  //       truncating them at the edge of the message box.  This must be fixed.
  for (std::deque<std::string>::iterator iter = impl->message_queue.begin();
       iter != impl->message_queue.end(); ++iter)
  {
    renderText.setString(*iter);
    renderText.setPosition(text_coord_x - 2, text_coord_y - 2);
    renderText.setColor(sf::Color::Black);
    impl->area_bg_texture->draw(renderText);
    renderText.setPosition(text_coord_x, text_coord_y);
    renderText.setColor(sf::Color::White);
    impl->area_bg_texture->draw(renderText);
    if (text_coord_y < text_offset_y) break;
    text_coord_y -= lineSpacing;
  }

  // Draw a little window title in the upper-left corner.
  sf::RectangleShape title_rect;
  sf::Text title_text;
  title_text.setString("Message Log");
  title_text.setFont(the_default_bold_font);
  title_text.setCharacterSize(impl->font_size);

  sf::FloatRect const& text_bounds = title_text.getLocalBounds();

  title_rect.setFillColor(the_window_bg_color);
  title_rect.setOutlineColor(impl->focus ?
                            sf::Color::Yellow :
                            sf::Color::White);
  title_rect.setOutlineThickness(2.0f);
  title_rect.setPosition(sf::Vector2f(0, 0));
  title_rect.setSize(sf::Vector2f(text_bounds.width + (text_offset_x * 2),
                                 text_bounds.height + (text_offset_y * 2)));

  impl->area_bg_texture->draw(title_rect);

  title_text.setColor(sf::Color::Black);
  title_text.setPosition(sf::Vector2f(text_offset_x - 2, text_offset_y - 2));
  impl->area_bg_texture->draw(title_text);
  title_text.setColor(sf::Color::White);
  title_text.setPosition(sf::Vector2f(text_offset_x, text_offset_y));
  impl->area_bg_texture->draw(title_text);

  // Render to the message texture.
  impl->area_bg_texture->display();

  // Draw the rectangle.
  impl->area_bg_shape.setPosition(sf::Vector2f(impl->dims.left, impl->dims.top));
  impl->area_bg_shape.setSize(sf::Vector2f(impl->dims.width, impl->dims.height));
  impl->area_bg_shape.setTexture(&(impl->area_bg_texture->getTexture()));
  impl->area_bg_shape.setTextureRect(sf::IntRect(0, 0,
                                               impl->dims.width,
                                               impl->dims.height));
  impl->area_bg_shape.setOutlineColor(impl->focus ?
                                    sf::Color::Yellow :
                                    sf::Color::White);
  impl->area_bg_shape.setOutlineThickness(2.0f);

  target.setView(sf::View(sf::FloatRect(0, 0,
                          target.getSize().x,
                          target.getSize().y)));

  target.draw(impl->area_bg_shape);

  return true;
}
