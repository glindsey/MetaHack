#include "stdafx.h"

#include "types/common.h"

#include "Action.h"
#include "ActionMove.h"

#include "components/ComponentManager.h"
#include "entity/Entity.h"
#include "entity/EntityPool.h"
#include "entity/EntityId.h"
#include "game/GameState.h"
#include "services/IStringDictionary.h"
#include "services/MessageLog.h"
#include "Service.h"
#include "systems/SystemManager.h"
#include "systems/SystemSpacialRelationships.h"
#include "utilities/StringTransforms.h"

namespace Actions
{
  std::unordered_map<std::string, ActionCreator> Action::s_actionMap;

  Action::Action(std::string type, std::string verb, ActionCreator creator)
    :
    m_state{ State::Pending },
    m_subject{ EntityId::Mu() },
    m_objects{},
    m_targetThing{ EntityId::Mu() },
    m_targetDirection{ Direction::None },
    m_quantity{ 1 },
    m_type{ type },
    m_verb{ verb }
  {
    registerActionAs(type, creator);
  }

  Action::Action(EntityId subject, std::string type, std::string verb)
    :
    m_state{ State::Pending },
    m_subject{ subject },
    m_objects{},
    m_targetThing{ EntityId::Mu() },
    m_targetDirection{ Direction::None },
    m_quantity{ 1 },
    m_type{ type },
    m_verb{ verb }
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

