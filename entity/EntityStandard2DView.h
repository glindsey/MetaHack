#pragma once

#include "stdafx.h"

#include <SFML/Graphics.hpp>

#include "entity/EntityView.h"
#include "systems/SystemLighting.h"

/// Class representing the standard 2D (pseudo-3D) view of a Entity object.
class EntityStandard2DView : public EntityView
{
  friend class Standard2DGraphicViews;

public:
  /// Destructor.
  virtual ~EntityStandard2DView();

  virtual void draw(sf::RenderTarget& target,
                    SystemLighting* lighting,
                    bool use_smoothing,
                    int frame) override;

  virtual std::string getViewName() override;

  /// Return the coordinates of the entity on the tilesheet.
  UintVec2 get_tile_sheet_coords(int frame) const;
  
protected:
  /// Constructor.
  explicit EntityStandard2DView(Entity& entity, 
                                TileSheet& tileSheet);

  virtual EventResult onEvent_NVI(Event const& event) override;

private:
  /// Reference to tilesheet storing entity graphics.
  TileSheet& m_tileSheet;
};