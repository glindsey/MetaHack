#pragma once

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

#include "entity/EntityId.h"
#include "Object.h"

/// Abstract class representing a view of a single Entity object.
/// The view provides an SFGUI widget that can be attached to another widget.
class EntityGUIView
  :
  public Object
{
public:
  virtual ~EntityGUIView();

  // Get the SFGUI widget that this view created.
  sfg::Widget::Ptr getWidget() const;

protected:
  /// Constructor.
  explicit EntityGUIView(sfg::SFGUI& sfgui, EntityId entity);

  // Set the SFGUI widget (called by child classes).
  void setWidget(sfg::Widget::Ptr widget);

  /// Get ID of Entity associated with this view.
  EntityId getEntity() const;

  /// Reference to the SFGUI API.
  sfg::SFGUI& m_sfgui;

private:
  /// Widget that this view creates.
  sfg::Widget::Ptr m_widget;

  /// ID of Entity associated with this view.
  EntityId m_entityId;
};
