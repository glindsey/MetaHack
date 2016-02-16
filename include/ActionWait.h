#pragma once

#include "Action.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionWait
  :
  public Action
{
public:
  explicit ActionWait(ThingRef subject);
  virtual ~ActionWait();

  virtual std::string get_type() const override
  {
    return "Wait";
  }

  ACTION_TRAIT(can_be_subject_only);

protected:
  virtual StateResult do_prebegin_work(AnyMap& params) override;
  virtual StateResult do_begin_work(AnyMap& params) override;
  virtual StateResult do_finish_work(AnyMap& params) override;
  virtual StateResult do_abort_work(AnyMap& params) override;
};
