#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "EntityId.h"

namespace Actions
{
  class ActionLock
    :
    public Action, public ActionCRTP<ActionLock>
  {
  private:
    ActionLock();
  public:
    explicit ActionLock(EntityId subject);
    virtual ~ActionLock();
    static ActionLock prototype;

    virtual std::unordered_set<Action::Trait> const& getTraits() const override;

  protected:
    virtual StateResult do_prebegin_work_(AnyMap& params) override;
    virtual StateResult do_begin_work_(AnyMap& params) override;
    virtual StateResult do_finish_work_(AnyMap& params) override;
    virtual StateResult do_abort_work_(AnyMap& params) override;
  };

} // end namespace
