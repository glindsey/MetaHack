#pragma once

#include "Action.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionMove
  :
  public Action
{
public:
  explicit ActionMove(ThingRef subject);
  virtual ~ActionMove();

  virtual std::string get_type() const override
  {
    return "Move";
  }

  ACTION_TRAIT(can_be_subject_verb_direction);

protected:
  virtual StateResult do_prebegin_work(AnyMap& params) override;
  virtual StateResult do_begin_work(AnyMap& params) override;
  virtual StateResult do_finish_work(AnyMap& params) override;
  virtual StateResult do_abort_work(AnyMap& params) override;
};
