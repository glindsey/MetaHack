#pragma once

#include "stdafx.h"

#include "components/ComponentMap.h"

// Forward declarations
class GameState;

class ComponentsManager final
{
public:
  ComponentsManager();
  ~ComponentsManager();

  friend void from_json(json const& j, ComponentsManager& obj);
  friend void to_json(json& j, ComponentsManager const& obj);

protected:

private:
  /// Testing component map
  ComponentMap<bool> m_testComponentMap;

};