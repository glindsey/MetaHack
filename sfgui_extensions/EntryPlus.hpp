#pragma once

#include <SFGUI/Entry.hpp>

#include <SFML/System/String.hpp>
#include <memory>

namespace sfg {

/** EntryPlus widget -- standard Entry widget with some extra functionality
 */
class EntryPlus : public Entry {
  public:
    typedef std::shared_ptr<EntryPlus> Ptr; //!< Shared pointer.
    typedef std::shared_ptr<const EntryPlus> PtrConst; //!< Shared pointer.

    /** Create entry.
     * @param text Text.
     * @return EntryPlus.
     */
    static Ptr Create( const sf::String& text = L"" );

    sf::Keyboard::Key GetLastPressedKey();

    const std::string& GetName() const override;

    // Signals.
    static Signal::SignalID OnTextChanged; //!< Fired when the text changes.

  protected:
    /** Ctor.
     */
    EntryPlus();

    void HandleKeyEvent( sf::Keyboard::Key key, bool press ) override;

  private:
    sf::Keyboard::Key m_lastKeyPressed;
};

}
