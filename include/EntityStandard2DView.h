#pragma once

#include "stdafx.h"

#include <SFML/Graphics.hpp>

#include "EntityView.h"

/// Class representing the standard 2D (pseudo-3D) view of a Entity object.
class EntityStandard2DView : public EntityView
{
  friend class Standard2DGraphicViews;

public:
  /// Destructor.
  virtual ~EntityStandard2DView();

  
protected:
  /// Constructor.
  explicit EntityStandard2DView(Entity& thing);

  virtual void notifyOfEvent_(Observable& observed, Event event) override;

private:
};