#include "stdafx.h"

#include "types/EntitySpecs.h"

void from_json(json const& j, EntitySpecs& obj)
{
  obj.category = "";
  obj.material = "";

  if (!j.is_object()) return;

  obj.category = j["category"].get<std::string>();
  if (j.count("material") > 0)
  {
    obj.material = j["material"].get<std::string>();
  }
}

void to_json(json& j, EntitySpecs const& obj)
{
  j = json::object();
  j["category"] = obj.category;
  if (!obj.material.empty())
  {
    j["material"] = obj.material;
  }
}
