#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"

namespace Actions
{
  class ActionMix
    :
    public Action, public ActionCRTP<ActionMix>
  {
  private:
    ActionMix();
  public:
    explicit ActionMix(EntityId subject);
    virtual ~ActionMix();
    static ActionMix prototype;

    //virtual ReasonBool subjectIsCapable() const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doFinishWorkNVI(AnyMap& params) override;
    virtual StateResult doAbortWorkNVI(AnyMap& params) override;
  };

} // end namespace