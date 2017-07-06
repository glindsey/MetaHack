#pragma once

#include <SFML/Graphics.hpp>

#include "tilesheet/TileSheet.h"
#include "views/EntityView.h"

/// Forward declarations

/// Class representing the pseudo-3D ASCII view of a Entity object.
class EntityFancyAsciiView : public EntityView
{
  friend class FancyAsciiGraphicViews;

public:
  /// Destructor.
  virtual ~EntityFancyAsciiView();

  virtual void draw(sf::RenderTarget& target,
                    Systems::Lighting* lighting,
                    bool use_smoothing,
                    int frame) override;

  virtual std::string getViewName() override;

  /// Return the coordinates of the entity on the tilesheet.
  UintVec2 getTileSheetCoords(int frame) const;
  
protected:
  /// Constructor.
  explicit EntityFancyAsciiView(EntityId entity,
                                FancyAsciiGraphicViews& views);

  virtual bool onEvent(Event const& event) override;

private:
  /// Reference to parent views object.
  FancyAsciiGraphicViews& m_views;
};