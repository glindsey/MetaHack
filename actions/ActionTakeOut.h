#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"

/// Action class for taking an item out of another item.
/// Why does this item have the "object can be out of reach" trait? Well, the
/// object is inside a container, so it is technically "out of reach" as far
/// as the game engine is concerned, until it is removed from the container.
/// We DO check if the CONTAINER is within reach, which is what really matters.
namespace Actions
{
  class ActionTakeOut
    :
    public Action, public ActionCRTP<ActionTakeOut>
  {
  private:
    ActionTakeOut();
  public:
    explicit ActionTakeOut(EntityId subject);
    virtual ~ActionTakeOut();
    static ActionTakeOut prototype;

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