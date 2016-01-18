#pragma once

#include "Action.h"
#include <string>

class ActionGet
  :
  public Action
{
public:
  ActionGet();
  virtual ~ActionGet();

  virtual std::string get_type() const
  {
    return "Get";
  }

protected:
  virtual StateResult do_prebegin_work(ThingRef actor, AnyMap& params) override;
  virtual StateResult do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params) override;
  virtual StateResult do_finish_work(ThingRef actor, AnyMap& params) override;
  virtual StateResult do_abort_work(ThingRef actor, AnyMap& params) override;
};
