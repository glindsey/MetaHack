#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"
#include "types/BodyPart.h"

namespace Actions
{
  class ActionWear
    :
    public Action, public ActionCRTP<ActionWear>
  {
  private:
    ActionWear();
  public:
    explicit ActionWear(EntityId subject);
    virtual ~ActionWear();
    static ActionWear prototype;

    //virtual ReasonBool subjectIsCapable() const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doFinishWorkNVI(AnyMap& params) override;
    virtual StateResult doAbortWorkNVI(AnyMap& params) override;

  private:
    BodyLocation m_body_location;
  };

} // end namespace
