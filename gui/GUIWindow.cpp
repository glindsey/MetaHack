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
  Window::Window(std::string name, Vec2i location, Vec2u size)
    :
    Object(name, location, size)
  {
    // *** TESTING CODE ***
    // Set this object to be movable and resizable.
    set_flag("movable", true);
    set_flag("resizable", true);
  }

  Window::Window(std::string name, sf::IntRect dimensions)
    :
    Object(name, dimensions)
  {
    // *** TESTING CODE ***
    // Set this object to be movable and resizable.
    set_flag("movable", true);
    set_flag("resizable", true);
  }

  Window::~Window()
  {}

  // === PROTECTED METHODS ======================================================

  Vec2i Window::get_child_area_location()
  {
    Vec2i child_location{ 0, 0 };

    if (m_titlebar_cached == true)
    {
      child_location.y += get_child(get_name() + "_titlebar").get_size().y;
    }

    return child_location;
  }

  Vec2u Window::get_child_area_size()
  {
    auto child_size = get_size();

    if (m_titlebar_cached == true)
    {
      child_size.y -= get_child(get_name() + "_titlebar").get_size().y;
    }

    return child_size;
  }

  void Window::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
    auto& config = Service<IConfigSettings>::get();
    Vec2u size = get_size();

    float line_spacing_y = the_default_font.getLineSpacing(config.get<unsigned int>("text_default_size"));

    // Text offsets relative to the background rectangle.
    float text_offset_x = config.get<float>("window_text_offset_x");
    float text_offset_y = config.get<float>("window_text_offset_y");

    // Clear the target.
    texture.clear(config.get<sf::Color>("window_bg_color"));

    // Render subclass contents, if any.
    drawContents_(texture, frame);

    // Draw the border.
    float border_width = config.get<float>("window_border_width");
    m_border_shape.setPosition(Vec2f(border_width, border_width));
    m_border_shape.setSize(Vec2f(static_cast<float>(size.x - (2 * border_width)), static_cast<float>(size.y - (2 * border_width))));
    m_border_shape.setFillColor(sf::Color::Transparent);
    m_border_shape.setOutlineColor(
      get_focus() ?
      config.get<sf::Color>("window_focused_border_color") :
      config.get<sf::Color>("window_border_color"));
    m_border_shape.setOutlineThickness(border_width);

    //texture.setView(sf::View(sf::FloatRect(0.0f, 0.0f, static_cast<float>(target.getSize().x), static_cast<float>(target.getSize().y))));

    texture.draw(m_border_shape);
  }

  void Window::handle_set_flag_(std::string name, bool enabled)
  {
    if (name == "titlebar")
    {
      m_titlebar_cached = enabled;

      std::string name = get_name() + "_titlebar";

      if (enabled)
      {
        if (!child_exists(name))
        {
          add_child(new TitleBar(name)).set_flag("decor", true);
        }
      }
      else
      {
        remove_child(name);
      }
    }
    else if (name == "resizable")
    {
      std::string name = get_name() + "_resizehandle";
      if (enabled)
      {
        if (!child_exists(name))
        {
          add_child(new ResizeHandle(name)).set_flag("decor", true);
        }
      }
      else
      {
        remove_child(name);
      }
    }
    else if (name == "closable")
    {
      std::string name = get_name() + "_closehandle";
      if (enabled)
      {
        if (!child_exists(name))
        {
          add_child(new CloseHandle(name)).set_flag("decor", true);
        }
      }
      else
      {
        remove_child(name);
      }
    }
    else if (name == "shrinkable")
    {
      std::string name = get_name() + "_shrinkhandle";
      if (enabled)
      {
        if (!child_exists(name))
        {
          add_child(new ShrinkHandle(name)).set_flag("decor", true);
        }
      }
      else
      {
        remove_child(name);
      }
    }
  }

  void Window::drawContents_(sf::RenderTexture& texture, int frame)
  {}

  Event::Result Window::handle_event_after_children_(EventDragStarted & event)
  {
    // If the window has a titlebar, only allow window dragging via the titlebar.
    if (child_exists(get_name() + "_titlebar"))
    {
      auto& child = get_child(get_name() + "_titlebar");
      if (!child.contains_point(event.start_location))
      {
        return Event::Result::Handled;
      }
    }

    return Event::Result::Acknowledged;
  }
}; // end namespace metagui