#pragma once

#include <SFML/Graphics.hpp>

#include "tilesheet/TileSheet.h"
#include "views/EntityView.h"
#include "views/Standard2DGraphicViews.h"

/// Forward declarations

/// Class representing the standard 2D (pseudo-3D) view of a Entity object.
class EntityStandard2DView : public EntityView
{
  friend class Standard2DGraphicViews;

public:
  /// Destructor.
  virtual ~EntityStandard2DView();

  virtual void draw(sf::RenderTarget& target,
                    Systems::Lighting* lighting,
                    bool use_smoothing,
                    int frame) override;

  virtual std::string getViewName() override;

  /// Return the coordinates of the entity on the tilesheet.
  UintVec2 getTileSheetCoords(int frame) const;
  
protected:
  /// Constructor.
  explicit EntityStandard2DView(EntityId entity, 
                                Standard2DGraphicViews& views);

  virtual bool onEvent(Event const& event) override;

private:
  /// Reference to parent views object.
  Standard2DGraphicViews& m_views;
};
