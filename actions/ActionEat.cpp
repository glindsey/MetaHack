#include "stdafx.h"

#include "ActionEat.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionEat ActionEat::prototype;
  ActionEat::ActionEat() : Action("eat", "EAT", ActionEat::create_) {}
  ActionEat::ActionEat(EntityId subject) : Action(subject, "eat", "EAT") {}
  ActionEat::~ActionEat() {}

  std::unordered_set<Trait> const & ActionEat::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::ObjectCanBeSelf
    };

    return traits;
  }

  StateResult ActionEat::doPreBeginWorkNVI(GameState& gameState, AnyMap& params)
  {
    std::string message;
    auto subject = getSubject();
    auto object = getObject();

    // Check that it isn't US!
    if (subject == object)
    {
      printMessageTry();

      /// @todo Handle "unusual" cases (e.g. zombies?)
      putTr("YOU_TRY_TO_EAT_YOURSELF_HUMOROUS");
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionEat::doBeginWorkNVI(GameState& gameState, AnyMap& params)
  {
    auto subject = getSubject();
    auto object = getObject();

    printMessageBegin();

    // Do the eating action here.
    /// @todo "Partially eaten" status for entities that were started to be eaten
    ///       but were interrupted.
    /// @todo Figure out eating time. This will obviously vary based on the
    ///       object being eaten.
    m_last_eat_result = object->beObjectOf(*this, subject);

    if (m_last_eat_result)
    {
      return StateResult::Success();
    }
    else
    {
      printMessageStop();
      return StateResult::Failure();
    }
  }

  StateResult ActionEat::doFinishWorkNVI(GameState& gameState, AnyMap& params)
  {
    auto object = getObject();

    printMessageFinish();

    if (m_last_eat_result)
    {
      object->destroy();
    }

    return StateResult::Success();
  }

  StateResult ActionEat::doAbortWorkNVI(GameState& gameState, AnyMap& params)
  {
    printMessageStop();
    return StateResult::Success();
  }
} // end namespace
