#pragma once

#include "Action.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionTakeOut
  :
  public Action
{
  /// @todo Handle taking multiple items out of a thing. Right now only the
  ///       first item is processed.
public:
  ActionTakeOut(ThingRef subject, ThingRef object);
  ActionTakeOut(ThingRef subject, std::vector<ThingRef> objects);
  virtual ~ActionTakeOut();

  virtual std::string get_type() const override
  {
    return "TakeOut";
  }

  ACTION_TRAIT(can_be_subject_verb_thing);
  ACTION_TRAIT(can_be_subject_verb_things);

protected:
  virtual StateResult do_prebegin_work(AnyMap& params) override;
  virtual StateResult do_begin_work(AnyMap& params) override;
  virtual StateResult do_finish_work(AnyMap& params) override;
  virtual StateResult do_abort_work(AnyMap& params) override;
};
