#pragma once

#include "entity/EntityId.h"

#include "json.hpp"
using json = ::nlohmann::json;

/// Component that handles items being magically bound (can't be moved/dropped/unwielded/taken off/used).
/// For simplicity's sake, only one binding can be active on an entity at any given time (although the
/// binding may affect multiple actions).
/// But if I want to, this could easily be expanded to a vector of bindings.
class ComponentMagicalBinding
{

  /// Bit values for types of magical binding supported.
  class Against final
  {
  public:
    Against(Bits32 bits) : m_bits(bits) {}
    Bits32 operator()() const
    {
      return m_bits;
    }

    bool action(Bits32 value)
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

    static Bits32 const   Dropping = 0x00000001;
    static Bits32 const Unwielding = 0x00000002;
    static Bits32 const  Disrobing = 0x00000004;
    static Bits32 const      Using = 0x00000008;
    static Bits32 const     Moving = 0x00000010;
  
    friend std::ostream& operator<<(std::ostream& os, Against against)
    {
      os << "Against( ";
      Bits32 bits = against.m_bits;

      if (bits == 0) os << "nothing ";
      else
      {
        if (bits & Against::Dropping) os << "Dropping ";
        if (bits & Against::Unwielding) os << "Unwielding ";
        if (bits & Against::Disrobing) os << "Disrobing ";
        if (bits & Against::Using) os << "Using ";
        if (bits & Against::Moving) os << "Moving ";
      }

      os << ")";
      return os;
    }

  protected:

  private:
    Bits32 m_bits;

  };


public:
  ComponentMagicalBinding();

  friend void from_json(json const& j, ComponentMagicalBinding& obj);
  friend void to_json(json& j, ComponentMagicalBinding const& obj);

  EntityId binder() const;
  void setBinder(EntityId binder);

  Against& against();
  Against const& against() const;

  bool isActive() const;
  void activate();
  void deactivate();

  ElapsedTime autoExpirationTime() const;
  void setAutoExpirationTime(ElapsedTime time);

  ElapsedTime activeTimeRemaining() const;
  void setActiveTime(ElapsedTime time);

protected:

private:
  /// Entity responsible for this binding.
  EntityId m_binder;

  /// Actions this binding is against.
  Against m_against;

  /// Binding is currently active.
  bool m_active;

  /// When binding is activated, number of milliseconds it will be activated for, or 0 if it is indefinite.
  ElapsedTime m_autoExpirationTime;

  /// If active, milliseconds remaining in active time, or 0 if it never expires on its own.
  ElapsedTime m_activeTimeRemaining;
};
