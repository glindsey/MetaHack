#include "types/Vec2.h"

void to_json(json& j, Vec2<float> const& obj)
{
  j = json::array({ "realvec2", obj.x, obj.y });
}

void to_json(json& j, Vec2<unsigned int> const& obj)
{
  j = json::array({ "uintvec2", obj.x, obj.y });
}

void to_json(json& j, Vec2<int> const& obj)
{
  j = json::array({ "intvec2", obj.x, obj.y });
}
