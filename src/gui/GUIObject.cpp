#include "stdafx.h"

#include "gui/GUIObject.h"

#include <exception>

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

namespace metagui
{
  Object::Object(StringKey name, sf::Vector2i location, sf::Vector2u size)
  {
    SET_UP_LOGGER("GUI", true);

    m_name = name;
    set_relative_location(location);
    set_size(size);
  }

  Object::Object(StringKey name, sf::IntRect dimensions)
  {
    m_name = name;
    set_relative_dimensions(dimensions);
  }

  Object::~Object()
  {
    //dtor
  }

  StringKey Object::get_name()
  {
    return m_name;
  }

  void Object::set_pre_child_render_functor(RenderFunctor functor)
  {
    m_pre_child_render_functor = functor;
  }

  void Object::clear_pre_child_render_functor()
  {
    m_pre_child_render_functor = RenderFunctor();
  }

  void Object::set_post_child_render_functor(RenderFunctor functor)
  {
    m_post_child_render_functor = functor;
  }

  void Object::clear_post_child_render_functor()
  {
    m_post_child_render_functor = RenderFunctor();
  }

  void Object::set_focus(bool focus)
  {
    if ((m_disabled_cached == false) || (m_hidden_cached == false))
    {
      if (m_parent != nullptr)
      {
        m_parent->clear_child_focuses();
      }

      set_focus_only(focus);
    }
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
    if ((m_disabled_cached == false) || (m_hidden_cached == false))
    {
      if (m_parent != nullptr)
      {
        m_parent->set_global_focus(focus);
      }

      set_focus(focus);
    }
  }

  void Object::set_text(StringDisplay text)
  {
    m_text = text;
  }

  StringDisplay Object::get_text()
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

  sf::Vector2u Object::get_size()
  {
    return m_size;
  }

  void Object::set_size(sf::Vector2u size)
  {
    m_size = size;

    // Would be better not to create a texture if size is 0 in either dimension
    // but for the moment this is faster.
    /// @todo See if we can make this more elegant.
    if (size.x < 1) size.x = 1;
    if (size.y < 1) size.y = 1;

    if (m_parent != nullptr)
    {
      auto max_size = ((m_decor_cached == true) ?
                       m_parent->get_size() :
                       m_parent->get_child_area_size());

      if (size.x > max_size.x) size.x = max_size.x;
      if (size.y > max_size.y) size.y = max_size.y;
    }

    m_bg_texture.reset(NEW sf::RenderTexture());
    m_bg_texture->create(size.x, size.y);

    // Inform children of the parent size change.
    for (auto& child : m_children)
    {
      child.second->handle_parent_size_changed_(size);
    }
  }

  sf::IntRect Object::get_relative_dimensions()
  {
    sf::IntRect dimensions;
    sf::Vector2i location = get_relative_location();
    sf::Vector2u size = get_size();
    dimensions.left = location.x;
    dimensions.top = location.y;
    dimensions.width = size.x;
    dimensions.height = size.y;
    return dimensions;
  }

  void Object::set_relative_dimensions(sf::IntRect dimensions)
  {
    set_relative_location({ dimensions.left, dimensions.top });
    set_size({ static_cast<unsigned int>(dimensions.width),
               static_cast<unsigned int>(dimensions.height) });
  }

  sf::Vector2i Object::get_absolute_location()
  {
    sf::Vector2i absolute_location = get_relative_location();

    if (m_parent != nullptr)
    {
      sf::Vector2i child_area_absolute_location =
        m_parent->get_absolute_location();

      if (m_decor_cached != true)
      {
        child_area_absolute_location += m_parent->get_child_area_location();
      }

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
        m_parent->get_absolute_location();

      if (m_decor_cached != true)
      {
        child_area_absolute_location += m_parent->get_child_area_location();
      }

      relative_location -= child_area_absolute_location;
    }

    set_relative_location(relative_location);
  }

  Object& Object::add_child(std::unique_ptr<Object> child, uint32_t z_order)
  {
    ASSERT_CONDITION(child);

    StringKey name = child->get_name();

    if (child_exists(name))
    {
      throw std::runtime_error("Tried to add already-present child \"" + name + "\" of GUI object \"" + get_name() + "\"");
    }

    Object& child_ref = *(child.get());
    child->set_parent(this);

    // This odd syntax is in order to work around VS compiler bug when having
    // a unique_ptr as a map value. See:
    // https://stackoverflow.com/questions/21056872/c-stdunique-ptr-wont-compile-in-map
    m_children.insert<ChildMap::value_type>(ChildMap::value_type(name, std::move(child)));
    m_zorder_map.insert({ z_order, name });

    child_ref.handle_parent_size_changed_(get_size());

    CLOG(TRACE, "GUI") << "Added child \"" << name <<
      "\" (with Z-order " << z_order <<
      ") to parent \"" << get_name() << "\"";

    return child_ref;
  }

