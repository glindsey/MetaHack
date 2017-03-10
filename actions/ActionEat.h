#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "EntityId.h"

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
    virtual StateResult do_prebegin_work_(AnyMap& params) override;
    virtual StateResult do_begin_work_(AnyMap& params) override;
    virtual StateResult do_finish_work_(AnyMap& params) override;
    virtual StateResult do_abort_work_(AnyMap& params) override;

  private:
    /// The status of the be_object_of() call, which needs to be saved
    /// for the "finish" portion of the action.
    ActionResult m_last_eat_result;
  };

} // end namespace