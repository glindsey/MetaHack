#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "EntityId.h"

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

    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult do_prebegin_work_(AnyMap& params) override;
    virtual StateResult do_begin_work_(AnyMap& params) override;
    virtual StateResult do_finish_work_(AnyMap& params) override;
    virtual StateResult do_abort_work_(AnyMap& params) override;
  };

} // end namespace