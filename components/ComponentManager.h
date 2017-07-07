#pragma once

#include "components/ComponentMap.h"

#include "components/ComponentGlobals.h"
#include "components/ComponentActivity.h"
#include "components/ComponentAppearance.h"
#include "components/ComponentBodyparts.h"
#include "components/ComponentCombustible.h"
#include "components/ComponentCorrodible.h"
#include "components/ComponentDigestiveSystem.h"
#include "components/ComponentEquippable.h"
#include "components/ComponentGender.h"
#include "components/ComponentHealth.h"
#include "components/ComponentInventory.h"
#include "components/ComponentLightSource.h"
#include "components/ComponentLockable.h"
#include "components/ComponentMagicalBinding.h"
#include "components/ComponentMaterialFlags.h"
#include "components/ComponentMatterState.h"
#include "components/ComponentMobility.h"
#include "components/ComponentOpenable.h"
#include "components/ComponentPhysical.h"
#include "components/ComponentPosition.h"
#include "components/ComponentSapience.h"
#include "components/ComponentSenseSight.h"
#include "components/ComponentSpacialMemory.h"

// Forward declarations
class EntityId;
class GameState;

namespace Components
{

  class ComponentManager final
  {
  public:
    ComponentManager(GameState& gameState);
    ComponentManager(GameState& gameState, json const& j);
    ~ComponentManager();

    void initialize();

    void clone(EntityId original, EntityId newId);

    /// Erases an Entity completely from the component maps.
    void erase(EntityId id);

    void populate(EntityId newId, json const& jsonComponents);

    /// Dump ALL component data.
    json toJSON();

    /// Dump component data for a single ID.
    json toJSON(EntityId id);

    ComponentGlobals                               globals;
    ComponentMapConcrete<std::string>              category;
    ComponentMapConcrete<std::string>              material;

    ComponentMapConcrete<std::string>              properName;
    ComponentMapConcrete<std::string>              noun;
    // Adjective order in English (from the Cambridge Dictionary):
    //  #   type              examples
    // ===  ================= ========================================
    //  1   opinion           unusual, lovely, beautiful
    //  2   size              big, small, tall
    //  3   physical quality  thin, rough, untidy
    //  4   shape             round, square, rectangular
    //  5   age               young, old, youthful
    //  6   color             blue, red, pink
    //  7   origin            Dutch, Japanese, Turkish
    //  8   material          metal, wood, plastic
    //  9   type              general-purpose, four-sided, U-shaped
    //  10  purpose           cleaning, hammering, cooking
    ComponentMapConcrete<std::string>              adjMaterial;

    ComponentMapConcrete<ComponentActivity>        activity;
    ComponentMapConcrete<ComponentAppearance>      appearance;
    ComponentMapConcrete<ComponentBodyparts>       bodyparts;
    ComponentMapConcrete<ComponentCombustible>     combustible;
    ComponentMapConcrete<ComponentCorrodible>      corrodible;
    ComponentMapConcrete<ComponentDigestiveSystem> digestiveSystem;
    ComponentMapConcrete<ComponentEquippable>      equippable;
    ComponentMapConcrete<ComponentGender>          gender;
    ComponentMapConcrete<ComponentHealth>          health;
    ComponentMapConcrete<ComponentInventory>       inventory;
    ComponentMapConcrete<ComponentLightSource>     lightSource;
    ComponentMapConcrete<ComponentLockable>        lockable;
    ComponentMapConcrete<ComponentMagicalBinding>  magicalBinding;
    ComponentMapConcrete<ComponentMaterialFlags>   materialFlags;
    ComponentMapConcrete<ComponentMatterState>     matterState;
    ComponentMapConcrete<ComponentMobility>        mobility;
    ComponentMapConcrete<ComponentOpenable>        openable;
    ComponentMapConcrete<ComponentPhysical>        physical;
    ComponentMapConcrete<ComponentPosition>        position;
    ComponentMapConcrete<unsigned int>             quantity;
    ComponentMapConcrete<ComponentSapience>        sapience;
    ComponentMapConcrete<ComponentSenseSight>      senseSight;
    ComponentMapConcrete<ComponentSpacialMemory>   spacialMemory;

    /// Map of JSON component names to component references.
    std::map<std::string, ComponentMap&> const nameToComponent
    {
        { "category"           , category        },
        { "material"           , material        },
        { "position"           , position        },

        { "proper-name"        , properName      },
        { "noun"               , noun            },
        { "adjective-material" , adjMaterial     },

        { "activity"           , activity        },
        { "appearance"         , appearance      },
        { "bodyparts"          , bodyparts       },
        { "combustible"        , combustible     },
        { "corrodible"         , corrodible      },
        { "digestive-system"   , digestiveSystem },
        { "equippable"         , equippable      },
        { "gender"             , gender          },
        { "health"             , health          },
        { "inventory"          , inventory       },
        { "light-source"       , lightSource     },
        { "lockable"           , lockable        },
        { "magical-binding"    , magicalBinding  },
        { "material-flags"     , materialFlags   },
        { "matter-state"       , matterState     },
        { "mobility"           , mobility        },
        { "openable"           , openable        },
        { "physical"           , physical        },
        { "quantity"           , quantity        },
        { "sapience"           , sapience        },
        { "sense-sight"        , senseSight      },
        { "spacial-memory"     , spacialMemory   }
    };

    /// Map of component pointers to JSON component names.
    /// (Pointers because using a reference as a map key is... non-trivial.)
    /// Could combine this with above and use boost::bimap... but I don't like
    /// the boost::bimap syntax.
    std::map<ComponentMap*, std::string> const componentToName
    {
        { &category        , "category"           },
        { &material        , "material"           },
        { &position        , "position"           },

        { &properName      , "proper-name"        },
        { &noun            , "noun"               },
        { &adjMaterial     , "adjective-material" },

        { &activity        , "activity"           },
        { &appearance      , "appearance"         },
        { &bodyparts       , "bodyparts"          },
        { &combustible     , "combustible"        },
        { &corrodible      , "corrodible"         },
        { &digestiveSystem , "digestive-system"   },
        { &equippable      , "equippable"         },
        { &gender          , "gender"             },
        { &health          , "health"             },
        { &inventory       , "inventory"          },
        { &lightSource     , "light-source"       },
        { &lockable        , "lockable"           },
        { &magicalBinding  , "magical-binding"    },
        { &materialFlags   , "material-flags"     },
        { &matterState     , "matter-state"       },
        { &mobility        , "mobility"           },
        { &openable        , "openable"           },
        { &physical        , "physical"           },
        { &quantity        , "quantity"           },
        { &sapience        , "sapience"           },
        { &senseSight      , "sense-sight"        },
        { &spacialMemory   , "spacial-memory"     }
    };

  private:
    /// Reference to parent GameState instance.
    GameState& m_gameState;
  };

} // end namespace Components
