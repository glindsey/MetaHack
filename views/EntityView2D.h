#pragma once

#include <SFML/Graphics.hpp>

#include "tilesheet/TileSheet.h"
#include "views/EntityView.h"

/// Forward declarations

/// Class representing the standard 2D (pseudo-3D) view of a Entity object.
class EntityView2D : public EntityView
{
public:
  /// Constructor.
  explicit EntityView2D(EntityId entity);

  /// Destructor.
  virtual ~EntityView2D();

  virtual void draw(sf::RenderTarget& target,
                    Systems::Lighting* lighting,
                    bool use_smoothing,
                    int frame) override;

  virtual sf::RectangleShape drawRectangle(int frame) override;

  virtual std::string getViewName() override;

  /// Return the coordinates of the entity on the tilesheet.
  UintVec2 getTileSheetCoords(int frame) const;

protected:
  virtual bool onEvent(Event const& event) override;

private:
};
