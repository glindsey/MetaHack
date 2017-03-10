#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "EntityId.h"

/// Action subclass describing writing on something.
/// ActionInscribe is a little backwards due to the limitations of the
/// class system. It is taken to mean:
///     `subject` writes using finger on `target` (for writing in dust)
///     `subject` writes using `object` on `target` (for writing with item)
namespace Actions
{
  class ActionInscribe
    :
    public Action, public ActionCRTP<ActionInscribe>
  {
  private:
    ActionInscribe();
  public:
    explicit ActionInscribe(EntityId subject);
    virtual ~ActionInscribe();
    static ActionInscribe prototype;

    virtual std::unordered_set<Action::Trait> const& getTraits() const override;

  protected:
    virtual StateResult do_prebegin_work_(AnyMap& params) override;
    virtual StateResult do_begin_work_(AnyMap& params) override;
    virtual StateResult do_finish_work_(AnyMap& params) override;
    virtual StateResult do_abort_work_(AnyMap& params) override;
  };

} // end namespace