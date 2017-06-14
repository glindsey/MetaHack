#include "stdafx.h"

#include "types/common.h"

#include "Action.h"
#include "ActionMove.h"

#include "components/ComponentManager.h"
#include "entity/EntityId.h"
#include "game/GameState.h"
#include "services/IStrings.h"
#include "services/MessageLog.h"
#include "services/Service.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "systems/SystemGeometry.h"
#include "utilities/Shortcuts.h"
#include "utilities/StringTransforms.h"

namespace Actions
{
  std::unordered_map<std::string, ActionCreator> Action::s_actionMap;

  Action::Action(std::string type, ActionCreator creator)
    :
    m_state{ State::Pending },
    m_subject{ EntityId::Mu() },
    m_objects{},
    m_targetThing{ EntityId::Mu() },
    m_targetDirection{ Direction::None },
    m_quantity{ 1 },
    m_type{ type }
  {
    registerActionAs(type, creator);
  }

  json Action::makeJSONArgumentsObject() const
  {
    auto subject = getSubject();
    auto& objects = getObjects();
    auto target = getTargetThing();
    auto verb = getType();
    auto targetDirection = getTargetDirection();

    json result{};

    result["subject"] = subject;
    if (objects.size() > 0) result["object"] = objects;
    if (target != EntityId::Mu()) result["target"] = target;
    if (!verb.empty()) result["verb"] = verb;
    if (targetDirection != Direction::None) result["target-direction"] = targetDirection;

    return result;
  }

  Action::Action(EntityId subject, std::string type)
    :
    m_state{ State::Pending },
    m_subject{ subject },
    m_objects{},
    m_targetThing{ EntityId::Mu() },
    m_targetDirection{ Direction::None },
    m_quantity{ 1 },
    m_type{ type }
  {}

  Action::~Action()
  {}

  std::unordered_set<Trait> const& Action::getTraits() const
  {
    static std::unordered_set<Trait> traits = {};
    return traits;
  }

  bool Action::hasTrait(Trait trait) const
  {
    return (getTraits().count(trait) != 0);
  }

  EntityId Action::getSubject() const
  {
    return m_subject;
  }

  void Action::setObject(EntityId object)
  {
    m_objects.clear();
    m_objects.push_back(object);
  }

  void Action::setObjects(std::vector<EntityId> objects)
  {
    m_objects = objects;
  }

  std::vector<EntityId> const& Action::getObjects() const
  {
    return m_objects;
  }

  EntityId Action::getObject() const
  {
    return m_objects[0];
  }

  EntityId Action::getSecondObject() const
  {
    return m_objects[1];
  }

  bool Action::process(GameState& gameState, Systems::Manager& systems)
  {
    auto subject = getSubject();
    auto& components = gameState.components();
    auto& arguments = makeJSONArgumentsObject();

    if (!components.activity.existsFor(subject)) return false;

    auto& activity = components.activity[subject];

    // If entity is currently busy, decrement by one and return.
    if (activity.busyTicks() > 0)
    {
      CLOG(TRACE, "Action") << "Entity #" <<
        subject << " (" <<
        components.category[subject] << "): is busy, busyTicks = " << activity.busyTicks();

      activity.decBusyTicks(1);
      return false;
    }

    // Continue running through states until the event is processed, or the
    // target actor is busy.
    while ((m_state != State::Processed) && (activity.busyTicks() == 0))
    {
      StateResult result{ false, 0 };

      CLOG(TRACE, "Action") << "Entity #" <<
        subject << " (" <<
        components.category[subject] << "): Action " <<
        getType().c_str() << " is in state " <<
        str(getState());

      switch (m_state)
      {
        case State::Pending:
          result = doPreBeginWork(gameState, systems, arguments);

          if (result.success)
          {
            // Update the busy counter.
            activity.incBusyTicks(result.elapsed_time);
            setState(State::PreBegin);
          }
          else
          {
            // Clear the busy counter.
            activity.clearBusyTicks();
            setState(State::PostFinish);
          }
          break;

        case State::PreBegin:
          result = doBeginWork(gameState, systems, arguments);

          // If starting the action succeeded, move to the in-progress state.
          // Otherwise, just go right to post-finish.
          if (result.success)
          {
            // Update the busy counter.
            activity.incBusyTicks(result.elapsed_time);
            setState(State::InProgress);
          }
          else
          {
            // Clear the busy counter.
            activity.clearBusyTicks();
            setState(State::PostFinish);
          }
          break;

        case State::InProgress:
          result = doFinishWork(gameState, systems, arguments);

          activity.incBusyTicks(result.elapsed_time);
          setState(State::PostFinish);
          break;

        case State::Interrupted:
          result = doAbortWork(gameState, systems, arguments);

          activity.incBusyTicks(result.elapsed_time);
          setState(State::PostFinish);
          break;

        case State::PostFinish:
          setState(State::Processed);
          break;

        default:
          break;
      }
    }

    return (m_state == State::Processed);
  }

