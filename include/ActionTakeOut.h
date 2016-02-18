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

    /// @todo Handle taking multiple items out of a thing. Right now only the
    ///       first item is processed.
public:
  ACTION_TRAIT(can_be_subject_verb_thing);
  ACTION_TRAIT(can_be_subject_verb_things);

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};
