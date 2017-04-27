#pragma once

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"

#include <string>
#include <vector>

namespace Actions
{
  class ActionMove
    :
    public Action, public ActionCRTP<ActionMove>
  {
  private:
    ActionMove();
  public:
    explicit ActionMove(EntityId subject);
    virtual ~ActionMove();
    static ActionMove prototype;

    virtual bool subjectIsCapable() const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doFinishWorkNVI(AnyMap& params) override;
    virtual StateResult doAbortWorkNVI(AnyMap& params) override;
  };

} // end namespace