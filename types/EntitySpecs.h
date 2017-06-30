#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

/// Class containing the category and optional material for an entity.
class EntitySpecs
{
public:
  EntitySpecs() :
    category{ "" },
    material{ "" }
  {}

  EntitySpecs(std::string category_) :
    category{ category_ },
    material{ "" }
  {}

  EntitySpecs(std::string category_, std::string material_) :
    category{ category_ },
    material{ material_ }
  {}

  friend void from_json(json const& j, EntitySpecs& obj);
  friend void to_json(json& j, EntitySpecs const& obj);

  std::string category;
  std::string material;
};
