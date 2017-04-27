#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"

namespace Actions
{
  class ActionGet
    :
    public Action, public ActionCRTP<ActionGet>
  {
  private:
    ActionGet();
  public:
    explicit ActionGet(EntityId subject);
    virtual ~ActionGet();
    static ActionGet prototype;

    //virtual bool subjectIsCapable() const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doFinishWorkNVI(AnyMap& params) override;
    virtual StateResult doAbortWorkNVI(AnyMap& params) override;
  };

} // end namespace