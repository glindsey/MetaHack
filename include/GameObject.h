#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <boost/noncopyable.hpp>
#include <SFML/Graphics.hpp>
#include <string>

/// Everything in the game inherits from GameObject.
class GameObject
  :
  public boost::noncopyable
{
public:
  GameObject();
  virtual ~GameObject();

  /// Return this game object's description.
  virtual std::string get_display_name() const = 0;

  /// Add this game object to a VertexArray to be drawn.
  /// @param vertices Array to add vertices to.
  /// @param use_lighting If true, calculate lighting when adding.
  ///                     If false, store directly w/white bg color.
  /// @param frame Animation frame number.
  virtual void add_floor_vertices_to(sf::VertexArray& vertices,
                                     bool use_lighting,
                                     int frame) = 0;

  /// Draw this game object onto a RenderTexture, at the specified coordinates.
  /// @param target Texture to draw onto.
  /// @param target_coords Coordinates to draw the Thing at.
  /// @param target_size Target size of thing, in pixels.
  /// @param use_lighting If true, calculate lighting when adding.
  ///                     If false, store directly w/white bg color.
  /// @param frame Animation frame number.
  virtual void draw_to(sf::RenderTarget& target,
                       sf::Vector2f target_coords,
                       unsigned int target_size,
                       bool use_lighting,
                       int frame) = 0;

protected:
private:
};

#endif // GAMEOBJECT_H
