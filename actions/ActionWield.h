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

    virtual ReasonBool subjectIsCapable(GameState& gameState) const override;
    //virtual ReasonBool objectIsAllowed(GameState& gameState) const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(GameState& gameState, AnyMap& params) override;
    virtual StateResult doBeginWorkNVI(GameState& gameState, AnyMap& params) override;
    virtual StateResult doFinishWorkNVI(GameState& gameState, AnyMap& params) override;
    virtual StateResult doAbortWorkNVI(GameState& gameState, AnyMap& params) override;

  private:
    BodyLocation m_bodyLocation;
  };

} // end namespace