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
  m_focus = focus;
}

bool GUIObject::get_focus()
{
  return m_focus;
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
    sf::Vector2i parents_absolute_location = m_parent->get_absolute_location();
    absolute_location += parents_absolute_location;
  }

  return absolute_location;
}

void GUIObject::set_absolute_location(sf::Vector2i location)
{
  sf::Vector2i relative_location = location;

  if (m_parent != nullptr)
  {
    sf::Vector2i parents_absolute_location = m_parent->get_absolute_location();
    relative_location -= parents_absolute_location;
  }

  set_relative_location(relative_location);
}

bool GUIObject::add_child(std::unique_ptr<GUIObject> child)
{
  ASSERT_CONDITION(child);

  if (std::find_if(m_children.cbegin(),
                   m_children.cend(),
                   [&](std::unique_ptr<GUIObject> const& c) { return c.get() == child.get(); }) == m_children.cend())
  {
    child->set_parent(this);
    m_children.push_back(std::move(child));
    return true;
  }
  return false;
}

bool GUIObject::child_exists(std::string name)
{
  if (std::find_if(m_children.cbegin(),
                   m_children.cend(),
                   [&](std::unique_ptr<GUIObject> const& c) { return c->get_name() == name; }) == m_children.cend())
  {
    return true;
  }
  return false;
}

GUIObject& GUIObject::get_child(std::string name)
{
  auto& iter = std::find_if(m_children.cbegin(),
                            m_children.cend(),
                            [&](std::unique_ptr<GUIObject> const& c) { return c->get_name() == name; });

  if (iter == m_children.cend())
  {
    std::string message = "Tried to get non-existent child \"" + name + "\" of GUI object \"" + get_name() + "\"";
    throw std::runtime_error(message.c_str());
  }

  return *(iter->get());
}

void GUIObject::clear_children()
{
  m_children.clear();
}

bool GUIObject::render(sf::RenderTarget& target, int frame)
{
  sf::RenderTexture& texture = *(m_bg_texture.get());

  // Clear background texture.
  m_bg_texture->clear();

  /// Render self to our bg texture.
  /* bool success = */ _render_self(texture, frame);

  /// Render all child objects to our bg texture.
  for (auto& child : m_children)
  {
    child->render(texture, frame);
  }

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

void GUIObject::set_parent(GUIObject* parent)
{
  m_parent = parent;
}