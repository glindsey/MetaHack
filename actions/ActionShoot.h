#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "EntityId.h"

namespace Actions
{
  class ActionShoot
    :
    public Action, public ActionCRTP<ActionShoot>
  {
  private:
    ActionShoot();
  public:
    explicit ActionShoot(EntityId subject);
    virtual ~ActionShoot();
    static ActionShoot prototype;

    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult do_prebegin_work_(AnyMap& params) override;
    virtual StateResult do_begin_work_(AnyMap& params) override;
    virtual StateResult do_finish_work_(AnyMap& params) override;
    virtual StateResult do_abort_work_(AnyMap& params) override;
  };

} // end namespace