  Object& Object::add_child(std::unique_ptr<Object> child)
  {
    uint32_t z_order = get_highest_child_z_order() + 1;
    return add_child(std::move(child), z_order);
  }

  Object & Object::add_child_top(std::unique_ptr<Object> child)
  {
    uint32_t z_order = get_lowest_child_z_order() - 1;
    return add_child(std::move(child), z_order);
  }

  bool Object::child_exists(StringKey name)
  {
    return (m_children.count(name) > 0);
  }

  Object& Object::get_child(StringKey name)
  {
    if (child_exists(name))
    {
      return *(m_children.at(name).get());
    }

    throw std::runtime_error("Tried to get non-existent child \"" + name + "\" of GUI object \"" + get_name() + "\"");
  }

  std::unique_ptr<Object> Object::remove_child(StringKey name)
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

  sf::Vector2u Object::get_child_area_size()
  {
    return get_size();
  }

  bool Object::render(sf::RenderTexture& parent_texture, int frame)
  {
    sf::RenderTexture& our_texture = *(m_bg_texture.get());

    our_texture.clear(sf::Color::Blue);

    if (m_hidden_cached == false)
    {
      /// Render self to our bg texture.
      render_self_before_children_(our_texture, frame);

      /// If a pre-child render functor is present, call that.
      if (m_pre_child_render_functor)
      {
        m_pre_child_render_functor(our_texture, frame);
      }

      /// Render all child objects to our bg texture.
      for (auto& z_pair : m_zorder_map)
      {
        m_children.at(z_pair.second)->render(our_texture, frame);
      }

      /// If a post-child render functor is present, call that.
      if (m_post_child_render_functor)
      {
        m_post_child_render_functor(our_texture, frame);
      }

      /// Render self after children are done.
      render_self_after_children_(our_texture, frame);

      our_texture.display();

      // Create the RectangleShape that will be drawn onto the target.
      m_bg_shape.setPosition(sf::Vector2f(static_cast<float>(m_location.x), static_cast<float>(m_location.y)));
      m_bg_shape.setSize(sf::Vector2f(static_cast<float>(m_size.x), static_cast<float>(m_size.y)));
      m_bg_shape.setTexture(&(m_bg_texture->getTexture()));
      m_bg_shape.setTextureRect(sf::IntRect(0, 0, m_size.x, m_size.y));

      // Draw onto the parent.
      parent_texture.setView(sf::View(sf::FloatRect(0.0f, 0.0f, static_cast<float>(parent_texture.getSize().x), static_cast<float>(parent_texture.getSize().y))));
      parent_texture.draw(m_bg_shape);

      parent_texture.display();
      return true;
    }
    else
    {
      return false;
    }
  }

  void Object::set_flag(StringKey name, bool value)
  {
    if ((m_flags.count(name) == 0) || (m_flags[name] != value))
    {
      m_flags[name] = value;
      handle_set_flag(name, value);
    }
  }

  bool Object::get_flag(StringKey name, bool default_value)
  {
    if (m_flags.count(name) == 0)
    {
      set_flag(name, default_value);
    }
    return m_flags[name];
  }

  void Object::handle_set_flag(StringKey name, bool value)
  {
    if (name == "hidden")
    {
      if (value == true)
      {
        set_focus(false);
      }
      m_hidden_cached = value;
    }
    else if (name == "disabled")
    {
      if (value == true)
      {
        set_focus(false);
      }
      m_disabled_cached = value;
    }
    else if (name == "draggable")
    {
      m_draggable_cached = value;
    }
    else if (name == "decor")
    {
      m_decor_cached = value;
    }

    handle_set_flag_(name, value);
  }

  bool Object::contains_point(sf::Vector2i point)
  {
    auto left = get_absolute_location().x;
    auto top = get_absolute_location().y;
    auto right = left + (static_cast<int>(get_size().x) - 1);
    auto bottom = top + (static_cast<int>(get_size().y) - 1);

    return ((point.x >= left) && (point.x <= right) &&
            (point.y >= top) && (point.y <= bottom));
  }

  bool Object::contains_mouse()
  {
    return m_contains_mouse;
  }

  Event::Result Object::handle_event_before_children(EventDragFinished& event)
  {
    return handle_event_before_children_(event);
  }