  bool Action::process(GameState& gameState, SystemManager& systems)
  {
    auto subject = getSubject();
    auto& components = gameState.components();

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
          result = doPreBeginWork(gameState, systems);

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
          result = doBeginWork(gameState, systems);

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
          result = doFinishWork(gameState, systems);

          activity.incBusyTicks(result.elapsed_time);
          setState(State::PostFinish);
          break;

        case State::Interrupted:
          result = doAbortWork(gameState, systems);

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

    StateResult Action::doPreBeginWork(GameState& gameState, SystemManager& systems)
  {
    auto& components = gameState.components();
    auto subject = getSubject();
    auto& objects = getObjects();
    bool hasPosition = components.position.existsFor(subject);
    auto new_direction = getTargetDirection();

    // Check that we're capable of performing this action at all.
    std::string canVerb{ "can-" + getType() };
    ReasonBool capable = subjectIsCapable(gameState);
    if (!capable.value)
    {
      printMessageTry();
      if (!capable.reason.empty())
      {
        /// @todo Add translation key
        putMsg(makeTr("YOU_ARE_NOT_CAPABLE_OF_VERBING_BECAUSE",
        {
          getIndefArt(subject->getDisplayName()),
          subject->getDisplayName(),
          capable.reason
        }));
      }
      else
      {
        putMsg(makeTr("YOU_ARE_NOT_CAPABLE_OF_VERBING",
        {
          getIndefArt(subject->getDisplayName()),
          subject->getDisplayName()
        }));
      }

      return StateResult::Failure();
    }

    // Check that we're capable of performing this action right now.
    ReasonBool capableNow = subjectIsCapableNow(gameState);
    if (!capableNow.value)
    {
      printMessageTry();
      putMsg(makeTr("YOU_CANT_VERB_NOW", { getIndefArt(subject->getDisplayName()), subject->getDisplayName() }));
      return StateResult::Failure();
    }

    if (hasTrait(Trait::SubjectMustBeAbleToMove))
    {
      // Make sure we can move RIGHT NOW.
      if (!components.mobility.existsFor(subject))
      {
        putTr("YOU_CANT_MOVE_NOW");
        return StateResult::Failure();
      }
    }

    if (!hasTrait(Trait::SubjectCanBeInLimbo))
    {
      // Make sure we're not in limbo!
      if (!hasPosition)
      {
        putTr("DONT_EXIST_PHYSICALLY");
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
        auto subjectParent = subjectPosition.parent();
        printMessageTry();
        putMsg(makeTr("YOU_ARE_INSIDE_OBJECT",
        { subjectParent->getDescriptiveString(ArticleChoice::Indefinite) }));
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
            return doPreBeginWorkNVI(gameState, systems);
          }
        }
        else
        {
          if (subject == object)
          {
            if (!(components.globals.player() == subject))
            {
              putTr("YOU_TRY_TO_VERB_YOURSELF_INVALID");
              CLOG(WARNING, "Action") << "NPC tried to " << getType() << " self!?";
            }

            putTr("YOU_CANT_VERB_YOURSELF");
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
          ComponentInventory& inv = components.inventory[object];
          if (inv.count() == 0)
          {
            printMessageTry();
            putTr("THE_FOO_IS_EMPTY");
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustBeEmpty))
        {
          // Check that it is not empty.
          ComponentInventory& inv = components.inventory[object];
          if (inv.count() != 0)
          {
            printMessageTry();
            putTr("THE_FOO_IS_NOT_EMPTY");
            return StateResult::Failure();
          }
        }

        if (!hasTrait(Trait::ObjectCanBeOutOfReach))
        {
          // Check that each object is within reach.
          if (!systems.spacial()->firstCanReachSecond(subject, object))
          {
            printMessageTry();
            putMsg(makeTr("CONJUNCTION_HOWEVER") + " " + makeTr("FOO_PRO_SUB_IS_OUT_OF_REACH"));
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustBeInInventory))
        {
          // Check that each object is in our inventory.
          if (!components.inventory[subject].contains(object))
          {
            printMessageTry();
            auto message = makeTr("CONJUNCTION_HOWEVER") + " " + makeTr("FOO_PRO_SUB_IS_NOT_IN_YOUR_INVENTORY");
            if (systems.spacial()->firstCanReachSecond(subject, object))
            {
              message += makeTr("PICK_UP_OBJECT_FIRST");
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
            printMessageTry();
            putTr("THE_FOO_IS_ALREADY_IN_YOUR_INVENTORY");
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustBeWielded))
        {
          // Check to see if the object is being wielded.
          if (components.bodyparts.existsFor(subject) &&
              components.bodyparts[subject].getWieldedLocation(object).part == BodyPart::Nowhere)
          {
            printMessageTry();
            putTr("THE_FOO_MUST_BE_WIELDED");
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustBeWorn))
        {
          // Check to see if the object is being worn.
          if (components.bodyparts.existsFor(subject) &&
              components.bodyparts[subject].getWornLocation(object).part == BodyPart::Nowhere)
          {
            printMessageTry();
            putTr("THE_FOO_MUST_BE_WORN");
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustNotBeWielded))
        {
          // Check to see if the object is being wielded.
          if (components.bodyparts.existsFor(subject) &&
              components.bodyparts[subject].getWieldedLocation(object).part != BodyPart::Nowhere)
          {
            printMessageTry();

            /// @todo Perhaps automatically try to unwield the item before dropping?
            putTr("YOU_CANT_VERB_WIELDED");
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustNotBeWorn))
        {
          // Check to see if the object is being worn.
          if (components.bodyparts.existsFor(subject) &&
              components.bodyparts[subject].getWornLocation(object).part != BodyPart::Nowhere)
          {
            printMessageTry();
            putTr("YOU_CANT_VERB_WORN");
            return StateResult::Failure();
          }
        }

        // Check that we can perform this Action on this object.
        ReasonBool allowedNow = objectIsAllowedNow(gameState);
        if (!allowedNow.value)
        {
          printMessageTry();
          printMessageCant(); ///< @todo Add the reason why here
          return StateResult::Failure();
        }
      }
    }

    auto result = doPreBeginWorkNVI(gameState, systems);
    return result;
  }

  StateResult Action::doBeginWork(GameState& gameState, SystemManager& systems)
  {
    auto result = doBeginWorkNVI(gameState, systems);
    return result;
  }

  StateResult Action::doFinishWork(GameState& gameState, SystemManager& systems)
  {
    auto result = doFinishWorkNVI(gameState, systems);
    return result;
  }

  StateResult Action::doAbortWork(GameState& gameState, SystemManager& systems)
  {
    auto result = doAbortWorkNVI(gameState, systems);
    return result;
  }

  ReasonBool Action::subjectIsCapable(GameState const& gameState) const
  {
    std::string reason = "Missing subjectIsCapable() implementation for action \"" + m_verb + "\"";
    CLOG(ERROR, "Action") << reason;
    return { false, reason };
  }

  ReasonBool Action::subjectIsCapableNow(GameState const& gameState) const
  {
    return subjectIsCapable(gameState);
  }

  ReasonBool Action::objectIsAllowed(GameState const& gameState) const
  {
    std::string reason = "Missing objectIsAllowed() implementation for action \"" + m_verb + "\"";
    CLOG(ERROR, "Action") << reason;
    return { false, reason };
  }

  ReasonBool Action::objectIsAllowedNow(GameState const& gameState) const
  {
    return objectIsAllowed(gameState);
  }

  StateResult Action::doPreBeginWorkNVI(GameState& gameState, SystemManager& systems)
  {
    /// @todo Set counter_busy based on the action being taken and
    ///       the entity's reflexes.
    return StateResult::Success();
  }

  StateResult Action::doBeginWorkNVI(GameState& gameState, SystemManager& systems)
  {
    putTr("ACTN_NOT_IMPLEMENTED");
    return StateResult::Failure();
  }

  StateResult Action::doFinishWorkNVI(GameState& gameState, SystemManager& systems)
  {
    /// @todo Complete the action here
    return StateResult::Success();
  }

  StateResult Action::doAbortWorkNVI(GameState& gameState, SystemManager& systems)
  {
    /// @todo Handle aborting the action here.
    return StateResult::Success();
  }

  std::string Action::getObjectString(SystemManager& systems) const
  {
    std::string description;

    if (getObjects().size() == 0)
    {
      description += makeTr("NOUN_NOTHING");
    }
    else if (getObjects().size() == 1)
    {
      if (getObject() == getSubject())
      {
        description += getSubject()->getReflexivePronoun();
      }
      else
      {
        if (getObject() == EntityId::Mu())
        {
          description += makeTr("NOUN_NOTHING");
        }
        else
        {
          if (getQuantity() > 1)
          {
            description += getQuantity() + " " + makeTr("PREPOSITION_OF");
          }
          description += getObject()->getDescriptiveString(ArticleChoice::Definite);
        }
      }
    }
    else if (getObjects().size() == 2)
    {
      description += getObject()->getDescriptiveString(ArticleChoice::Definite) + " " + makeTr("CONJUNCTION_AND") + " " +
        getSecondObject()->getDescriptiveString(ArticleChoice::Definite);
    }
    else if (getObjects().size() > 1)
    {
      /// @todo May want to change this depending on whether subject is the player.
      ///       If not, we should print "several items" or something to that effect.
      auto string_items = makeTr("NOUN_ITEMS");
      description += getDefArt(string_items) + " " + string_items;
    }
    else
    {
      auto new_direction = getTargetDirection();
      if (new_direction != Direction::None)
      {
        description += str(new_direction);
      }
    }

    return description;
  }

  std::string Action::getTargetString(SystemManager& systems) const
  {
    auto subject = getSubject();
    auto& objects = getObjects();
    auto target = getTargetThing();

    if (target == subject)
    {
      return subject->getReflexivePronoun();
    }
    else if ((objects.size() == 1) && (target == getObject()))
    {
      return getObject()->getReflexivePronoun();
    }
    else
    {
      return target->getDescriptiveString(ArticleChoice::Definite);
    }
  }

  void Action::printMessageTry() const
  {
    auto& objects = getObjects();
    if (objects.size() == 0)
    {
      putTr("YOU_TRY_TO_VERB");
    }
    else if (objects.size() == 1)
    {
      putTr("YOU_TRY_TO_VERB_THE_FOO");
    }
    else
    {

    }
  }

  void Action::printMessageDo() const
  {
    auto& objects = getObjects();
    if (objects.size() == 0)
    {
      putTr("YOU_CVERB");
    }
    else if (objects.size() == 1)
    {
      putTr("YOU_CVERB_THE_FOO");
    }
    else
    {
      putTr("YOU_CVERB_THE_ITEMS");
    }
  }

  void Action::printMessageBegin() const
  {
    auto& objects = getObjects();
    if (objects.size() == 0)
    {
      putTr("YOU_BEGIN_TO_VERB");
    }
    else if (objects.size() == 1)
    {
      putTr("YOU_BEGIN_TO_VERB_THE_FOO");
    }
    else
    {
      putTr("YOU_BEGIN_TO_VERB_THE_ITEMS");
    }
  }

  void Action::printMessageStop() const
  {
    auto& objects = getObjects();
    if (objects.size() == 0)
    {
      putTr("YOU_STOP_VERBING");
    }
    else if (objects.size() == 1)
    {
      putTr("YOU_STOP_VERBING_THE_FOO");
    }
    else
    {
      putTr("YOU_STOP_VERBING_THE_ITEMS");
    }
  }

  void Action::printMessageFinish() const
  {
    auto& objects = getObjects();
    if (objects.size() == 0)
    {
      putTr("YOU_FINISH_VERBING");
    }
    else if (objects.size() == 1)
    {
      putTr("YOU_FINISH_VERBING_THE_FOO");
    }
    else
    {
      putTr("YOU_FINISH_VERBING_THE_ITEMS");
    }
  }

  void Action::printMessageCant() const
  {
    auto& objects = getObjects();
    if (objects.size() == 0)
    {
      putTr("YOU_CANT_VERB");
    }
    else if (objects.size() == 1)
    {
      putTr("YOU_CANT_VERB_THAT");
    }
    else
    {
      putTr("YOU_CANT_VERB_THOSE");
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

  //std::string Action::makeTr(std::string key) const
  //{
  //  return makeString(Service<IStringDictionary>::get().get(key), {});
  //}

  //std::string Action::makeTr(std::string key, std::vector<std::string> optional_strings) const
  //{
  //  return makeString(Service<IStringDictionary>::get().get(key), optional_strings);
  //}

  //std::string Action::makeString(std::string pattern) const
  //{
  //  return makeString(pattern, {});
  //}

  //std::string Action::makeString(std::string pattern, std::vector<std::string> optional_strings) const
  //{
  //  std::string new_string = StringTransforms::replace_tokens(pattern,
  //                                                            [&](std::string token) -> std::string
  //  {
  //    if (token == "are")
  //    {
  //      return getSubject()->chooseVerb(tr("VERB_BE_2"), tr("VERB_BE_3"));
  //    }
  //    if (token == "were")
  //    {
  //      return getSubject()->chooseVerb(tr("VERB_BE_P2"), tr("VERB_BE_P3"));
  //    }
  //    if (token == "do")
  //    {
  //      return getSubject()->chooseVerb(tr("VERB_DO_2"), tr("VERB_DO_3"));
  //    }
  //    if (token == "get")
  //    {
  //      return getSubject()->chooseVerb(tr("VERB_GET_2"), tr("VERB_GET_3"));
  //    }
  //    if (token == "have")
  //    {
  //      return getSubject()->chooseVerb(tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
  //    }
  //    if (token == "seem")
  //    {
  //      return getSubject()->chooseVerb(tr("VERB_SEEM_2"), tr("VERB_SEEM_3"));
  //    }
  //    if (token == "try")
  //    {
  //      return getSubject()->chooseVerb(tr("VERB_TRY_2"), tr("VERB_TRY_3"));
  //    }

  //    if ((token == "foo_is") || (token == "foois"))
  //    {
  //      return getObject()->chooseVerb(tr("VERB_BE_2"), tr("VERB_BE_3"));
  //    }
  //    if ((token == "foo_has") || (token == "foohas"))
  //    {
  //      return getObject()->chooseVerb(tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
  //    }

  //    if ((token == "the_foo") || (token == "thefoo"))
  //    {
  //      return getObjectString();
  //    }

  //    if ((token == "the_foos_location") || (token == "thefooslocation"))
  //    {
  //      return COMPONENTS.position[getObject()].parent()->getDescriptiveString(ArticleChoice::Definite);
  //    }

  //    if ((token == "the_target_thing") || (token == "thetargetthing"))
  //    {
  //      return getTargetString();
  //    }

  //    if (token == "fooself")
  //    {
  //      return getObject()->getReflexiveString(getSubject(), ArticleChoice::Definite);
  //    }

  //    if ((token == "foo_pro_sub") || (token == "fooprosub"))
  //    {
  //      return getObject()->getSubjectPronoun();
  //    }

  //    if ((token == "foo_pro_obj") || (token == "fooproobj"))
  //    {
  //      return getObject()->getObjectPronoun();
  //    }

  //    if ((token == "foo_pro_ref") || (token == "fooproref"))
  //    {
  //      return getObject()->getReflexivePronoun();
  //    }

  //    if (token == "verb")
  //    {
  //      return getVerb2();
  //    }
  //    if (token == "verb3")
  //    {
  //      return getVerb3();
  //    }
  //    if (token == "verbed")
  //    {
  //      return getVerbed();
  //    }
  //    if (token == "verbing")
  //    {
  //      return getVerbing();
  //    }
  //    if ((token == "verb_pp") || (token == "verbpp"))
  //    {
  //      return getVerbPP();
  //    }
  //    if (token == "cverb")
  //    {
  //      return (getSubject()->isThirdPerson() ? getVerb3() : getVerb2());
  //    }
  //    if (token == "objcverb")
  //    {
  //      return (getObject()->isThirdPerson() ? getVerb3() : getVerb2());
  //    }

  //    if (token == "you")
  //    {
  //      return getSubject()->getSubjectiveString();
  //    }
  //    if ((token == "you_pro_sub") || (token == "youprosub"))
  //    {
  //      return getSubject()->getSubjectPronoun();
  //    }
  //    if ((token == "you_pro_obj") || (token == "youproobj"))
  //    {
  //      return getSubject()->getObjectPronoun();
  //    }
  //    if (token == "yourself")
  //    {
  //      return getSubject()->getReflexivePronoun();
  //    }

  //    if (token == "targdir")
  //    {
  //      std::stringstream ss;
  //      ss << getTargetDirection();
  //      return ss.str();
  //    }

  //    // Check for a numerical token.
  //    try
  //    {
  //      unsigned int converted = static_cast<unsigned int>(std::stoi(token));

  //      // Check that the optional arguments are at least this size.
  //      if (converted < optional_strings.size())
  //      {
  //        // Return the string passed in.
  //        return optional_strings.at(converted);
  //      }
  //    }
  //    catch (std::invalid_argument&)
  //    {
  //      // Not a number, so bail.
  //    }
  //    catch (...)
  //    {
  //      // Throw anything else.
  //      throw;
  //    }

  //    // Nothing else matched, return default.
  //    return "[" + token + "]";
  //  },
  //  [&](std::string token, std::string arg) -> std::string
  //  {
  //    if (token == "your")
  //    {
  //      return getSubject()->getPossessiveString(arg);
  //    }

  //    return "[" + token + "(" + arg + ")]";
  //  },
  //                                          [&](std::string token) -> bool
  //  {
  //    if ((token == "cv") || (token == "subjcv") || (token == "subj_cv"))
  //    {
  //      return !(getSubject()->isThirdPerson());
  //    }
  //    if ((token == "objcv") || (token == "obj_cv") || (token == "foocv") || (token == "foo_cv"))
  //    {
  //      return !(getObject()->isThirdPerson());
  //    }
  //    if ((token == "isPlayer") || (token == "isplayer"))
  //    {
  //      return getSubject()->isPlayer();
  //    }
  //    if ((token == "targcv") || (token == "targ_cv"))
  //    {
  //      return !(getTargetThing()->isThirdPerson());
  //    }

  //    if (token == "true")
  //    {
  //      return true;
  //    }
  //    if (token == "false")
  //    {
  //      return false;
  //    }

  //    return true;
  //  });

  //  return new_string;
  //}

  ActionMap const & Action::getMap()
  {
    return Action::s_actionMap;
  }

} // end namespace

