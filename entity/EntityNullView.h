#pragma once

#include "stdafx.h"

#include <SFML/Graphics.hpp>

#include "entity/EntityView.h"

/// Class representing a null view of a Entity object.
class EntityNullView : public EntityView
{
  friend class NullGraphicViews;

public:
  /// Destructor.
  virtual ~EntityNullView();

  virtual void draw(sf::RenderTarget& target,
                    bool use_lighting,
                    bool use_smoothing,
                    int frame) override;

  virtual std::string getViewName() override;

protected:
  /// Constructor.
  explicit EntityNullView(Entity& entity);

private:
};