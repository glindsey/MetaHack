#pragma once

#include "Action.h"

class ActionWait
  :
  public Action
{
public:
  ActionWait();
  virtual ~ActionWait();

  virtual bool target_can_be_thing() const override;
  virtual bool target_can_be_direction() const override;

protected:
  virtual StateResult do_prebegin_work(ThingRef actor, AnyMap& params) override;
  virtual StateResult do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params) override;
  virtual StateResult do_finish_work(ThingRef actor, AnyMap& params) override;
  virtual StateResult do_abort_work(ThingRef actor, AnyMap& params) override;
};
