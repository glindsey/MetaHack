#pragma once

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionTakeOut
  :
  public Action, public ActionCRTP<ActionTakeOut>
{
  ACTION_HDR_BOILERPLATE(ActionTakeOut)
    ACTION_TRAIT(can_be_subject_verb_object)
    ACTION_TRAIT(can_be_subject_verb_objects)

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
