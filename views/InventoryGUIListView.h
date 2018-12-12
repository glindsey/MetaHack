#pragma once

#include "views/InventoryGUIView.h"

/// Forward declarations

/// Class representing a list view of an entity's inventory.
class InventoryGUIListView : public InventoryGUIView
{

public:
  /// Constructor.
  explicit InventoryGUIListView(sfg::SFGUI& sfgui,
                                EntityId id);

  /// Destructor.
  virtual ~InventoryGUIListView();

protected:
  virtual bool onEvent(Event const& event) override;

private:
};
