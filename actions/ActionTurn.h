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