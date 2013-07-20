#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include <memory>
#include <SFML/Graphics.hpp>

#include "Aggregate.h"
#include "Direction.h"

// Forward declarations
class Entity;

/// Interface class representing a thing that emits or absorbs light.
class LightSource : public Aggregate
{
  public:
    virtual ~LightSource();

    /// Returns the current strength of this light.
    /// Since we can deal with magical stuff, a negative value means this
    /// object actually *absorbs* light.
    int get_light_strength() const;

    /// Gets the level of this light as a fraction of total output from 0-256.
    int get_light_level() const;

    /// Sets the level of this light as a fraction of total output from 0-256.
    void set_light_level(int level);

    /// Returns the maximum strength of this light.
    /// Since we can deal with magical stuff, a negative value means this
    /// object actually *absorbs* light.
    int get_max_light_strength() const;

    /// Sets the maximum strength of this light.
    void set_max_light_strength(int maxStrength);

    /// Gets the direction this light is pointing.
    /// If the light is omnidirectional, returns Direction::Self.
    Direction get_light_direction() const;

    /// Sets the direction this light is pointing.
    /// If the light is omnidirectional, pass Direction::Self.
    void set_light_direction(Direction direction);

    /// Gets the color of this light.
    virtual sf::Color get_light_color() const;

    /// Sets the color of this light when it is at maximum brightness.
    /// Returns a boolean indicating whether setting was allowed.
    void set_light_color(sf::Color color);

    /// Return whether or not this light source can be turned on/off by this
    /// Entity.
    /// The default behavior for this is to return false.
    virtual bool can_be_activated_by(Entity const& entity) const override;

    /// Perform an action when this light source is turned on/off.
    /// If this function returns false, the action is aborted.
    /// The default behavior is to do nothing and return false.
    virtual bool do_action_activated_by(Entity& entity) override;

    /// Set whether this light source is currently lit.
    void set_lit(bool lit);

    /// Get whether this light source is currently lit.
    bool is_lit() const;

  protected:
    LightSource();
    LightSource(LightSource const& original);

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // LIGHTSOURCE_H
