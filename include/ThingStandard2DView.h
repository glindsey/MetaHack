#pragma once

#include "stdafx.h"

#include <SFML/Graphics.hpp>

#include "ThingView.h"

/// Class representing the standard 2D (pseudo-3D) view of a Thing object.
class ThingStandard2DView : public ThingView
{
public:
  /// Constructor.
  explicit ThingStandard2DView(Thing& thing);

  /// Destructor.
  virtual ~ThingStandard2DView();

  
protected:
  virtual void notifyOfEvent_(Observable& observed, Event event) override;

private:
};