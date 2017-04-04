#include "stdafx.h"

#include "ActionTakeOff.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionTakeOff ActionTakeOff::prototype;
  ActionTakeOff::ActionTakeOff() : Action("disrobe", "DISROBE", ActionTakeOff::create_) {}
  ActionTakeOff::ActionTakeOff(EntityId subject) : Action(subject, "disrobe", "DISROBE") {}
  ActionTakeOff::~ActionTakeOff() {}

  std::unordered_set<Trait> const & ActionTakeOff::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbBodypart,
      Trait::ObjectMustBeWorn
    };

    return traits;
  }

  StateResult ActionTakeOff::doPreBeginWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionTakeOff::doBeginWorkNVI(AnyMap& params)
  {
    StateResult result = StateResult::Failure();
    std::string message;

    auto subject = getSubject();
    auto object = getObject();

    BodyLocation wear_location;
    subject->isWearing(object, wear_location);

    std::string bodypart_desc = subject->getBodypartDescription(wear_location);

    // Check if the worn item is bound.
    if (object->getModifiedProperty("bound", false))
    {
      std::string message;
      putMsg(makeTr("YOU_CANT_VERB_FOO_MAGICALLY_BOUND", 
      { subject->getPossessiveString(bodypart_desc) }));

      // Premature exit.
      return result;
    }

    // Try to take off the item.
    /// @todo Disrobing shouldn't be instantaneous.
    auto lua_result = object->be_object_of(*this, subject);
    if (object->be_object_of(*this, subject))
    {
      std::string message;
      putMsg(makeTr("YOU_CVERB_THE_FOO") + " " + 
             makeTr("YOU_ARE_NOW_WEARING_NOTHING", 
             { subject->getPossessiveString(bodypart_desc) }));
      subject->setWielded(EntityId::Mu(), wear_location);
    }

    return result;
  }

  StateResult ActionTakeOff::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionTakeOff::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
}
