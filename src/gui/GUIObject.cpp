#include "stdafx.h"

#include "gui/GUIObject.h"

#include <exception>

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

namespace metagui
{
  Object::Object(std::string name, sf::Vector2i location, sf::Vector2i size)
  {
    set_relative_location(location);
    set_size(size);
  }

  Object::Object(std::string name, sf::IntRect dimensions)
  {
    set_relative_dimensions(dimensions);
  }

  Object::~Object()
  {
    //dtor
  }

  std::string Object::get_name()
  {
    return m_name;
  }

  void Object::set_focus(bool focus)
  {
    if (m_parent != nullptr)
    {
      m_parent->clear_child_focuses();
    }

    set_focus_only(focus);
  }

  bool Object::get_focus()
  {
    return m_focus;
  }

  bool Object::get_global_focus()
  {
    bool global_focus = m_focus;

    if ((global_focus == true) && (m_parent != nullptr))
    {
      global_focus &= m_parent->get_global_focus();
    }

    return global_focus;
  }

  void Object::set_global_focus(bool focus)
  {
    if (m_parent != nullptr)
    {
      m_parent->set_global_focus(focus);
    }

    set_focus(focus);
  }

  void Object::set_hidden(bool hidden)
  {
    m_hidden = hidden;
  }

  bool Object::get_hidden()
  {
    return m_hidden;
  }

  void Object::set_enabled(bool enabled)
  {
    m_enabled = enabled;
  }

  bool Object::get_enabled()
  {
    return m_enabled;
  }

  void Object::set_text(std::string text)
  {
    m_text = text;
  }

  std::string Object::get_text()
  {
    return m_text;
  }

  sf::Vector2i Object::get_relative_location()
  {
    return m_location;
  }

  void Object::set_relative_location(sf::Vector2i location)
  {
    m_location = location;
  }

  sf::Vector2i Object::get_size()
  {
    return m_size;
  }

  void Object::set_size(sf::Vector2i size)
  {
    m_size = size;
    m_bg_texture.reset(NEW sf::RenderTexture());
    m_bg_texture->create(size.x, size.y);
  }

  sf::IntRect Object::get_relative_dimensions()
  {
    sf::IntRect dimensions;
    sf::Vector2i location = get_relative_location();
    sf::Vector2i size = get_size();
    dimensions.left = location.x;
    dimensions.top = location.y;
    dimensions.width = size.x;
    dimensions.height = size.y;
    return dimensions;
  }

  void Object::set_relative_dimensions(sf::IntRect dimensions)
  {
    set_relative_location({ dimensions.left, dimensions.top });
    set_size({ dimensions.width, dimensions.height });
  }

  sf::Vector2i Object::get_absolute_location()
  {
    sf::Vector2i absolute_location = get_relative_location();

    if (m_parent != nullptr)
    {
      sf::Vector2i child_area_absolute_location =
        m_parent->get_absolute_location() + m_parent->get_child_area_location();

      absolute_location += child_area_absolute_location;
    }

    return absolute_location;
  }

  void Object::set_absolute_location(sf::Vector2i location)
  {
    sf::Vector2i relative_location = location;

    if (m_parent != nullptr)
    {
      sf::Vector2i child_area_absolute_location =
        m_parent->get_absolute_location() + m_parent->get_child_area_location();

      relative_location -= child_area_absolute_location;
    }

    set_relative_location(relative_location);
  }

  Object& Object::add_child(std::unique_ptr<Object> child, uint32_t z_order)
  {
    ASSERT_CONDITION(child);

    std::string name = child->get_name();
    ASSERT_CONDITION(child_exists(name) == false);

    Object& child_ref = *(child.get());
    child->set_parent(this);

    // This odd syntax is in order to work around VS compiler bug when having
    // a unique_ptr as a map value. See:
    // https://stackoverflow.com/questions/21056872/c-stdunique-ptr-wont-compile-in-map
    m_children.insert<ChildMap::value_type>(ChildMap::value_type(name, std::move(child)));
    m_zorder_map.insert({ z_order, name });

    return child_ref;
  }

  Object & Object::add_child(Object * child, uint32_t z_order)
  {
    std::unique_ptr<Object> child_ptr(child);
    return add_child(std::move(child), z_order);
  }

  Object& Object::add_child(std::unique_ptr<Object> child)
  {
    uint32_t z_order = get_highest_child_z_order() + 1;
    return add_child(std::move(child), z_order);
  }

  Object & Object::add_child(Object * child)
  {
    std::unique_ptr<Object> child_ptr(child);
    return add_child(std::move(child));
  }

  Object & Object::add_child_top(std::unique_ptr<Object> child)
  {
    uint32_t z_order = get_lowest_child_z_order() - 1;
    return add_child(std::move(child), z_order);
  }

