#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"
#include "types/BodyPart.h"

namespace Actions
{
  class ActionWield
    :
    public Action, public ActionCRTP<ActionWield>
  {
  private:
    ActionWield();
  public:
    explicit ActionWield(EntityId subject);
    virtual ~ActionWield();
    static ActionWield prototype;

    virtual ReasonBool subjectIsCapable(GameState const& gameState) const override;
    //virtual ReasonBool objectIsAllowed(GameState const& gameState) const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(GameState& gameState, SystemManager& systems) override;
    virtual StateResult doBeginWorkNVI(GameState& gameState, SystemManager& systems) override;
    virtual StateResult doFinishWorkNVI(GameState& gameState, SystemManager& systems) override;
    virtual StateResult doAbortWorkNVI(GameState& gameState, SystemManager& systems) override;

  private:
    BodyLocation m_bodyLocation;
  };

} // end namespace