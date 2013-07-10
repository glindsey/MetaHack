#ifndef TILESHEET_H
#define TILESHEET_H

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

/// Sheet containing all tiles in the application.
class TileSheet
{
  public:
    TileSheet();
    virtual ~TileSheet();

    /// Load a sprite sheet from disk.
    bool load(std::string const& filename);

    /// Get a particular tile from the sheet.
    /// @warning Assumes tile is within the bounds of the loaded texture,
    ///          unless DEBUG is defined, in order to save execution time.
    sf::IntRect get_tile(sf::Vector2u tile) const;

    /// Get a reference to the texture to render.
    sf::Texture const& getTexture(void) const;

    static int const TileSize;
  protected:
  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // TILESHEET_H
