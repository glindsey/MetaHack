#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"

namespace Actions
{
  class ActionUse
    :
    public Action, public ActionCRTP<ActionUse>
  {
  private:
    ActionUse();
  public:
    explicit ActionUse(EntityId subject);
    virtual ~ActionUse();
    static ActionUse prototype;

    virtual ReasonBool subjectIsCapable(GameState const& gameState) const override;
    virtual ReasonBool objectIsAllowed(GameState const& gameState) const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(GameState& gameState, SystemManager& systems) override;
    virtual StateResult doBeginWorkNVI(GameState& gameState, SystemManager& systems) override;
    virtual StateResult doFinishWorkNVI(GameState& gameState, SystemManager& systems) override;
    virtual StateResult doAbortWorkNVI(GameState& gameState, SystemManager& systems) override;
  };

} // end namespace