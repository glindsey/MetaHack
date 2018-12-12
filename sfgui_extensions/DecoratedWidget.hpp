#pragma once

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <SFGUI/RenderQueue.hpp>

class DecoratedWidget : public sfg::Widget
{
public:
  using Ptr = std::shared_ptr<DecoratedWidget>;            //!< Shared pointer.
  using PtrConst = std::shared_ptr<DecoratedWidget const>; //!< Shared pointer.

  virtual ~DecoratedWidget() {}

protected:
  DecoratedWidget() {}

};
