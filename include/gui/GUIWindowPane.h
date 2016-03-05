#ifndef GUIWINDOWPANE_H
#define GUIWINDOWPANE_H

#include "stdafx.h"

#include "gui/GUIPane.h"

namespace metagui
{
  class WindowPane :
    public Pane
  {
  public:
    WindowPane(std::string name, sf::Vector2i location, sf::Vector2i size = sf::Vector2i(0, 0));
    WindowPane(std::string name, sf::IntRect dimensions);
    virtual ~WindowPane();

    /// Choose whether this window has a title bar.
    void set_title_bar(bool enabled);

    /// Get whether this window has a title bar.
    bool get_title_bar();

    /// Choose whether this window is movable.
    /// @note   If a window has a title bar, you must click and drag the title
    ///         bar to move it. Otherwise, you may drag anywhere in the window
    ///         that isn't a child object to move it.
    void set_movable(bool enabled);

    /// Get whether this window is enabled.
    bool get_movable();

    /// Choose whether this window is resizable.
    void set_resizable(bool enabled);

    /// Get whether this window is resizable.
    bool get_resizable();

    /// Choose whether this window is closable.
    void set_closable(bool enabled);

    /// Get whether this window is closable.
    bool get_closable();

    /// Choose whether this window is shrinkable.
    void set_shrinkable(bool enabled);

    /// Get whether this window is shrinkable.
    bool get_shrinkable();

  protected:
    virtual void render_self_before_children_(sf::RenderTexture& texture, int frame) override final;

    /// Temporary function used to render the contents of classes derived from
    /// GUIWindowPane until they can be properly replaced with child controls.
    virtual void render_contents_(sf::RenderTexture& texture, int frame);

  private:
    /// Border shape.
    sf::RectangleShape m_border_shape;

    /// Flag indicating whether this window pane has a title bar.
    bool m_title_bar;

    /// Flag indicating whether this window is movable.
    bool m_movable;

    /// Flag indicating whether this window is resizable.
    bool m_resizable;

    /// Flag indicating whether this window is closable.
    bool m_closable;

    /// Flag indicating whether this window is shrinkable.
    bool m_shrinkable;

  };
}; // end namespace metagui

#endif // GUIWINDOWPANE_H
