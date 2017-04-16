#include "stdafx.h"

#include "components/ComponentsManager.h"

#include "game/GameState.h"

ComponentsManager::ComponentsManager()
{}

ComponentsManager::~ComponentsManager()
{}

void from_json(json const & j, ComponentsManager & obj)
{
  obj.position = j["position"];
}

void to_json(json & j, ComponentsManager const & obj)
{
  j["position"] = obj.position;
}