  void Action::setState(State state)
  {
    auto subject = getSubject();
    CLOG(TRACE, "Action") << "Entity #" <<
      subject << " (" <<
      COMPONENTS.category[subject] << "): Action " <<
      getType().c_str() << " switching to state " <<
      str(getState());

    m_state = state;
  }

  State Action::getState()
  {
    return m_state;
  }

  void Action::setTarget(EntityId entity)
  {
    m_targetThing = entity;
    m_targetDirection = Direction::None;
  }

  void Action::setTarget(Direction direction)
  {
    m_targetThing = EntityId::Mu();
    m_targetDirection = direction;
  }

  void Action::setQuantity(unsigned int quantity)
  {
    m_quantity = quantity;
  }

  EntityId Action::getTargetThing() const
  {
    return m_targetThing;
  }

  Direction Action::getTargetDirection() const
  {
    return m_targetDirection;
  }

  unsigned int Action::getQuantity() const
  {
    return m_quantity;
  }

  std::string Action::getType() const
  {
    return m_type;
  }

  StateResult Action::doPreBeginWork(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto& components = gameState.components();
    auto subject = getSubject();
    auto& objects = getObjects();
    auto& narrator = systems.narrator();
    bool hasPosition = components.position.existsFor(subject);
    auto new_direction = getTargetDirection();

    // Check that we're capable of performing this action at all.
    std::string canVerb{ "can-" + getType() };
    ReasonBool capable = subjectIsCapable(gameState);
    if (!capable.value)
    {
      printMessageTry(systems, arguments);

      if (!capable.reason.empty())
      {
        arguments["reason"] = capable.reason;
        putMsg(narrator.makeTr("YOU_ARE_NOT_CAPABLE_OF_VERBING_BECAUSE", arguments));
      }
      else
      {
        putMsg(narrator.makeTr("YOU_ARE_NOT_CAPABLE_OF_VERBING", arguments));
      }

      return StateResult::Failure();
    }

    // Check that we're capable of performing this action right now.
    ReasonBool capableNow = subjectIsCapableNow(gameState);
    if (!capableNow.value)
    {
      printMessageTry(systems, arguments);
      putMsg(narrator.makeTr("YOU_CANT_VERB_NOW", arguments));
      return StateResult::Failure();
    }

    if (hasTrait(Trait::SubjectMustBeAbleToMove))
    {
      // Make sure we can move RIGHT NOW.
      if (!components.mobility.existsFor(subject))
      {
        putMsg(narrator.makeTr("YOU_CANT_MOVE_NOW", arguments));
        return StateResult::Failure();
      }
    }

    if (!hasTrait(Trait::SubjectCanBeInLimbo))
    {
      // Make sure we're not in limbo!
      if (!hasPosition)
      {
        putMsg(narrator.makeTr("DONT_EXIST_PHYSICALLY", arguments));
        return StateResult::Failure();
      }
    }

    if (hasTrait(Trait::SubjectCanNotBeInsideAnotherObject))
    {
      // Make sure we're not confined inside another entity.
      /// @todo Allow for attacking when swallowed!
      auto& subjectPosition = components.position.of(subject);
      if (subjectPosition.isInsideAnotherEntity())
      {
        printMessageTry(systems, arguments);
        putMsg(narrator.makeTr("YOU_ARE_INSIDE_OBJECT", arguments));
        return StateResult::Failure();
      }
    }

    if (objects.size() > 0)
    {
      for (auto object : objects)
      {
        if (hasTrait(Trait::ObjectCanBeSelf))
        {
          if (subject == object)
          {
            // If object can be self, we bypass other checks and let the action
            // subclass handle it.
            /// @todo Not sure this is the best option... think more about this later.
            return doPreBeginWorkNVI(gameState, systems, arguments);
          }
        }
        else
        {
          if (subject == object)
          {
            if (!(components.globals.player() == subject))
            {
              putMsg(narrator.makeTr("YOU_TRY_TO_VERB_YOURSELF_INVALID", arguments));
              CLOG(WARNING, "Action") << "NPC tried to " << getType() << " self!?";
            }

            putMsg(narrator.makeTr("YOU_CANT_VERB_YOURSELF", arguments));
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustBeLiquidCarrier))
        {
          // Check that both are liquid containers.
          /// @todo Re-implement me. Do something better here.
          //if (!object->getBaseProperty("liquid_carrier", false))
          //{
          //  printMessageTry();
          //  putTr("THE_FOO_IS_NOT_A_LIQUID_CARRIER");
          //  return StateResult::Failure();
          //}
        }

        if (hasTrait(Trait::ObjectMustNotBeEmpty))
        {
          // Check that it is not empty.
          Components::ComponentInventory& inv = components.inventory[object];
          if (inv.count() == 0)
          {
            printMessageTry(systems, arguments);
            putMsg(narrator.makeTr("THE_FOO_IS_EMPTY", arguments));
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustBeEmpty))
        {
          // Check that it is not empty.
          Components::ComponentInventory& inv = components.inventory[object];
          if (inv.count() != 0)
          {
            printMessageTry(systems, arguments);
            putMsg(narrator.makeTr("THE_FOO_IS_NOT_EMPTY", arguments));
            return StateResult::Failure();
          }
        }

        if (!hasTrait(Trait::ObjectCanBeOutOfReach))
        {
          // Check that each object is within reach.
          if (!systems.geometry()->firstCanReachSecond(subject, object))
          {
            printMessageTry(systems, arguments);
            putMsg(narrator.makeTr("CONJUNCTION_HOWEVER", arguments) + " " + 
                   narrator.makeTr("FOO_PRO_SUB_IS_OUT_OF_REACH", arguments));
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustBeInInventory))
        {
          // Check that each object is in our inventory.
          if (!components.inventory[subject].contains(object))
          {
            printMessageTry(systems, arguments);
            auto message = 
              narrator.makeTr("CONJUNCTION_HOWEVER", arguments) + " " + 
              narrator.makeTr("FOO_PRO_SUB_IS_NOT_IN_YOUR_INVENTORY", arguments);
            if (systems.geometry()->firstCanReachSecond(subject, object))
            {
              message += narrator.makeTr("PICK_UP_OBJECT_FIRST", arguments);
            }
            message += ".";
            putMsg(message);
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustNotBeInInventory))
        {
          // Check if it's already in our inventory.
          if (components.inventory[subject].contains(object))
          {
            printMessageTry(systems, arguments);
            putMsg(narrator.makeTr("THE_FOO_IS_ALREADY_IN_YOUR_INVENTORY", arguments));
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustBeWielded))
        {
          // Check to see if the object is being wielded.
          if (components.bodyparts.existsFor(subject) &&
              components.bodyparts[subject].getWieldedLocation(object).part == BodyPart::Nowhere)
          {
            printMessageTry(systems, arguments);
            putMsg(narrator.makeTr("THE_FOO_MUST_BE_WIELDED", arguments));
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustBeWorn))
        {
          // Check to see if the object is being worn.
          if (components.bodyparts.existsFor(subject) &&
              components.bodyparts[subject].getWornLocation(object).part == BodyPart::Nowhere)
          {
            printMessageTry(systems, arguments);
            putMsg(narrator.makeTr("THE_FOO_MUST_BE_WORN", arguments));
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustNotBeWielded))
        {
          // Check to see if the object is being wielded.
          if (components.bodyparts.existsFor(subject) &&
              components.bodyparts[subject].getWieldedLocation(object).part != BodyPart::Nowhere)
          {
            printMessageTry(systems, arguments);

            /// @todo Perhaps automatically try to unwield the item before dropping?
            putMsg(narrator.makeTr("YOU_CANT_VERB_WIELDED", arguments));
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustNotBeWorn))
        {
          // Check to see if the object is being worn.
          if (components.bodyparts.existsFor(subject) &&
              components.bodyparts[subject].getWornLocation(object).part != BodyPart::Nowhere)
          {
            printMessageTry(systems, arguments);
            putMsg(narrator.makeTr("YOU_CANT_VERB_WORN", arguments));
            return StateResult::Failure();
          }
        }

        // Check that we can perform this Action on this object.
        ReasonBool allowedNow = objectIsAllowedNow(gameState);
        if (!allowedNow.value)
        {
          printMessageTry(systems, arguments);
          printMessageCant(systems, arguments); ///< @todo Add the reason why here
          return StateResult::Failure();
        }
      }
    }

    auto result = doPreBeginWorkNVI(gameState, systems, arguments);
    return result;
  }

