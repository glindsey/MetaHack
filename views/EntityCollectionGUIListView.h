#pragma once

#include "views/EntityCollectionGUIView.h"

/// Forward declarations

/// Class representing a list view of an entity's inventory.
class EntityCollectionGUIListView : public EntityCollectionGUIView
{

public:
  /// Constructor.
  explicit EntityCollectionGUIListView(sfg::SFGUI& sfgui,
                                       std::vector<EntityId> ids);

  /// Destructor.
  virtual ~EntityCollectionGUIListView();

protected:
  virtual bool onEvent(Event const& event) override;

private:
};
