#include "stdafx.h"

#include "components/ComponentManager.h"

#include "game/GameState.h"

ComponentManager::ComponentManager()
{}

ComponentManager::~ComponentManager()
{}

void from_json(json const & j, ComponentManager & obj)
{
  obj.position = j["position"];
}

void to_json(json & j, ComponentManager const & obj)
{
  j["position"] = obj.position;
}
