#include "stdafx.h"

#include "gui/GUIObject.h"

#include <exception>

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

GUIObject::GUIObject(std::string name, sf::Vector2i location, sf::Vector2i size)
{
  set_relative_location(location);
  set_size(size);
}

GUIObject::GUIObject(std::string name, sf::IntRect dimensions)
{
  set_relative_dimensions(dimensions);
}

GUIObject::~GUIObject()
{
  //dtor
}

std::string GUIObject::get_name()
{
  return m_name;
}

void GUIObject::set_focus(bool focus)
{
  if (m_parent != nullptr)
  {
    m_parent->clear_child_focuses();
  }

  set_focus_only(focus);
}

bool GUIObject::get_focus()
{
  return m_focus;
}

bool GUIObject::get_global_focus()
{
  bool global_focus = m_focus;

  if ((global_focus == true) && (m_parent != nullptr))
  {
    global_focus &= m_parent->get_global_focus();
  }

  return global_focus;
}

void GUIObject::set_global_focus(bool focus)
{
  if (m_parent != nullptr)
  {
    m_parent->set_global_focus(focus);
  }

  set_focus(focus);
}

void GUIObject::set_hidden(bool hidden)
{
  m_hidden = hidden;
}

bool GUIObject::get_hidden()
{
  return m_hidden;
}

void GUIObject::set_enabled(bool enabled)
{
  m_enabled = enabled;
}

bool GUIObject::get_enabled()
{
  return m_enabled;
}

void GUIObject::set_text(std::string text)
{
  m_text = text;
}

std::string GUIObject::get_text()
{
  return m_text;
}

sf::Vector2i GUIObject::get_relative_location()
{
  return m_location;
}

void GUIObject::set_relative_location(sf::Vector2i location)
{
  m_location = location;
}

sf::Vector2i GUIObject::get_size()
{
  return m_size;
}

void GUIObject::set_size(sf::Vector2i size)
{
  m_size = size;
  m_bg_texture.reset(NEW sf::RenderTexture());
  m_bg_texture->create(size.x, size.y);
}

sf::IntRect GUIObject::get_relative_dimensions()
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

void GUIObject::set_relative_dimensions(sf::IntRect dimensions)
{
  set_relative_location({ dimensions.left, dimensions.top });
  set_size({ dimensions.width, dimensions.height });
}

sf::Vector2i GUIObject::get_absolute_location()
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

void GUIObject::set_absolute_location(sf::Vector2i location)
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

GUIObject& GUIObject::add_child(std::unique_ptr<GUIObject> child, uint32_t z_order)
{
  ASSERT_CONDITION(child);

  GUIObject& child_ref = *(child.get());
  child->set_parent(this);

  // This odd syntax is in order to work around VS compiler bug when having
  // a unique_ptr as a map value. See:
  // https://stackoverflow.com/questions/21056872/c-stdunique-ptr-wont-compile-in-map
  m_children.insert<ChildMap::value_type>(ChildMap::value_type(z_order, std::move(child)));
  return child_ref;
}

GUIObject& GUIObject::add_child(std::unique_ptr<GUIObject> child)
{
  uint32_t z_order = get_highest_child_z_order() + 1;
  return add_child(std::move(child), z_order);
}

GUIObject & GUIObject::add_child_top(std::unique_ptr<GUIObject> child)
{
  uint32_t z_order = get_lowest_child_z_order() - 1;
  return add_child(std::move(child), z_order);
}

bool GUIObject::child_exists(std::string name)
{
  for (auto& child_pair : m_children)
  {
    if ((child_pair.second)->get_name() == name)
    {
      return true;
    }
  }
  return false;
}

GUIObject& GUIObject::get_child(std::string name)
{
  for (auto& child_pair : m_children)
  {
    if ((child_pair.second)->get_name() == name)
    {
      return *(child_pair.second.get());
    }
  }

  std::string message = "Tried to get non-existent child \"" + name + "\" of GUI object \"" + get_name() + "\"";
  throw std::runtime_error(message.c_str());
}

uint32_t GUIObject::get_lowest_child_z_order()
{
  auto& iter = m_children.cbegin();

  return uint32_t();
}

uint32_t GUIObject::get_highest_child_z_order()
{
  auto& iter = (m_children.cend())--;

  return uint32_t();
}

void GUIObject::clear_children()
{
  m_children.clear();
}

sf::Vector2i GUIObject::get_child_area_location()
{
  return{ 0, 0 };
}

sf::Vector2i GUIObject::get_child_area_size()
{
  return get_size();
}

bool GUIObject::render(sf::RenderTarget& target, int frame)
{
  if (m_hidden == false)
  {
    sf::RenderTexture& texture = *(m_bg_texture.get());

    // Clear background texture.
    m_bg_texture->clear();

    /// Render self to our bg texture.
    render_self_before_children_(texture, frame);

    /// Render all child objects to our bg texture.
    for (auto& child_pair : m_children)
    {
      (child_pair.second)->render(texture, frame);
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

EventResult GUIObject::handle_event(sf::Event & event)
{
  EventResult result = EventResult::Ignored;

  if (m_enabled == true)
  {
    result = handle_event_before_children_(event);
    if (result != EventResult::Handled)
    {
      for (auto& child_pair : m_children)
      {
        result = (child_pair.second)->handle_event(event);
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

void GUIObject::set_parent(GUIObject* parent)
{
  m_parent = parent;
}

void GUIObject::clear_child_focuses()
{
  for (auto& child_pair : m_children)
  {
    child_pair.second->set_focus_only(false);
  }
}

void GUIObject::set_focus_only(bool focus)
{
  m_focus = focus;
}

void GUIObject::render_self_before_children_(sf::RenderTexture& texture, int frame)
{
}

void GUIObject::render_self_after_children_(sf::RenderTexture& texture, int frame)
{
}

EventResult GUIObject::handle_event_before_children_(sf::Event& event)
{
  return EventResult::Ignored;
}

EventResult GUIObject::handle_event_after_children_(sf::Event& event)
{
  return EventResult::Ignored;
}