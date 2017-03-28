#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"

namespace Actions
{
  class ActionEat
    :
    public Action, public ActionCRTP<ActionEat>
  {
  private:
    ActionEat();
  public:
    explicit ActionEat(EntityId subject);
    virtual ~ActionEat();
    static ActionEat prototype;

    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doFinishWorkNVI(AnyMap& params) override;
    virtual StateResult doAbortWorkNVI(AnyMap& params) override;

  private:
    /// The status of the be_object_of() call, which needs to be saved
    /// for the "finish" portion of the action.
    ActionResult m_last_eat_result;
  };

} // end namespace