  Object & Object::add_child_top(Object * child)
  {
    std::unique_ptr<Object> child_ptr(child);
    return add_child_top(std::move(child));
  }

  bool Object::child_exists(std::string name)
  {
    return (m_children.count(name) > 0);
  }

  Object& Object::get_child(std::string name)
  {
    if (child_exists(name))
    {
      return *(m_children.at(name).get());
    }

    std::string message = "Tried to get non-existent child \"" + name + "\" of GUI object \"" + get_name() + "\"";
    throw std::runtime_error(message.c_str());
  }

  std::unique_ptr<Object> Object::remove_child(std::string name)
  {
    if (child_exists(name))
    {
      // This moves the object out of the stored unique_ptr.
      std::unique_ptr<Object> moved_object = std::move(m_children.at(name));
      // This erases the (now empty) unique_ptr from the map.
      m_children.erase(name);

      // This finds the child name in the Z-order map and gets rid of it.
      for (auto& iter = m_zorder_map.begin(); iter != m_zorder_map.end(); ++iter)
      {
        if (iter->second == name)
        {
          m_zorder_map.erase(iter);
          break;
        }
      }

      // This returns the object we removed.
      return std::move(moved_object);
    }

    // Didn't find the object, so return an empty unique_ptr.
    return std::unique_ptr<Object>();
  }

  uint32_t Object::get_lowest_child_z_order()
  {
    if (m_zorder_map.size() > 0)
    {
      auto& iter = m_zorder_map.cbegin();
      return iter->first;
    }
    return 0;
  }

  uint32_t Object::get_highest_child_z_order()
  {
    if (m_zorder_map.size() > 0)
    {
      auto& iter = (m_zorder_map.cend())--;
      return iter->first;
    }
    return 0;
  }

  void Object::clear_children()
  {
    m_children.clear();
    m_zorder_map.clear();
  }

  sf::Vector2i Object::get_child_area_location()
  {
    return{ 0, 0 };
  }

  sf::Vector2i Object::get_child_area_size()
  {
    return get_size();
  }

  bool Object::render(sf::RenderTarget& target, int frame)
  {
    if (m_hidden == false)
    {
      sf::RenderTexture& texture = *(m_bg_texture.get());

      // Clear background texture.
      m_bg_texture->clear();

      /// Render self to our bg texture.
      render_self_before_children_(texture, frame);

      /// Render all child objects to our bg texture.
      for (auto& z_pair : m_zorder_map)
      {
        m_children.at(z_pair.second)->render(texture, frame);
      }

      /// Render self after children are done.
      render_self_after_children_(texture, frame);

      texture.display();

      // Create the RectangleShape that will be drawn onto the target.
      m_bg_shape.setPosition(sf::Vector2f(static_cast<float>(m_location.x), static_cast<float>(m_location.y)));
      m_bg_shape.setSize(sf::Vector2f(static_cast<float>(m_size.x), static_cast<float>(m_size.y)));
      m_bg_shape.setTexture(&(m_bg_texture->getTexture()));
      m_bg_shape.setTextureRect(sf::IntRect(0, 0, m_size.x, m_size.y));

      // Draw onto the target.
      target.setView(sf::View(sf::FloatRect(0.0f, 0.0f, static_cast<float>(target.getSize().x), static_cast<float>(target.getSize().y))));
      target.draw(m_bg_shape);

      return true;
    }
    else
    {
      return false;
    }
  }

  EventResult Object::handle_event(sf::Event & event)
  {
    EventResult result = EventResult::Ignored;

    if (m_enabled == true)
    {
      result = handle_event_before_children_(event);
      if (result != EventResult::Handled)
      {
        for (auto& z_pair : m_zorder_map)
        {
          result = m_children.at(z_pair.second)->handle_event(event);
          if (result == EventResult::Handled) break;
        }
      }

      if (result != EventResult::Handled)
      {
        result = handle_event_after_children_(event);
      }
    }

    return result;
  }

  void Object::set_parent(Object* parent)
  {
    m_parent = parent;
  }

  void Object::clear_child_focuses()
  {
    for (auto& child_pair : m_children)
    {
      child_pair.second->set_focus_only(false);
    }
  }

  void Object::set_focus_only(bool focus)
  {
    m_focus = focus;
  }

  void Object::render_self_before_children_(sf::RenderTexture& texture, int frame)
  {
  }

  void Object::render_self_after_children_(sf::RenderTexture& texture, int frame)
  {
  }

  EventResult Object::handle_event_before_children_(sf::Event& event)
  {
    return EventResult::Ignored;
  }

  EventResult Object::handle_event_after_children_(sf::Event& event)
  {
    return EventResult::Ignored;
  }
}; // end namespace metagui