#pragma once

#include "DecoratedWidget.hpp"

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <SFGUI/RenderQueue.hpp>

#include <memory>

template <class ChildWidget>
class Decorated : public DecoratedWidget
{
public:
  using Ptr = std::shared_ptr<Decorated<ChildWidget>>;            //!< Shared pointer.
  using PtrConst = std::shared_ptr<Decorated<ChildWidget>>;       //!< Shared pointer.

  // Every widget should have a Create() method.
  // It can take any parameters you want, but must return a Ptr.
  static Ptr Create(std::shared_ptr<ChildWidget> child)
  {
    return Ptr(new Decorated(child));
  }

  // Every widget must have a GetName() method
  // that returns the name of the widget type.
  const std::string& GetName() const override {
    static const std::string name( "Decorated<" );
    return name + m_child->GetName() + ">";
  }

  virtual ~Decorated() {}

protected:
  Decorated(std::shared_ptr<ChildWidget> child) :
    m_child{ child }
  {

  }

private:
  /// Child widget.
  std::shared_ptr<ChildWidget> m_child;
};
