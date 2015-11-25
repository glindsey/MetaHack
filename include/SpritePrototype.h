#ifndef SPRITEPROTOTYPE_H
#define SPRITEPROTOTYPE_H

#include <string>
#include <SFML/Graphics.hpp>

/// Ways that a sprite might animate.
/// Disregarded if rendersPerFrame is set to zero.
enum class SpriteAnimationStyle
{
  LoopAll,          /// Sprite loops through all animation frames
  PingPongAll,      /// Sprite goes back and forth through all animation frames
  IdlePlusLoop,     /// Frame 0 is idle image, all other frames loop
  IdlePlusPingPong, /// Frame 0 is idle image, all other frames loop
  Loop3,            /// Every 3 frames loop (used for multiple animations)
  Loop4,            /// Every 4 frames loop
  Count
};

/// Definition of a single sprite prototype in the sheet.
struct SpritePrototype
{
  SpritePrototype(sf::Vector2u spriteCoords_ = sf::Vector2u(0, 0),
                  unsigned int numAnimFrames_ = 1,
                  SpriteAnimationStyle animationStyle_ =
                  SpriteAnimationsStyle::IdlePlusLoop,
                  unsigned int numDirections_ = 1,
                  unsigned int rendersPerFrame_ = 1)
    : spriteCoords(spriteCoords_),
    numAnimFrames(numAnimFrames_),
    animationStyle(animationStyle_),
    numDirections(numDirections_),
    rendersPerFrame(rendersPerFrame_) {}

  /// Coordinates of the upper-left tile for the sprite.
  unsigned sf::Vector2u spriteCoords;

  /// Number of animation frames composing the sprite.
  unsigned int numAnimFrames;

  /// Animation style the sprite uses.
  SpriteAnimationStyle animationStyle;

  /// Number of directions.
  /// This number defines how the sprite's direction affects its display:
  ///   1 - Sprite is directionless
  ///   2 - Sprite has a N/S facing image, and an E/W facing image.
  ///   4 - Sprite has N, E, S, and W facing images.
  ///   8 - Sprite has N, NE, E, SE, S, SW, W, and NW facing images.
  unsigned int numDirections;

  /// Sprite animation speed (in screen renders).
  /// An animation speed of 0 means the sprite frame is changed manually.
  unsigned int rendersPerFrame;
};

class SpritePrototype
{
public:
  SpritePrototype() {}
  virtual ~SpritePrototype() {}
protected:
private:
};

#endif // SPRITEPROTOTYPE_H
