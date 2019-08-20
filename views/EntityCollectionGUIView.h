#pragma once

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

#include "components/ComponentInventory.h"
#include "inventory/InventorySelection.h"
#include "Object.h"

// Forward declarations

/// Abstract class representing a view of a single entity's inventory.
/// The view provides an SFGUI Table widget that can be attached to a window.
class EntityCollectionGUIView
  :
  public Object
{
  /// @todo FINISH ME
  struct EventSelectionChanged : public ConcreteEvent<EventSelectionChanged>
  {
    EventSelectionChanged(/* stick event details here */)
      /* : stick member assignments here */
    {}

    // stick event detail members here

    void printToStream(std::ostream& os) const
    {
      Event::printToStream(os);
      os << " | other stuff";
    }
  };

public:
  virtual ~EntityCollectionGUIView();

  // Get the SFGUI table widget that this view created.
  sfg::Table::Ptr getWidget() const;

protected:
  /// Constructor.
  explicit EntityCollectionGUIView(sfg::SFGUI& sfgui, std::vector<EntityId> const ids);

  // Set the SFGUI table widget (called by child classes).
  void setWidget(sfg::Table::Ptr widget);

  /// Get collection of Entity IDs associated with this view.
  std::vector<EntityId> const& getEntityIds() const;

  /// Reference to the SFGUI API.
  sfg::SFGUI& m_sfgui;

private:
  /// Table widget that this view creates.
  sfg::Table::Ptr m_widget;

  /// Collection of Entity IDs associated with this view.
  std::vector<EntityId> const m_entityIds;
};
