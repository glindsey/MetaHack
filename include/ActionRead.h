#pragma once

#include "Action.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionRead
  :
  public Action
{
public:
  ActionRead(ThingRef subject, ThingRef object);
  virtual ~ActionRead();

  virtual std::string get_type() const override
  {
    return "Read";
  }

  ACTION_TRAIT(can_be_subject_verb_thing);

protected:
  virtual StateResult do_prebegin_work(AnyMap& params) override;
  virtual StateResult do_begin_work(AnyMap& params) override;
  virtual StateResult do_finish_work(AnyMap& params) override;
  virtual StateResult do_abort_work(AnyMap& params) override;
};
