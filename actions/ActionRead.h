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

    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult do_prebegin_work_(AnyMap& params) override;
    virtual StateResult do_begin_work_(AnyMap& params) override;
    virtual StateResult do_finish_work_(AnyMap& params) override;
    virtual StateResult do_abort_work_(AnyMap& params) override;

    virtual void print_message_cant_() const override;
  };

} // end namespace
