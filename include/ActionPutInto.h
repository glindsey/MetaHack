#pragma once

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionPutInto
  :
  public Action, public ActionCRTP<ActionPutInto>
{
  ACTION_HDR_BOILERPLATE(ActionPutInto)

    /// @todo Handle putting multiple items into a thing. Right now only the
    ///       first item is processed.
public:
  ACTION_TRAIT(can_be_subject_verb_thing_preposition_thing);
  ACTION_TRAIT(can_be_subject_verb_things_preposition_thing);

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;

  virtual void print_message_try_() override;
  virtual void print_message_do_() override;
};
