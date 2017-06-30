#include "stdafx.h"
#include "MapMemoryChunk.h"

void from_json(json const & j, MapMemoryChunk & obj)
{
  auto& entities = j["entities"];
  if (entities.is_array())
  {
    obj.m_entitySpecs.clear();
    for (auto& entity : entities)
    {
      obj.m_entitySpecs.push_back(entity);
    }
  }

  obj.m_when = j["when"];
}

void to_json(json & j, MapMemoryChunk const & obj)
{
  j["entities"] = json::array();
  for (auto& entity : obj.m_entitySpecs)
  {
    j["entities"].push_back(entity);
  }

  j["when"] = obj.m_when;
}
