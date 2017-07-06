#pragma once

#include <SFML/Graphics.hpp>

#include "views/EntityView.h"

/// Class representing a null view of a Entity object.
class EntityNullView : public EntityView
{
  friend class NullGraphicViews;

public:
  /// Destructor.
  virtual ~EntityNullView();

  virtual void draw(sf::RenderTarget& target,
                    Systems::Lighting* lighting,
                    bool use_smoothing,
                    int frame) override;

  virtual std::string getViewName() override;

protected:
  /// Constructor.
  explicit EntityNullView(EntityId entity);

  virtual bool onEvent(Event const& event) override;

private:
};