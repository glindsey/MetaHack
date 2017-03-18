#include "stdafx.h"

#include "GUIWindow.h"

#include "GUITitleBar.h"
#include "GUICloseHandle.h"
#include "GUIResizeHandle.h"
#include "GUIShrinkHandle.h"

#include "App.h"
#include "IConfigSettings.h"
#include "Service.h"

namespace metagui
{
  Window::Window(std::string name, IntVec2 location, UintVec2 size)
    :
    Object(name, location, size)
  {
    // *** TESTING CODE ***
    // Set this object to be movable and resizable.
    setFlag("movable", true);
    setFlag("resizable", true);
  }

  Window::Window(std::string name, sf::IntRect dimensions)
    :
    Object(name, dimensions)
  {
    // *** TESTING CODE ***
    // Set this object to be movable and resizable.
    setFlag("movable", true);
    setFlag("resizable", true);
  }

  Window::~Window()
  {}

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
    auto& config = Service<IConfigSettings>::get();
    UintVec2 size = getSize();

    float line_spacing_y = the_default_font.getLineSpacing(config.get("text_default_size").as<uint32_t>());

    // Text offsets relative to the background rectangle.
    float text_offset_x = config.get("window_text_offset_x").as<float>();
    float text_offset_y = config.get("window_text_offset_y").as<float>();

    // Clear the target.
    texture.clear(config.get("window_bg_color").as<Color>());

    // Render subclass contents, if any.
    drawContents_(texture, frame);

    // Draw the border.
    float border_width = config.get("window_border_width").as<float>();
    m_border_shape.setPosition(RealVec2(border_width, border_width));
    m_border_shape.setSize(RealVec2(static_cast<float>(size.x - (2 * border_width)), static_cast<float>(size.y - (2 * border_width))));
    m_border_shape.setFillColor(sf::Color::Transparent);
    m_border_shape.setOutlineColor(
      getFocus() ?
      config.get("window_focused_border_color").as<Color>() :
      config.get("window_border_color").as<Color>());
    m_border_shape.setOutlineThickness(border_width);

    //texture.setView(sf::View(sf::FloatRect(0.0f, 0.0f, static_cast<float>(target.getSize().x), static_cast<float>(target.getSize().y))));

    texture.draw(m_border_shape);
  }

  void Window::handleSetFlag_(std::string name, bool enabled)
  {
    if (name == "titlebar")
    {
      m_titlebar_cached = enabled;

      std::string name = getName() + "_titlebar";

      if (enabled)
      {
        if (!childExists(name))
        {
          addChild(new TitleBar(name))->setFlag("decor", true);
        }
      }
      else
      {
        removeChild(name);
      }
    }
    else if (name == "resizable")
    {
      std::string name = getName() + "_resizehandle";
      if (enabled)
      {
        if (!childExists(name))
        {
          addChild(new ResizeHandle(name))->setFlag("decor", true);
        }
      }
      else
      {
        removeChild(name);
      }
    }
    else if (name == "closable")
    {
      std::string name = getName() + "_closehandle";
      if (enabled)
      {
        if (!childExists(name))
        {
          addChild(new CloseHandle(name))->setFlag("decor", true);
        }
      }
      else
      {
        removeChild(name);
      }
    }
    else if (name == "shrinkable")
    {
      std::string name = getName() + "_shrinkhandle";
      if (enabled)
      {
        if (!childExists(name))
        {
          addChild(new ShrinkHandle(name))->setFlag("decor", true);
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

  Event::Result Window::handleGUIEventPostChildren_(EventDragStarted & event)
  {
    // If the window has a titlebar, only allow window dragging via the titlebar.
    if (childExists(getName() + "_titlebar"))
    {
      auto& child = getChild(getName() + "_titlebar");
      if (!child.containsPoint(event.start_location))
      {
        return Event::Result::Handled;
      }
    }

    return Event::Result::Acknowledged;
  }
}; // end namespace metagui