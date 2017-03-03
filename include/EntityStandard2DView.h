#pragma once

#include "stdafx.h"

#include <SFML/Graphics.hpp>

#include "EntityView.h"

/// Class representing the standard 2D (pseudo-3D) view of a Entity object.
class EntityStandard2DView : public EntityView
{
public:
  /// Constructor.
  explicit EntityStandard2DView(Entity& thing);

  /// Destructor.
  virtual ~EntityStandard2DView();

  
protected:
  virtual void notifyOfEvent_(Observable& observed, Event event) override;

private:
};