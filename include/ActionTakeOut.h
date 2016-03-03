#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingRef.h"

/// Action class for taking an item out of another item.
/// Why does this item have the "object can be out of reach" trait? Well, the
/// object is inside a container, so it is technically "out of reach" as far
/// as the game engine is concerned, until it is removed from the container.
/// We DO check if the CONTAINER is within reach, which is what really matters.
class ActionTakeOut
  :
  public Action, public ActionCRTP<ActionTakeOut>
{
  ACTION_HDR_BOILERPLATE(ActionTakeOut)
    ACTION_TRAIT(can_be_subject_verb_object)
    ACTION_TRAIT(can_be_subject_verb_objects)
    ACTION_TRAIT(object_can_be_out_of_reach)

    /// @todo Handle taking multiple items out of a thing. Right now only the
    ///       first item is processed.
public:

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;

  virtual void print_message_try_() override;
  virtual void print_message_do_() override;
};
