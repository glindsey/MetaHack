#include "sfgui_extensions/EntryPlus.hpp"
#include <SFGUI/Context.hpp>
#include <SFGUI/RenderQueue.hpp>
#include <SFGUI/Engine.hpp>

#include <SFML/Graphics/Font.hpp>
#include <cmath>

namespace sfg {

// Signals.
Signal::SignalID EntryPlus::OnTextChanged = 0;

EntryPlus::EntryPlus() :
  Entry()
{
}

EntryPlus::Ptr EntryPlus::Create( const sf::String& text ) {
  Ptr ptr( new EntryPlus );
  ptr->SetText( text );
  return ptr;
}

sf::Keyboard::Key EntryPlus::GetLastPressedKey() {
  return m_lastKeyPressed;
}

void EntryPlus::HandleKeyEvent( sf::Keyboard::Key key, bool press ) {
  // Save the key pressed into a member variable for retrieval.
  if (press) {
    m_lastKeyPressed = key;
  }

  // call super
  Entry::HandleKeyEvent(key, press);
}

const std::string& EntryPlus::GetName() const {
  static const std::string name( "EntryPlus" );
  return name;
}

}
