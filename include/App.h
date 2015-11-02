#ifndef APP_H
#define APP_H

#define _CRTDBG_MAP_ALLOC

#ifdef _DEBUG
#ifndef NEW
#define NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif
#else
#define NEW new 
#endif  // _DEBUG


#include <boost/noncopyable.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <memory>
#include <SFML/Graphics.hpp>

#include "EventHandler.h"

// Forward declarations
class MessageLog;
class StateMachine;
class TileSheet;

class App : public EventHandler, public boost::noncopyable
{
  public:
    App();
    virtual ~App();

    void run();

    EventResult handle_event(sf::Event& event);

    bool has_window_focus();

    int get_rand(int minimum, int maximum);

  protected:
  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/// Global window instance
extern std::unique_ptr<sf::RenderWindow> app_window_;

/// Global default font instance
extern std::unique_ptr<sf::Font> default_font_;

/// Global default bold font instance
extern std::unique_ptr<sf::Font> default_bold_font_;

/// Global default monospace font instance
extern std::unique_ptr<sf::Font> default_mono_font_;

/// Global RNG instance
extern std::unique_ptr<boost::random::mt19937> rng_;

/// Global tile sheet instance
extern std::unique_ptr<TileSheet> tile_sheet_;

/// Global shader instance
extern std::unique_ptr<sf::Shader> shader_;

// Here are a few macros to save on typing.
#define the_window            (*(app_window_.get()))
#define the_default_font      (*(default_font_.get()))
#define the_default_bold_font (*(default_bold_font_.get()))
#define the_default_mono_font (*(default_mono_font_.get()))
#define the_shader            (*(shader_.get()))
#define the_RNG               (*(rng_.get()))

#endif // APP_H
