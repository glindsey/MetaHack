#include "components/ComponentGender.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, ComponentGender& obj)
{
  obj = ComponentGender();
  JSONUtils::doIfPresent(j, "gender", [&](auto& value) { obj.m_gender = value; });
}

void to_json(json& j, ComponentGender const& obj)
{
  j = json::object();
  j["gender"] = obj.m_gender;
}

Gender & ComponentGender::gender()
{
  return m_gender;
}

Gender const & ComponentGender::gender() const
{
  return m_gender;
}
