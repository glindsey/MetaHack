#pragma once

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionDie
  :
  public Action, public ActionCRTP<ActionDie>
{
public:
  explicit ActionDie(ThingRef subject);
  virtual ~ActionDie();

  virtual std::string get_type() const override
  {
    return "Die";
  }

  ACTION_TRAIT(can_be_subject_only);

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};