  Event::Result Object::handle_event_after_children(EventDragFinished& event)
  {
    return handle_event_after_children_(event);
  }

  Event::Result Object::handle_event_before_children(EventDragging& event)
  {
    if (contains_point(event.start_location))
    {
      // We "Acknowledge" the event so it is passed to children.
      CLOG(TRACE, "GUI") << "Point is within object, passing to children";
      return Event::Result::Acknowledged;
    }
    else
    {
      // We "Discard" the event so it is not passed to children.
      CLOG(TRACE, "GUI") << "Point is not within object, discarding";
      return Event::Result::Discarded;
    }
  }

  Event::Result Object::handle_event_after_children(EventDragging& event)
  {
    // If we got here, all children ignored the event (or there are no
    // children) so we want to process it if we are draggable.
    if (m_draggable_cached == true)
    {
      CLOG(TRACE, "GUI") << "m_draggable_cached is true, dragging";

      /// @todo THIS IS WRONG. Move amount should be current location minus last location,
      ///       or new_coords should be drag start location + move_amount.
      auto move_amount = event.current_location - event.start_location;
      auto new_coords = m_location_last_mousedown + move_amount;

      set_absolute_location(new_coords);
      return Event::Result::Handled;
    }
    else
    {
      CLOG(TRACE, "GUI") << "m_draggable_cached is false, ignoring";
      return Event::Result::Ignored;
    }
  }

  Event::Result Object::handle_event_before_children(EventKeyPressed& event)
  {
    return handle_event_before_children_(event);
  }

  Event::Result Object::handle_event_after_children(EventKeyPressed& event)
  {
    return handle_event_after_children_(event);
  }

  Event::Result Object::handle_event_before_children(EventMouseDown& event)
  {
    m_location_last_mousedown = m_location;
    return handle_event_before_children_(event);
  }

  Event::Result Object::handle_event_after_children(EventMouseDown& event)
  {
    return handle_event_after_children_(event);
  }

  Event::Result Object::handle_event_before_children(EventResized& event)
  {
    return handle_event_before_children_(event);
  }

  Event::Result Object::handle_event_after_children(EventResized& event)
  {
    return handle_event_after_children_(event);
  }

  Object * Object::get_parent()
  {
    return m_parent;
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

  void Object::set_contains_mouse(bool contains)
  {
    if (m_contains_mouse != contains)
    {
      CLOG(TRACE, "GUI") << "Mouse has " << (contains ? "entered " : "left ") <<
        "the object " << get_name();
    }
    m_contains_mouse = contains;
  }

  void Object::render_self_before_children_(sf::RenderTexture& texture, int frame)
  {
  }

  void Object::render_self_after_children_(sf::RenderTexture& texture, int frame)
  {
  }

#if 0
  Event::Result Object::handle_event_before_children_(Event& event)
  {
    CLOG(TRACE, "GUI") << "Default Object handle_event_before_children_() called on event";
    return Event::Result::Ignored;
  }

  Event::Result Object::handle_event_after_children_(Event& event)
  {
    return Event::Result::Ignored;
  }
#endif

  Event::Result Object::handle_event_before_children_(EventDragFinished& event)
  {
    return Event::Result::Ignored;
  }

  Event::Result Object::handle_event_after_children_(EventDragFinished& event)
  {
    return Event::Result::Ignored;
  }

  Event::Result Object::handle_event_before_children_(EventDragging& event)
  {
    return Event::Result::Ignored;
  }

  Event::Result Object::handle_event_after_children_(EventDragging& event)
  {
    return Event::Result::Ignored;
  }

  Event::Result Object::handle_event_before_children_(EventKeyPressed& event)
  {
    return Event::Result::Ignored;
  }

  Event::Result Object::handle_event_after_children_(EventKeyPressed& event)
  {
    return Event::Result::Ignored;
  }

  Event::Result Object::handle_event_before_children_(EventMouseDown& event)
  {
    return Event::Result::Ignored;
  }

  Event::Result Object::handle_event_after_children_(EventMouseDown& event)
  {
    return Event::Result::Ignored;
  }

  Event::Result Object::handle_event_before_children_(EventResized& event)
  {
    return Event::Result::Ignored;
  }

  Event::Result Object::handle_event_after_children_(EventResized& event)
  {
    return Event::Result::Ignored;
  }

  void Object::handle_set_flag_(StringKey name, bool enabled)
  {}

  void Object::handle_parent_size_changed_(sf::Vector2u parent_size)
  {
  }
}; // end namespace metagui