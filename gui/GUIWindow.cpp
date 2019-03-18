#include "stdafx.h"

#include "GUIWindow.h"

#include "GUITitleBar.h"

#include "config/Settings.h"
#include "game/App.h"
#include "services/Service.h"
#include "types/Color.h"

namespace metagui
{
  Window::Window(Desktop& desktop, std::string name, IntVec2 location, UintVec2 size) :
    GUIObject(desktop, name, {}, location, size)
  {
    // *** TESTING CODE ***
    // Set this object to be movable and resizable.
    setFlag("movable", true);
    setFlag("resizable", true);

    setFlag("hasShadow", true);
  }

  Window::Window(Desktop& desktop, std::string name, sf::IntRect dimensions)
    :
    GUIObject(desktop, name, {}, dimensions)
  {
    // *** TESTING CODE ***
    // Set this object to be movable and resizable.
    setFlag("movable", true);
    setFlag("resizable", true);

    setFlag("hasShadow", true);
  }

  Window::~Window()
  {
  }

  // === PROTECTED METHODS ======================================================

  IntVec2 Window::getChildAreaLocation()
  {
    IntVec2 child_location{ 0, 0 };

    if (m_titlebar_cached == true)
    {
      child_location.y += getChild(getName() + "_titlebar").getSize().y;
    }

    return child_location;
  }

  UintVec2 Window::getChildAreaSize()
  {
    auto child_size = getSize();

    if (m_titlebar_cached == true)
    {
      child_size.y -= getChild(getName() + "_titlebar").getSize().y;
    }

    return child_size;
  }

  void Window::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
    auto& config = Config::settings();
    UintVec2 size = getSize();

    float line_spacing_y = the_default_font.getLineSpacing(config.get("text-default-size"));

    // Clear the target.
    auto bg_color = config.get("window-bg-color").get<Color>();
    auto focused_color = config.get("window-focused-border-color").get<Color>();
    auto unfocused_color = config.get("window-border-color").get<Color>();

    texture.clear(bg_color);

    // Render subclass contents, if any.
    drawContents_(texture, frame);

    // Draw the border.
    float border_width = config.get("window-border-width");
    m_border_shape.setPosition(RealVec2(border_width, border_width));
    m_border_shape.setSize(RealVec2(static_cast<float>(size.x - (2 * border_width)), static_cast<float>(size.y - (2 * border_width))));
    m_border_shape.setFillColor(Color::Transparent);
    m_border_shape.setOutlineColor(getFocus() ? focused_color : unfocused_color);
    m_border_shape.setOutlineThickness(border_width);

    texture.draw(m_border_shape);
  }

  void Window::handleSetFlag_V(std::string name, bool enabled)
  {
    if (name == "titlebar")
    {
      m_titlebar_cached = enabled;

      std::string name = getName() + "_titlebar";

      if (enabled)
      {
        if (!childExists(name))
        {
          addChild(new TitleBar(desktop(), name))->setFlag("decor", true);
        }
      }
      else
      {
        removeChild(name);
      }
    }
  }

  void Window::drawContents_(sf::RenderTexture& texture, int frame)
  {}

  bool Window::onEvent_V(Event const& event)
  {
    auto id = event.getId();
    if (id == EventDragStarted::id)
    {
      // If the window has a titlebar, only allow window dragging via the titlebar.
      if (childExists(getName() + "_titlebar"))
      {
        auto& castEvent = static_cast<EventDragStarted const&>(event);
        auto& child = getChild(getName() + "_titlebar");
        if (!child.containsPoint(castEvent.startLocation))
        {
          return true;  /// "true" swallows the event here
        }
      }
    }

    return false;
  }
}; // end namespace metagui
