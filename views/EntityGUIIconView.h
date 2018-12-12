#pragma once

#include "views/EntityGUIView.h"

/// Forward declarations

/// Class representing an icon view of an entity.
class EntityGUIIconView : public EntityGUIView
{

public:
  /// Constructor.
  explicit EntityGUIIconView(sfg::SFGUI& sfgui,
                                EntityId id);

  /// Destructor.
  virtual ~EntityGUIIconView();

protected:
  virtual bool onEvent(Event const& event) override;

private:
};
