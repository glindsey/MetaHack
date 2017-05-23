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

    virtual ReasonBool subjectIsCapable(GameState const& gameState) const override;
    //virtual ReasonBool objectIsAllowed(GameState const& gameState) const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(GameState& gameState) override;
    virtual StateResult doBeginWorkNVI(GameState& gameState) override;
    virtual StateResult doFinishWorkNVI(GameState& gameState) override;
    virtual StateResult doAbortWorkNVI(GameState& gameState) override;
  };

} // end namespace