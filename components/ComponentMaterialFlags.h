#pragma once

#include "types/common.h"
#include "utilities/StringTransforms.h"

#include "json.hpp"
using json = ::nlohmann::json;

namespace Components
{

  /// Component that indicates what material class(es) this entity is made of.
  /// For example: animal, plant, fish, insect, fungus, wood, rock, metal, detritus, etc.
  /// Used for handling what entities with digestive systems can eat/drink.
  class ComponentMaterialFlags
  {
  public:

    bool material(Bits32 value) const;

    void set(Bits32 value);

    void clear(Bits32 value);

    static Bits32 const    Unknown = 0x00000000;
    static Bits32 const       Meat = 0x00000001;
    static Bits32 const       Fish = 0x00000002;
    static Bits32 const      Dairy = 0x00000004;
    static Bits32 const        Egg = 0x00000008;
    static Bits32 const     Insect = 0x00000010;

    static Bits32 const      Blood = 0x00000100;
    static Bits32 const   Detritus = 0x00000200;
    static Bits32 const    Carrion = 0x00000400;

    static Bits32 const      Fruit = 0x00001000;
    static Bits32 const      Grass = 0x00002000;
    static Bits32 const      Grain = 0x00004000;
    static Bits32 const     Fungus = 0x00008000;
    static Bits32 const      Plant = 0x0000f000;

    static Bits32 const       Rock = 0x00100000;
    static Bits32 const       Wood = 0x00200000;
    static Bits32 const      Metal = 0x00400000;

    static Bits32 const    Sapient = 0x80000000;

    static Bits32 const   Anything = 0xffffffff;

    std::string toString() const;

    void setFromString(std::string input);

    friend std::ostream& operator<<(std::ostream& os, Components::ComponentMaterialFlags flags)
    {
      os << "MaterialFlags( " << flags.toString() << ")";
      return os;
    }

    friend void from_json(json const& j, ComponentMaterialFlags& obj);

    friend void to_json(json& j, ComponentMaterialFlags const& obj);

  protected:

  private:
    Bits32 m_bits;
  };

} // end namespace Components