  StateResult Action::doBeginWork(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto result = doBeginWorkNVI(gameState, systems, arguments);
    return result;
  }

  StateResult Action::doFinishWork(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto result = doFinishWorkNVI(gameState, systems, arguments);
    return result;
  }

  StateResult Action::doAbortWork(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto result = doAbortWorkNVI(gameState, systems, arguments);
    return result;
  }

  ReasonBool Action::subjectIsCapable(GameState const& gameState) const
  {
    std::string reason = "Missing subjectIsCapable() implementation for action \"" + m_type + "\"";
    CLOG(ERROR, "Action") << reason;
    return { false, reason };
  }

  ReasonBool Action::subjectIsCapableNow(GameState const& gameState) const
  {
    return subjectIsCapable(gameState);
  }

  ReasonBool Action::objectIsAllowed(GameState const& gameState) const
  {
    std::string reason = "Missing objectIsAllowed() implementation for action \"" + m_type + "\"";
    CLOG(ERROR, "Action") << reason;
    return { false, reason };
  }

  ReasonBool Action::objectIsAllowedNow(GameState const& gameState) const
  {
    return objectIsAllowed(gameState);
  }

  StateResult Action::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    /// @todo Set counter_busy based on the action being taken and
    ///       the entity's reflexes.
    return StateResult::Success();
  }

  StateResult Action::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    putMsg(tr("ACTN_NOT_IMPLEMENTED"));
    return StateResult::Failure();
  }

  StateResult Action::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    /// @todo Complete the action here
    return StateResult::Success();
  }

  StateResult Action::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    /// @todo Handle aborting the action here.
    return StateResult::Success();
  }

  void Action::printMessageTry(Systems::Manager& systems, json& arguments) const
  {
    auto& objects = getObjects();
    auto& narrator = systems.narrator();

    if (objects.size() == 0)
    {
      putMsg(narrator.makeTr("YOU_TRY_TO_VERB", arguments));
    }
    else if (objects.size() == 1)
    {
      putMsg(narrator.makeTr("YOU_TRY_TO_VERB_THE_FOO", arguments));
    }
    else
    {

    }
  }

  void Action::printMessageDo(Systems::Manager& systems, json& arguments) const
  {
    auto& objects = getObjects();
    auto& narrator = systems.narrator();

    if (objects.size() == 0)
    {
      putMsg(narrator.makeTr("YOU_CVERB", arguments));
    }
    else if (objects.size() == 1)
    {
      putMsg(narrator.makeTr("YOU_CVERB_THE_FOO", arguments));
    }
    else
    {
      putMsg(narrator.makeTr("YOU_CVERB_THE_ITEMS", arguments));
    }
  }

  void Action::printMessageBegin(Systems::Manager& systems, json& arguments) const
  {
    auto& objects = getObjects();
    auto& narrator = systems.narrator();

    if (objects.size() == 0)
    {
      putMsg(narrator.makeTr("YOU_BEGIN_TO_VERB", arguments));
    }
    else if (objects.size() == 1)
    {
      putMsg(narrator.makeTr("YOU_BEGIN_TO_VERB_THE_FOO", arguments));
    }
    else
    {
      putMsg(narrator.makeTr("YOU_BEGIN_TO_VERB_THE_ITEMS", arguments));
    }
  }

  void Action::printMessageStop(Systems::Manager& systems, json& arguments) const
  {
    auto& objects = getObjects();
    auto& narrator = systems.narrator();

    if (objects.size() == 0)
    {
      putMsg(narrator.makeTr("YOU_STOP_VERBING", arguments));
    }
    else if (objects.size() == 1)
    {
      putMsg(narrator.makeTr("YOU_STOP_VERBING_THE_FOO", arguments));
    }
    else
    {
      putMsg(narrator.makeTr("YOU_STOP_VERBING_THE_ITEMS", arguments));
    }
  }

  void Action::printMessageFinish(Systems::Manager& systems, json& arguments) const
  {
    auto& objects = getObjects();
    auto& narrator = systems.narrator();

    if (objects.size() == 0)
    {
      putMsg(narrator.makeTr("YOU_FINISH_VERBING", arguments));
    }
    else if (objects.size() == 1)
    {
      putMsg(narrator.makeTr("YOU_FINISH_VERBING_THE_FOO", arguments));
    }
    else
    {
      putMsg(narrator.makeTr("YOU_FINISH_VERBING_THE_ITEMS", arguments));
    }
  }

  void Action::printMessageCant(Systems::Manager& systems, json& arguments) const
  {
    auto& objects = getObjects();
    auto& narrator = systems.narrator();

    if (objects.size() == 0)
    {
      putMsg(narrator.makeTr("YOU_CANT_VERB", arguments));
    }
    else if (objects.size() == 1)
    {
      putMsg(narrator.makeTr("YOU_CANT_VERB_THAT", arguments));
    }
    else
    {
      putMsg(narrator.makeTr("YOU_CANT_VERB_THOSE", arguments));
    }
  }

  void Action::registerActionAs(std::string key, ActionCreator creator)
  {
    Action::s_actionMap.insert({ key, creator });
  }

  bool Action::exists(std::string key)
  {
    return Action::s_actionMap.count(key) != 0;
  }

  std::unique_ptr<Action> Action::create(std::string key, 
                                         EntityId subject,
                                         std::vector<EntityId> objects,
                                         EntityId targetThing,
                                         Direction targetDirection,
                                         unsigned int quantity)
  {
    if (Action::s_actionMap.count(key) != 0)
    {
      std::unique_ptr<Action> action = (Action::s_actionMap.at(key))(subject);
      if (objects.size() != 0) action->setObjects(objects);
      if (targetThing != EntityId::Mu()) action->setTarget(targetThing);
      if (targetDirection != Direction::None) action->setTarget(targetDirection);
      if (quantity != 0) action->setQuantity(quantity);

      return std::move(action);
    }
    else
    {
      throw std::runtime_error("Requested non-existent action " + key);
    }
  }

  ActionMap const & Action::getMap()
  {
    return Action::s_actionMap;
  }

} // end namespace

