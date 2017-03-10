#pragma once

#include "Action.h"
#include "ActionCRTP.h"
#include "EntityId.h"

#include <string>
#include <vector>

namespace Actions
{
  class ActionMove
    :
    public Action, public ActionCRTP<ActionMove>
  {
  private:
    ActionMove();
  public:
    explicit ActionMove(EntityId subject);
    virtual ~ActionMove();
    static ActionMove prototype;

    virtual std::unordered_set<Action::Trait> const& getTraits() const override;

  protected:
    virtual StateResult do_prebegin_work_(AnyMap& params) override;
    virtual StateResult do_begin_work_(AnyMap& params) override;
    virtual StateResult do_finish_work_(AnyMap& params) override;
    virtual StateResult do_abort_work_(AnyMap& params) override;
  };

} // end namespace