#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"

namespace Actions
{
  class ActionClose
    :
    public Action, public ActionCRTP<ActionClose>
  {
  private:
    ActionClose();
  public:
    explicit ActionClose(EntityId subject);
    virtual ~ActionClose();
    static ActionClose prototype;

    virtual ReasonBool subjectIsCapable() const override;
    //virtual ReasonBool objectIsAllowed() const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doFinishWorkNVI(AnyMap& params) override;
    virtual StateResult doAbortWorkNVI(AnyMap& params) override;
  };

} // end namespace
