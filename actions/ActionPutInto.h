#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "EntityId.h"

namespace Actions
{
  class ActionPutInto
    :
    public Action, public ActionCRTP<ActionPutInto>
  {
  private:
    ActionPutInto();
  public:
    explicit ActionPutInto(EntityId subject);
    virtual ~ActionPutInto();
    static ActionPutInto prototype;

    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult do_prebegin_work_(AnyMap& params) override;
    virtual StateResult do_begin_work_(AnyMap& params) override;
    virtual StateResult do_finish_work_(AnyMap& params) override;
    virtual StateResult do_abort_work_(AnyMap& params) override;

    virtual void print_message_try_() const override;
    virtual void print_message_do_() const override;
  };
 
} // end namespace