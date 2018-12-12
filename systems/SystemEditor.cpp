#include "stdafx.h"

#include "systems/SystemEditor.h"

namespace Systems
{

  Editor::Editor(Components::ComponentGlobals& globals,
                 Components::ModifiersGroup& modifiers) :
    CRTP<Editor>({}),
    m_globals{ globals },
    m_modifiers{ modifiers }
  {}

  Editor::~Editor()
  {}

  void Editor::doCycleUpdate()
  {}

  void Editor::setMap_V(MapID newMap)
  {}

  bool Editor::onEvent(Event const & event)
  {
    return false;
  }

} // end namespace Systems
