#include "stdafx.h"

#include "types/common.h"

#include "Action.h"
#include "ActionMove.h"

#include "services/IStringDictionary.h"
#include "services/MessageLog.h"
#include "Service.h"
#include "utilities/StringTransforms.h"
#include "entity/Entity.h"
#include "entity/EntityPool.h"
#include "entity/EntityId.h"

namespace Actions
{
  std::unordered_map<std::string, ActionCreator> Action::action_map;

  struct Action::Impl
  {
    Impl(EntityId subject_, std::string type_, std::string verb_)
      :
      state{ State::Pending },
      subject{ subject_ },
      objects{},
      target_thing{ EntityId::Mu() },
      target_direction{ Direction::None },
      quantity{ 1 },
      type{ type_ },
      verb{ verb_ }
    {}

    /// State of this action.
    State state;

    /// The subject performing the action.
    EntityId subject;

    /// The objects of the action.
    std::vector<EntityId> objects;

    /// Target Entity for the action (if any).
    EntityId target_thing;

    /// Direction for the action (if any).
    Direction target_direction;

    /// Quantity for the action (only used in drop/pickup).
    unsigned int quantity;

    /// This action's type name.
    std::string const type;

    /// This action's verb.
    std::string const verb;
  };

  Action::Action(std::string type, std::string verb, ActionCreator creator)
    :
    pImpl{ new Impl(EntityId::Mu(), type, verb) }
  {
    registerActionAs(type, creator);
  }

