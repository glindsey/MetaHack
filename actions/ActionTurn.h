#pragma once

#include "Action.h"
#include "ActionCRTP.h"
#include "EntityId.h"

#include <string>
#include <vector>

/// Action: Turn to face a particular direction.
namespace Actions
{
  class ActionTurn
    :
    public Action, public ActionCRTP<ActionTurn>
  {
  private:
    ActionTurn();
  public:
    explicit ActionTurn(EntityId subject);
    virtual ~ActionTurn();
    static ActionTurn prototype;

    virtual std::unordered_set<Action::Trait> const& getTraits() const override;

  protected:
    virtual StateResult do_prebegin_work_(AnyMap& params) override;
    virtual StateResult do_begin_work_(AnyMap& params) override;
    virtual StateResult do_finish_work_(AnyMap& params) override;
    virtual StateResult do_abort_work_(AnyMap& params) override;
  };

} // end namespace