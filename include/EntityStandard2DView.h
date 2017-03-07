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

  virtual void draw(sf::RenderTarget& target,
                    bool use_lighting,
                    bool use_smoothing,
                    int frame) override;

  virtual std::string get_view_name() override;

  /// Return the coordinates of the entity on the tilesheet.
  Vec2u get_tile_sheet_coords(int frame) const;
  
protected:
  /// Constructor.
  explicit EntityStandard2DView(Entity& entity, TileSheet& tile_sheet);

  virtual void notifyOfEvent_(Observable& observed, Event event) override;

private:
  /// Reference to tilesheet storing entity graphics.
  TileSheet& m_tile_sheet;
};