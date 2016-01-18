#pragma once

#include "Action.h"
#include <string>

class ActionShoot
  :
  public Action
{
public:
  ActionShoot();
  virtual ~ActionShoot();

  virtual bool target_can_be_direction() const override
  {
    return true;
  }

  virtual std::string get_type() const
  {
    return "Shoot";
  }

protected:
  virtual StateResult do_prebegin_work(ThingRef actor, AnyMap& params) override;
  virtual StateResult do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params) override;
  virtual StateResult do_finish_work(ThingRef actor, AnyMap& params) override;
  virtual StateResult do_abort_work(ThingRef actor, AnyMap& params) override;
};
