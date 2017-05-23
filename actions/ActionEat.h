#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"

namespace Actions
{
  class ActionEat
    :
    public Action, public ActionCRTP<ActionEat>
  {
  private:
    ActionEat();
  public:
    explicit ActionEat(EntityId subject);
    virtual ~ActionEat();
    static ActionEat prototype;

    //virtual ReasonBool subjectIsCapable(GameState const& gameState) const override;
    //virtual ReasonBool objectIsAllowed(GameState const& gameState) const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(GameState& gameState, SystemManager& systems) override;
    virtual StateResult doBeginWorkNVI(GameState& gameState, SystemManager& systems) override;
    virtual StateResult doFinishWorkNVI(GameState& gameState, SystemManager& systems) override;
    virtual StateResult doAbortWorkNVI(GameState& gameState, SystemManager& systems) override;

  private:
    /// The status of the beObjectOf() call, which needs to be saved
    /// for the "finish" portion of the action.
    bool m_last_eat_result;
  };

} // end namespace