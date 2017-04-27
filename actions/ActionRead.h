#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"

namespace Actions
{
  class ActionRead
    :
    public Action, public ActionCRTP<ActionRead>
  {
  private:
    ActionRead();
  public:
    explicit ActionRead(EntityId subject);
    virtual ~ActionRead();
    static ActionRead prototype;

    //virtual bool subjectIsCapable() const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doFinishWorkNVI(AnyMap& params) override;
    virtual StateResult doAbortWorkNVI(AnyMap& params) override;

    virtual void printMessageCant() const override;
  };

} // end namespace
