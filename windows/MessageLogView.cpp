#include "stdafx.h"

#include "windows/MessageLogView.h"

#include "game/App.h"
#include "keybuffer/IKeyBuffer.h"
#include "services/IConfigSettings.h"
#include "services/IMessageLog.h"
#include "Service.h"

MessageLogView::MessageLogView(std::string name,
                               IMessageLog& model,
                               IKeyBuffer& key_buffer,
                               sf::IntRect dimensions)
  :
  metagui::Window(name, dimensions),
  m_model(model),
  m_key_buffer(key_buffer)
{
  setText("Message Log");
  m_model.addObserver(*this, EventID::All);
  m_key_buffer.addObserver(*this, EventID::All);
}

MessageLogView::~MessageLogView()
{
  m_key_buffer.removeObserver(*this);
  m_model.removeObserver(*this);
}

metagui::Event::Result MessageLogView::handleGUIEventPreChildren_(metagui::EventKeyPressed& event)
{
  CLOG(TRACE, "GUI") << "MessageLogView::handleGUIEventPreChildren_(EventKeyPressed&) called";

  /// @todo This is ugly, fix later
  if (getGlobalFocus() == true)
  {
    flagForRedraw();
    return static_cast<metagui::Event::Result>(m_key_buffer.handle_key_press(event));
  }
  else
  {
    return metagui::Event::Result::Acknowledged;
  }
}

void MessageLogView::drawContents_(sf::RenderTexture& texture, int frame)
{
  auto& config = Service<IConfigSettings>::get();
  auto text_default_size = config.get("text_default_size").as<int32_t>();

  // Dimensions of the pane.
  sf::IntRect pane_dims = getRelativeDimensions();

  float lineSpacing = the_default_font.getLineSpacing(text_default_size);

  // Text offsets relative to the background rectangle.
  auto text_offset_x = config.get("window_text_offset_x").as<float>();
  auto text_offset_y = config.get("window_text_offset_y").as<float>();

  // Start at the bottom, most recent text and work upwards.
  float text_coord_x = text_offset_x;
  float text_coord_y = pane_dims.height - (lineSpacing + text_offset_y);

  sf::Text render_text;

  render_text.setFont(the_default_font);
  render_text.setCharacterSize(text_default_size);

  // If we have the focus, put the current command at the bottom of the log.
  if (getFocus() == true)
  {
    m_key_buffer.render(
      texture,
      RealVec2(text_coord_x, text_coord_y),
      frame,
      the_default_font,
      text_default_size,
      config.get("text_highlight_color").as<Color>());

    text_coord_y -= lineSpacing;
  }

  // Draw each of the message_queue in the queue.
  /// @todo Split lines that are too long instead of truncating them.
  auto& message_queue = m_model.getMessageQueue();

  for (auto iter = message_queue.begin(); iter != message_queue.end(); ++iter)
  {
    render_text.setString(*iter);
    render_text.setPosition(text_coord_x, text_coord_y);
    render_text.setColor(config.get("text_color").as<Color>());
    texture.draw(render_text);
    if (text_coord_y < text_offset_y) break;
    text_coord_y -= lineSpacing;
  }

  return;
}

bool MessageLogView::onEvent_(Event const & event)
{
  /// @todo Flesh this out a bit more.
  ///       Right now we just set the "dirty" flag for the view so it is redrawn.
  flagForRedraw();
  return true;
}