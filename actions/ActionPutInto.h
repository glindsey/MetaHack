#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"

namespace Actions
{
  class ActionPutInto
    :
    public Action, public ActionCRTP<ActionPutInto>
  {
  private:
    ActionPutInto();
  public:
    explicit ActionPutInto(EntityId subject);
    virtual ~ActionPutInto();
    static ActionPutInto prototype;

    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doFinishWorkNVI(AnyMap& params) override;
    virtual StateResult doAbortWorkNVI(AnyMap& params) override;

    virtual void printMessageTry() const override;
    virtual void printMessageDo() const override;
  };
 
} // end namespace