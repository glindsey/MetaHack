#pragma once

#include "Action.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionPutInto
  :
  public Action
{
  /// @todo Handle putting multiple items into a thing. Right now only the
  ///       first item is processed.
public:
  ActionPutInto(ThingRef subject, ThingRef object);
  ActionPutInto(ThingRef subject, std::vector<ThingRef> objects);
  virtual ~ActionPutInto();

  virtual std::string get_type() const override
  {
    return "PutInto";
  }

  ACTION_TRAIT(can_be_subject_verb_thing_preposition_thing);
  ACTION_TRAIT(can_be_subject_verb_things_preposition_thing);

protected:
  virtual StateResult do_prebegin_work(AnyMap& params) override;
  virtual StateResult do_begin_work(AnyMap& params) override;
  virtual StateResult do_finish_work(AnyMap& params) override;
  virtual StateResult do_abort_work(AnyMap& params) override;
};
