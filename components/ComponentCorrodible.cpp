#include "components/ComponentCorrodible.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

namespace Components
{

  void from_json(json const& j, ComponentCorrodible& obj)
  {
    obj = ComponentCorrodible();
    JSONUtils::doIfPresent(j, "corrodes-forever", [&](auto& value) { obj.m_corrodesForever = value; });
    JSONUtils::doIfPresent(j, "corrosion-amount", [&](auto& value) { obj.m_corrosionAmount = value; });
    JSONUtils::doIfPresent(j, "corrosion-speed", [&](auto& value) { obj.m_corrosionSpeed = value; });
    JSONUtils::doIfPresent(j, "corrosion-results", [&](auto& value)
    {
      if (value.is_array())
      {
        obj.m_corrosionResults.clear();
        for (auto citer = value.cbegin(); citer != value.cend(); ++citer)
        {
          obj.m_corrosionResults.push_back(value.get<std::string>());
        }
      }
    });
  }

  void to_json(json& j, ComponentCorrodible const& obj)
  {
    j = json::object();
    j["corrodes-forever"] = obj.m_corrodesForever;
    j["corrosion-amount"] = obj.m_corrosionAmount;
    j["corrosion-speed"] = obj.m_corrosionSpeed;
    json results = json::array();
    for (auto& item : obj.m_corrosionResults)
    {
      results.push_back(item);
    }
    j["corrosion-results"] = results;
  }

  ModifiableBool& ComponentCorrodible::corrodesForever()
  {
    return m_corrodesForever;
  }

  ModifiableBool const& ComponentCorrodible::corrodesForever() const
  {
    return m_corrodesForever;
  }

  int ComponentCorrodible::corrosionAmount() const
  {
    return m_corrosionAmount;
  }

  void ComponentCorrodible::setCorrosionAmount(int burnAmount)
  {
    m_corrosionAmount = burnAmount;
  }

  void ComponentCorrodible::incCorrosionAmount(int incAmount)
  {
    m_corrosionAmount += incAmount;
  }

  void ComponentCorrodible::decCorrosionAmount(int decAmount)
  {
    incCorrosionAmount(-decAmount);
  }

  ModifiableInt& ComponentCorrodible::corrosionSpeed()
  {
    return m_corrosionSpeed;
  }

  ModifiableInt const& ComponentCorrodible::corrosionSpeed() const
  {
    return m_corrosionSpeed;
  }

  std::vector<std::string> const& ComponentCorrodible::corrosionResults() const
  {
    return m_corrosionResults;
  }

} // end namespace
