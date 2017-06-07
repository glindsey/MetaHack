#include "components/ComponentMaterialFlags.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

namespace Components
{
  void from_json(json const & j, ComponentMaterialFlags & obj)
  {
    obj.setFromString(j.get<std::string>());
  }

  void to_json(json& j, ComponentMaterialFlags const& obj)
  {
    j = obj.toString();
  }

  bool ComponentMaterialFlags::material(Bits32 value) const
  {
    return (m_bits & value) != 0;
  }

  void ComponentMaterialFlags::set(Bits32 value)
  {
    m_bits |= value;
  }

  void ComponentMaterialFlags::clear(Bits32 value)
  {
    m_bits &= ~value;
  }

  std::string ComponentMaterialFlags::toString() const
  {
    std::string output;

    if (m_bits == Unknown) return "";
    if (m_bits == Anything)
    {
      output = "anything";
    }
    else
    {
      if (m_bits & Meat) output += "meat ";
      if (m_bits & Fish) output += "fish ";
      if (m_bits & Dairy) output += "dairy ";
      if (m_bits & Egg) output += "egg ";
      if (m_bits & Insect) output += "insect ";

      if (m_bits & Blood) output += "blood ";
      if (m_bits & Detritus) output += "detritus ";
      if (m_bits & Carrion) output += "carrion ";

      if (m_bits & Plant) output += "plant ";
      else
      {
        if (m_bits & Fruit) output += "fruit ";
        if (m_bits & Grass) output += "grass ";
        if (m_bits & Grain) output += "grain ";
        if (m_bits & Fungus) output += "fungus ";
      }

      if (m_bits & Rock) output += "rock ";
      if (m_bits & Wood) output += "wood ";
      if (m_bits & Metal) output += "metal ";

      if (m_bits & Sapient) output += "sapient ";
    }

    return StringTransforms::squishWhitespace(output);
  }

  void ComponentMaterialFlags::setFromString(std::string input)
  {
    m_bits = 0;
    input = StringTransforms::squishWhitespace(input);
    boost::to_lower(input);

    if (input.empty()) return;
    if (input.find("anything") != std::string::npos)
    {
      m_bits = Anything;
      return;
    }

    m_bits |= (input.find("meat") != std::string::npos) ? Meat : 0;
    m_bits |= (input.find("fish") != std::string::npos) ? Fish : 0;
    m_bits |= (input.find("dairy") != std::string::npos) ? Dairy : 0;
    m_bits |= (input.find("egg") != std::string::npos) ? Egg : 0;
    m_bits |= (input.find("insect") != std::string::npos) ? Insect : 0;

    m_bits |= (input.find("blood") != std::string::npos) ? Blood : 0;
    m_bits |= (input.find("detritus") != std::string::npos) ? Detritus : 0;
    m_bits |= (input.find("carrion") != std::string::npos) ? Carrion : 0;

    m_bits |= (input.find("fruit") != std::string::npos) ? Fruit : 0;
    m_bits |= (input.find("grass") != std::string::npos) ? Grass : 0;
    m_bits |= (input.find("grain") != std::string::npos) ? Grain : 0;
    m_bits |= (input.find("fungus") != std::string::npos) ? Fungus : 0;
    m_bits |= (input.find("plant") != std::string::npos) ? Plant : 0;

    m_bits |= (input.find("rock") != std::string::npos) ? Rock : 0;
    m_bits |= (input.find("wood") != std::string::npos) ? Wood : 0;
    m_bits |= (input.find("metal") != std::string::npos) ? Metal : 0;

    m_bits |= (input.find("sapient") != std::string::npos) ? Sapient : 0;
  }

} // end namespace
