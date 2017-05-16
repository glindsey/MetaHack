#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"
#include "types/BodyPart.h"

namespace Actions
{
  class ActionWield
    :
    public Action, public ActionCRTP<ActionWield>
  {
  private:
    ActionWield();
  public:
    explicit ActionWield(EntityId subject);
    virtual ~ActionWield();
    static ActionWield prototype;

    virtual ReasonBool subjectIsCapable() const override;
    //virtual ReasonBool objectIsAllowed() const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doFinishWorkNVI(AnyMap& params) override;
    virtual StateResult doAbortWorkNVI(AnyMap& params) override;

  private:
    BodyLocation m_bodyLocation;
  };

} // end namespace