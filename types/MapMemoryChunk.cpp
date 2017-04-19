#include "stdafx.h"
#include "MapMemoryChunk.h"

void from_json(json const & j, MapMemoryChunk & obj)
{
  obj.m_type = j["type"].get<std::string>();
  obj.m_when = j["when"];
}

void to_json(json & j, MapMemoryChunk const & obj)
{
  j["type"] = obj.m_type;
  j["when"] = obj.m_when;
}
