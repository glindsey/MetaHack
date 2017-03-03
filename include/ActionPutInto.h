#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "EntityId.h"

class ActionPutInto
  :
  public Action, public ActionCRTP<ActionPutInto>
{
  ACTION_HDR_BOILERPLATE(ActionPutInto)
    ACTION_TRAIT(can_be_subject_verb_object_preposition_target)
    ACTION_TRAIT(can_be_subject_verb_objects_preposition_target)

    /// @todo Handle putting multiple items into a thing. Right now only the
    ///       first item is processed.

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;

  virtual void print_message_try_() const override;
  virtual void print_message_do_() const override;
};
