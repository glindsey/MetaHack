#pragma once

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"

#include <string>
#include <vector>

/// Action: Turn to face a particular direction.
namespace Actions
{
  class ActionTurn
    :
    public Action, public ActionCRTP<ActionTurn>
  {
  private:
    ActionTurn();
  public:
    explicit ActionTurn(EntityId subject);
    virtual ~ActionTurn();
    static ActionTurn prototype;

    virtual ReasonBool subjectIsCapable() const override;
    //virtual ReasonBool objectIsAllowed() const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doFinishWorkNVI(AnyMap& params) override;
    virtual StateResult doAbortWorkNVI(AnyMap& params) override;
  };

} // end namespace