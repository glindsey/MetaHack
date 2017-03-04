#pragma once

#include "stdafx.h"

#include <SFML/Graphics.hpp>

#include "EntityView.h"

/// Class representing a null view of a Entity object.
class EntityNullView : public EntityView
{
  friend class NullGraphicViews;

public:
  /// Destructor.
  virtual ~EntityNullView();

  
protected:
  /// Constructor.
  explicit EntityNullView(Entity& thing);

  virtual void notifyOfEvent_(Observable& observed, Event event) override;

private:
};