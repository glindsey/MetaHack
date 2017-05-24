#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"

namespace Actions
{
  class ActionUnwield
    :
    public Action, public ActionCRTP<ActionUnwield>
  {
  private:
    ActionUnwield();
  public:
    explicit ActionUnwield(EntityId subject);
    virtual ~ActionUnwield();
    static ActionUnwield prototype;

    virtual ReasonBool subjectIsCapable(GameState const& gameState) const override;
    //virtual ReasonBool objectIsAllowed(GameState const& gameState) const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(GameState& gameState, SystemManager& systems, json& arguments) override;
    virtual StateResult doBeginWorkNVI(GameState& gameState, SystemManager& systems, json& arguments) override;
    virtual StateResult doFinishWorkNVI(GameState& gameState, SystemManager& systems, json& arguments) override;
    virtual StateResult doAbortWorkNVI(GameState& gameState, SystemManager& systems, json& arguments) override;
  };

} // end namespace