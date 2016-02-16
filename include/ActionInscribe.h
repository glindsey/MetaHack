#pragma once

#include "Action.h"
#include "ThingRef.h"

#include <string>
#include <vector>

/// Action subclass describing writing on something.
/// ActionInscribe is a little backwards due to the limitations of the
/// class system. It is taken to mean:
///     `subject` writes using finger on `target` (for writing in dust)
///     `subject` writes using `object` on `target` (for writing with item)
class ActionInscribe
  :
  public Action
{
public:
  explicit ActionInscribe(ThingRef subject);
  ActionInscribe(ThingRef subject, ThingRef object);
  virtual ~ActionInscribe();

  virtual std::string get_type() const override
  {
    return "Inscribe";
  }

  ACTION_TRAIT(can_be_subject_verb_direction);
  ACTION_TRAIT(can_be_subject_verb_thing_preposition_thing);
  ACTION_TRAIT(can_be_subject_verb_thing_preposition_direction);

protected:
  virtual StateResult do_prebegin_work(AnyMap& params) override;
  virtual StateResult do_begin_work(AnyMap& params) override;
  virtual StateResult do_finish_work(AnyMap& params) override;
  virtual StateResult do_abort_work(AnyMap& params) override;
};
