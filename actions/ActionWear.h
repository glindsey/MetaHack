#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"
#include "types/BodyPart.h"

namespace Actions
{
  class ActionWear
    :
    public Action, public ActionCRTP<ActionWear>
  {
  private:
    ActionWear();
  public:
    explicit ActionWear(EntityId subject);
    virtual ~ActionWear();
    static ActionWear prototype;

    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult do_prebegin_work_(AnyMap& params) override;
    virtual StateResult do_begin_work_(AnyMap& params) override;
    virtual StateResult do_finish_work_(AnyMap& params) override;
    virtual StateResult do_abort_work_(AnyMap& params) override;

  private:
    BodyLocation m_body_location;
  };

} // end namespace
