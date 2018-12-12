#pragma once

#include "entity/EntityId.h"

#include "types/common.h"

namespace Components
{
  namespace Modifiers
  {
    /// Superclass for all modifiers in the game.
    class Base
    {
    public:
      Base() {}
      virtual ~Base() {}

    protected:

    private:
      /// ID of the entity responsible for this modifier.
      EntityId m_responsibleEntity;
    };
  } // end namespace Modifiers

} // end namespace Components
