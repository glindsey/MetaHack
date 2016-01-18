#pragma once

#include "Action.h"
#include <string>

class ActionEat
  :
  public Action
{
public:
  ActionEat();
  virtual ~ActionEat();

  virtual std::string get_type() const
  {
    return "Eat";
  }

protected:
  virtual StateResult do_prebegin_work(ThingRef actor, AnyMap& params) override;
  virtual StateResult do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params) override;
  virtual StateResult do_finish_work(ThingRef actor, AnyMap& params) override;
  virtual StateResult do_abort_work(ThingRef actor, AnyMap& params) override;
};
