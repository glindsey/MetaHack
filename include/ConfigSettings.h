#ifndef CONFIGSETTINGS_H
#define CONFIGSETTINGS_H

#include <memory>
#include <SFML/Graphics.hpp>

/// Class that contains configuration settings for the entire game.  The class
/// is instantiated as a lazy-initialized singleton.
class ConfigSettings
{
  public:
    virtual ~ConfigSettings();

    /// Get the singleton instance of the class.
    /// If the instance does not yet exist, it is constructed and initialized.
    static ConfigSettings& instance();


    sf::Color window_border_color;          ///< Window border color.
    sf::Color window_focused_border_color;  ///< Focused window border color.
    sf::Color window_bg_color;              ///< Window background color.
    sf::Color text_shadow_color;            ///< Text shadow color.
    sf::Color text_color;                   ///< Text color.
    sf::Color text_highlight_color;         ///< Text highlight color.
    sf::Color cursor_border_color;          ///< Cursor border color.
    sf::Color cursor_bg_color;              ///< Cursor background color.
    float window_border_width;              ///< Window border width.
    float tile_highlight_border_width;      ///< Highlight border width.
    unsigned int inventory_area_width;      ///< Width of inventory areas.
    unsigned int status_area_height;        ///< Height of the status area.
    unsigned int map_tile_size;             ///< Size of a map tile, in pixels.

  protected:
  private:
    /// Constructor.  Private because this is a singleton.
    ConfigSettings();

    /// Unique pointer to singleton instance.
    static std::unique_ptr<ConfigSettings> instance_;
};

/// Shortcut to the singleton instance, to save on typing.
#define Settings ConfigSettings::instance()

#endif // CONFIGSETTINGS_H
