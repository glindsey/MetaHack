#pragma once

#include <boost/algorithm/string.hpp>

#include "entity/EntityId.h"
#include "utilities/StringTransforms.h"

#include "json.hpp"
using json = ::nlohmann::json;

namespace Components
{

  /// Component that handles items being magically bound (can't be moved/dropped/unwielded/taken off/used).
  /// For simplicity's sake, only one binding can be active on an entity at any given time (although the
  /// binding may affect multiple actions).
  /// But if I want to, this could easily be expanded to a vector of bindings.
  class ComponentMagicalBinding
  {
  public:
    /// Bit values for types of magical binding supported.
    class Against final
    {
    public:
      Against() : m_bits{ 0 } {}
      Against(Bits32 bits) : m_bits{ bits } {}

      Bits32 operator()() const
      {
        return m_bits;
      }

      bool action(Bits32 value) const
      {
        return (m_bits & value) != 0;
      }

      void set(Bits32 value)
      {
        m_bits |= value;
      }

      void clear(Bits32 value)
      {
        m_bits &= ~value;
      }

      static Bits32 const    Nothing = 0x00000000;
      static Bits32 const   Dropping = 0x00000001;
      static Bits32 const Unwielding = 0x00000002;
      static Bits32 const  Disrobing = 0x00000004;
      static Bits32 const      Using = 0x00000008;
      static Bits32 const     Moving = 0x00000010;
      static Bits32 const   Anything = 0xffffffff;
      static Bits32 const Everything = 0xffffffff;

      std::string toString() const
      {
        std::string output;

        if (m_bits == Against::Nothing) output += "nothing ";
        else if (m_bits == Against::Everything) output += "everything ";
        else
        {
          if (m_bits & Against::Dropping) output += "drop ";
          if (m_bits & Against::Unwielding) output += "unwield ";
          if (m_bits & Against::Disrobing) output += "disrobe ";
          if (m_bits & Against::Using) output += "use ";
          if (m_bits & Against::Moving) output += "move ";
        }

        return StringTransforms::squishWhitespace(output);
      }

      void setFromString(std::string input)
      {
        m_bits = 0;
        input = StringTransforms::squishWhitespace(input);
        boost::to_lower(input);

        if (input.find("nothing") != std::string::npos)
        {
          return;
        }
        if (input.find("everything") != std::string::npos)
        {
          m_bits = Against::Everything;
          return;
        }

        m_bits |= (input.find("drop") != std::string::npos) ? Dropping : 0;
        m_bits |= (input.find("unwield") != std::string::npos) ? Unwielding : 0;
        m_bits |= (input.find("disrobe") != std::string::npos) ? Disrobing : 0;
        m_bits |= (input.find("use") != std::string::npos) ? Using : 0;
        m_bits |= (input.find("move") != std::string::npos) ? Moving : 0;
      }

      friend std::ostream& operator<<(std::ostream& os, Against against)
      {
        os << "Against( " << against.toString() << ")";
        return os;
      }

      friend void from_json(json const& j, Against& obj)
      {
        obj.setFromString(j.get<std::string>());
      }

      friend void to_json(json& j, Against const& obj)
      {
        j = obj.toString();
      }

    protected:

    private:
      Bits32 m_bits;

    };

    ComponentMagicalBinding();

    friend void from_json(json const& j, ComponentMagicalBinding& obj);
    friend void to_json(json& j, ComponentMagicalBinding const& obj);

    EntityId binder() const;
    void setBinder(EntityId binder);

    Against& against();
    Against const& against() const;
    bool isAgainst(Bits32 value) const;

    bool isActive() const;
    void activate();
    void deactivate();

    ElapsedTicks autoExpirationTime() const;
    void setAutoExpirationTime(ElapsedTicks time);

    ElapsedTicks activeTimeRemaining() const;
    void setActiveTime(ElapsedTicks time);

  protected:

  private:
    /// Entity responsible for this binding.
    EntityId m_binder;

    /// Actions this binding is against.
    Against m_against;

    /// Binding is currently active.
    bool m_active;

    /// When binding is activated, number of milliseconds it will be activated for, or 0 if it is indefinite.
    ElapsedTicks m_autoExpirationTime;

    /// If active, milliseconds remaining in active time, or 0 if it never expires on its own.
    ElapsedTicks m_activeTimeRemaining;
  };

} // end namespace Components