  Action::Action(EntityId subject, std::string type, std::string verb)
    :
    pImpl{ new Impl(subject, type, verb) }
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
    return pImpl->subject;
  }

  void Action::setObject(EntityId object)
  {
    pImpl->objects.clear();
    pImpl->objects.push_back(object);
  }

  void Action::setObjects(std::vector<EntityId> objects)
  {
    pImpl->objects = objects;
  }

  std::vector<EntityId> const& Action::getObjects() const
  {
    return pImpl->objects;
  }

  EntityId Action::getObject() const
  {
    return pImpl->objects[0];
  }

  EntityId Action::getSecondObject() const
  {
    return pImpl->objects[1];
  }

  bool Action::process(AnyMap params)
  {
    auto subject = getSubject();

    // If entity is currently busy, decrement by one and return.
    int counter_busy = subject->getBaseProperty("counter-busy", 0);
    if (counter_busy > 0)
    {
      subject->addToBaseProperty("counter-busy", -1);
      return false;
    }

    // Continue running through states until the event is processed, or the
    // target actor is busy.
    while ((pImpl->state != State::Processed) && (counter_busy == 0))
    {
      counter_busy = subject->getBaseProperty("counter-busy", 0);
      StateResult result{ false, 0 };

      CLOG(TRACE, "Action") << "Entity #" <<
        subject << " (" <<
        subject->getType().c_str() << "): Action " <<
        getType().c_str() << " is in state " <<
        str(getState());

      switch (pImpl->state)
      {
        case State::Pending:
          result = doPreBeginWork(params);

          if (result.success)
          {
            // Update the busy counter.
            pImpl->subject->setBaseProperty("counter-busy", result.elapsed_time);
            setState(State::PreBegin);
          }
          else
          {
            // Clear the busy counter.
            pImpl->subject->setBaseProperty("counter-busy", 0);
            setState(State::PostFinish);
          }
          break;

        case State::PreBegin:
          result = doBeginWork(params);

          // If starting the action succeeded, move to the in-progress state.
          // Otherwise, just go right to post-finish.
          if (result.success)
          {
            // Update the busy counter.
            pImpl->subject->setBaseProperty("counter-busy", result.elapsed_time);
            setState(State::InProgress);
          }
          else
          {
            // Clear the busy counter.
            pImpl->subject->setBaseProperty("counter-busy", 0);
            setState(State::PostFinish);
          }
          break;

        case State::InProgress:
          result = doFinishWork(params);

          pImpl->subject->setBaseProperty("counter-busy", result.elapsed_time);
          setState(State::PostFinish);
          break;

        case State::Interrupted:
          result = doAbortWork(params);

          pImpl->subject->addToBaseProperty("counter-busy", result.elapsed_time);
          setState(State::PostFinish);
          break;

        case State::PostFinish:
          setState(State::Processed);
          break;

        default:
          break;
      }
    }

    return true;
  }

  void Action::setState(State state)
  {
    auto subject = getSubject();
    CLOG(TRACE, "Action") << "Entity #" <<
      subject << " (" <<
      subject->getType().c_str() << "): Action " <<
      getType().c_str() << " switching to state " <<
      str(getState());

    pImpl->state = state;
  }

  State Action::getState()
  {
    return pImpl->state;
  }

  void Action::setTarget(EntityId entity) const
  {
    pImpl->target_thing = entity;
    pImpl->target_direction = Direction::None;
  }

  void Action::setTarget(Direction direction) const
  {
    pImpl->target_thing = EntityId::Mu();
    pImpl->target_direction = direction;
  }

  void Action::setQuantity(unsigned int quantity) const
  {
    pImpl->quantity = quantity;
  }

  EntityId Action::getTargetThing() const
  {
    return pImpl->target_thing;
  }

  Direction Action::getTargetDirection() const
  {
    return pImpl->target_direction;
  }

  unsigned int Action::getQuantity() const
  {
    return pImpl->quantity;
  }

  std::string Action::getType() const
  {
    return pImpl->type;
  }

  std::string Action::getVerb2() const
  {
    auto& dict = Service<IStringDictionary>::get();
    return dict.get("VERB_" + pImpl->verb + "_2");
  }

  std::string Action::getVerb3() const
  {
    auto& dict = Service<IStringDictionary>::get();
    return dict.get("VERB_" + pImpl->verb + "_3");
  }

  std::string Action::getVerbing() const
  {
    auto& dict = Service<IStringDictionary>::get();
    return dict.get("VERB_" + pImpl->verb + "_GER");
  }

  std::string Action::getVerbed() const
  {
    auto& dict = Service<IStringDictionary>::get();
    return dict.get("VERB_" + pImpl->verb + "_P2");
  }

  std::string Action::getVerbPP() const
  {
    auto& dict = Service<IStringDictionary>::get();
    return dict.get("VERB_" + pImpl->verb + "_PP");
  }

  std::string Action::getVerbable() const
  {
    auto& dict = Service<IStringDictionary>::get();
    return dict.get("VERB_" + pImpl->verb + "_ABLE");
  }

  StateResult Action::doPreBeginWork(AnyMap& params)
  {
    auto subject = getSubject();
    auto& objects = getObjects();
    auto location = subject->getLocation();
    MapTile* current_tile = subject->getMapTile();
    auto new_direction = getTargetDirection();

    // Check that we're capable of eating at all.
    std::string canVerb{ "can-" + getType() };
    if (!subject->getIntrinsic(canVerb, false))
    {
      printMessageTry();
      putMsg(makeTr("YOU_ARE_NOT_CAPABLE_OF_VERBING", { getIndefArt(subject->getDisplayName()), subject->getDisplayName() }));
      return StateResult::Failure();
    }

    // Check that we're capable of eating right now.
    if (!subject->getModifiedProperty(canVerb, false))
    {
      printMessageTry();
      putMsg(makeTr("YOU_CANT_VERB_NOW", { getIndefArt(subject->getDisplayName()), subject->getDisplayName() }));
      return StateResult::Failure();
    }

    if (hasTrait(Trait::SubjectMustBeAbleToMove))
    {
      // Make sure we can move RIGHT NOW.
      if (!subject->canCurrentlyMove())
      {
        putTr("YOU_CANT_MOVE_NOW");
        return StateResult::Failure();
      }
    }

    if (!hasTrait(Trait::SubjectCanBeInLimbo))
    {
      // Make sure we're not in limbo!
      if ((location == EntityId::Mu()) || (current_tile == nullptr))
      {
        putTr("DONT_EXIST_PHYSICALLY");
        return StateResult::Failure();
      }
    }

    if (hasTrait(Trait::SubjectCanNotBeInsideAnotherObject))
    {
      // Make sure we're not confined inside another entity.
      /// @todo Allow for attacking when swallowed!
      if (subject->isInsideAnotherEntity())
      {
        printMessageTry();
        putMsg(makeTr("YOU_ARE_INSIDE_OBJECT",
        { location->getDescriptiveString(ArticleChoice::Indefinite) }));
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
            return doPreBeginWorkNVI(params);
          }
        }
        else
        {
          if (subject == object)
          {
            if (!subject->isPlayer())
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
          /// @todo Do something better here.
          if (!object->getIntrinsic("liquid_carrier", false))
          {
            printMessageTry();
            putTr("THE_FOO_IS_NOT_A_LIQUID_CARRIER");
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustNotBeEmpty))
        {
          // Check that it is not empty.
          Inventory& inv = object->getInventory();
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
          Inventory& inv = object->getInventory();
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
          if (!subject->canReach(object))
          {
            printMessageTry();
            putMsg(makeTr("CONJUNCTION_HOWEVER") + " " + makeTr("FOO_PRO_SUB_IS_OUT_OF_REACH"));
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustBeInInventory))
        {
          // Check that each object is in our inventory.
          if (!subject->getInventory().contains(object))
          {
            printMessageTry();
            auto message = makeTr("CONJUNCTION_HOWEVER") + " " + makeTr("FOO_PRO_SUB_IS_NOT_IN_YOUR_INVENTORY");
            if (subject->canReach(object))
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
          if (subject->getInventory().contains(object))
          {
            printMessageTry();
            putTr("THE_FOO_IS_ALREADY_IN_YOUR_INVENTORY");
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustBeWielded))
        {
          // Check to see if the object is being wielded.
          if (!subject->isWielding(object))
          {
            printMessageTry();
            putTr("THE_FOO_MUST_BE_WIELDED");
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustBeWorn))
        {
          // Check to see if the object is being worn.
          if (!subject->isWearing(object))
          {
            printMessageTry();
            putTr("THE_FOO_MUST_BE_WORN");
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustNotBeWielded))
        {
          // Check to see if the object is being wielded.
          if (subject->isWielding(object))
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
          if (subject->isWearing(object))
          {
            printMessageTry();
            putTr("YOU_CANT_VERB_WORN");
            return StateResult::Failure();
          }
        }

        if (hasTrait(Trait::ObjectMustBeMovableBySubject))
        {
          // Check to see if we can move the object.
          if (!object->canHaveActionDoneBy(subject, ActionMove::prototype))
          {
            printMessageTry();

            putTr("YOU_CANT_MOVE_THE_FOO");
            return StateResult::Failure();
          }
        }

        // Check that we can perform this Action on this object.
        if (!object->canHaveActionDoneBy(subject, *this))
        {
          printMessageTry();
          printMessageCant();
          return StateResult::Failure();
        }
      }
    }

    auto result = doPreBeginWorkNVI(params);
    return result;
  }

  StateResult Action::doBeginWork(AnyMap& params)
  {
    auto result = doBeginWorkNVI(params);
    return result;
  }

  StateResult Action::doFinishWork(AnyMap& params)
  {
    auto result = doFinishWorkNVI(params);
    return result;
  }

  StateResult Action::doAbortWork(AnyMap& params)
  {
    auto result = doAbortWorkNVI(params);
    return result;
  }

  StateResult Action::doPreBeginWorkNVI(AnyMap& params)
  {
    /// @todo Set counter_busy based on the action being taken and
    ///       the entity's reflexes.
    return StateResult::Success();
  }

  StateResult Action::doBeginWorkNVI(AnyMap& params)
  {
    putTr("ACTN_NOT_IMPLEMENTED");
    return StateResult::Failure();
  }

  StateResult Action::doFinishWorkNVI(AnyMap& params)
  {
    /// @todo Complete the action here
    return StateResult::Success();
  }

  StateResult Action::doAbortWorkNVI(AnyMap& params)
  {
    /// @todo Handle aborting the action here.
    return StateResult::Success();
  }

  std::string Action::getObjectString() const
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

  std::string Action::getTargetString() const
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
    Action::action_map.insert({ key, creator });
  }

  bool Action::exists(std::string key)
  {
    return Action::action_map.count(key) != 0;
  }

  std::unique_ptr<Action> Action::create(std::string key, EntityId subject)
  {
    if (Action::action_map.count(key) != 0)
    {
      std::unique_ptr<Action> action = (Action::action_map.at(key))(subject);
      return std::move(action);
    }
    else
    {
      throw std::runtime_error("Requested non-existent action " + key);
    }
  }

  std::string Action::makeTr(std::string key) const
  {
    return makeString(Service<IStringDictionary>::get().get(key), {});
  }

  std::string Action::makeTr(std::string key, std::vector<std::string> optional_strings) const
  {
    return makeString(Service<IStringDictionary>::get().get(key), optional_strings);
  }

  std::string Action::makeString(std::string pattern) const
  {
    return makeString(pattern, {});
  }

  std::string Action::makeString(std::string pattern, std::vector<std::string> optional_strings) const
  {
    std::string new_string = StringTransforms::replace_tokens(pattern,
                                                              [&](std::string token) -> std::string
    {
      if (token == "are")
      {
        return getSubject()->chooseVerb(tr("VERB_BE_2"), tr("VERB_BE_3"));
      }
      if (token == "were")
      {
        return getSubject()->chooseVerb(tr("VERB_BE_P2"), tr("VERB_BE_P3"));
      }
      if (token == "do")
      {
        return getSubject()->chooseVerb(tr("VERB_DO_2"), tr("VERB_DO_3"));
      }
      if (token == "get")
      {
        return getSubject()->chooseVerb(tr("VERB_GET_2"), tr("VERB_GET_3"));
      }
      if (token == "have")
      {
        return getSubject()->chooseVerb(tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
      }
      if (token == "seem")
      {
        return getSubject()->chooseVerb(tr("VERB_SEEM_2"), tr("VERB_SEEM_3"));
      }
      if (token == "try")
      {
        return getSubject()->chooseVerb(tr("VERB_TRY_2"), tr("VERB_TRY_3"));
      }

      if ((token == "foo_is") || (token == "foois"))
      {
        return getObject()->chooseVerb(tr("VERB_BE_2"), tr("VERB_BE_3"));
      }
      if ((token == "foo_has") || (token == "foohas"))
      {
        return getObject()->chooseVerb(tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
      }

      if ((token == "the_foo") || (token == "thefoo"))
      {
        return getObjectString();
      }

      if ((token == "the_foos_location") || (token == "thefooslocation"))
      {
        return getObject()->getLocation()->getDescriptiveString(ArticleChoice::Definite);
      }

      if ((token == "the_target_thing") || (token == "thetargetthing"))
      {
        return getTargetString();
      }

      if (token == "fooself")
      {
        return getObject()->getReflexiveString(getSubject(), ArticleChoice::Definite);
      }

      if ((token == "foo_pro_sub") || (token == "fooprosub"))
      {
        return getObject()->getSubjectPronoun();
      }

      if ((token == "foo_pro_obj") || (token == "fooproobj"))
      {
        return getObject()->getObjectPronoun();
      }

      if ((token == "foo_pro_ref") || (token == "fooproref"))
      {
        return getObject()->getReflexivePronoun();
      }

      if (token == "verb")
      {
        return getVerb2();
      }
      if (token == "verb3")
      {
        return getVerb3();
      }
      if (token == "verbed")
      {
        return getVerbed();
      }
      if (token == "verbing")
      {
        return getVerbing();
      }
      if ((token == "verb_pp") || (token == "verbpp"))
      {
        return getVerbPP();
      }
      if (token == "cverb")
      {
        return (getSubject()->isThirdPerson() ? getVerb2() : getVerb3());
      }
      if (token == "objcverb")
      {
        return (getObject()->isThirdPerson() ? getVerb2() : getVerb3());
      }

      if (token == "you")
      {
        return getSubject()->getSubjectiveString();
      }
      if ((token == "you_pro_sub") || (token == "youprosub"))
      {
        return getSubject()->getSubjectPronoun();
      }
      if ((token == "you_pro_obj") || (token == "youproobj"))
      {
        return getSubject()->getObjectPronoun();
      }
      if (token == "yourself")
      {
        return getSubject()->getReflexivePronoun();
      }

      if (token == "targdir")
      {
        std::stringstream ss;
        ss << getTargetDirection();
        return ss.str();
      }

      // Check for a numerical token.
      try
      {
        unsigned int converted = static_cast<unsigned int>(std::stoi(token));

        // Check that the optional arguments are at least this size.
        if (converted < optional_strings.size())
        {
          // Return the string passed in.
          return optional_strings.at(converted);
        }
      }
      catch (std::invalid_argument&)
      {
        // Not a number, so bail.
      }
      catch (...)
      {
        // Throw anything else.
        throw;
      }

      // Nothing else matched, return default.
      return "[" + token + "]";
    },
    [&](std::string token, std::string arg) -> std::string
    {
      if (token == "your")
      {
        return getSubject()->getPossessiveString(arg);
      }

      return "[" + token + "(" + arg + ")]";
    },
                                            [&](std::string token) -> bool
    {
      if ((token == "cv") || (token == "subjcv") || (token == "subj_cv"))
      {
        return getSubject()->isThirdPerson();
      }
      if ((token == "objcv") || (token == "obj_cv") || (token == "foocv") || (token == "foo_cv"))
      {
        return getObject()->isThirdPerson();
      }
      if ((token == "isPlayer") || (token == "isplayer"))
      {
        return getSubject()->isPlayer();
      }
      if ((token == "targcv") || (token == "targ_cv"))
      {
        return getTargetThing()->isThirdPerson();
      }

      if (token == "true")
      {
        return true;
      }
      if (token == "false")
      {
        return false;
      }

      return true;
    });

    return new_string;
  }

  ActionMap const & Action::getMap()
  {
    return Action::action_map;
  }

} // end namespace

