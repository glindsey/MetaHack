#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"

namespace Actions
{
  class ActionPutInto
    :
    public Action, public ActionCRTP<ActionPutInto>
  {
  private:
    ActionPutInto();
  public:
    explicit ActionPutInto(EntityId subject);
    virtual ~ActionPutInto();
    static ActionPutInto prototype;

    virtual ReasonBool subjectIsCapable(GameState const& gameState) const override;
    //virtual ReasonBool objectIsAllowed(GameState const& gameState) const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(GameState& gameState, SystemManager& systems) override;
    virtual StateResult doBeginWorkNVI(GameState& gameState, SystemManager& systems) override;
    virtual StateResult doFinishWorkNVI(GameState& gameState, SystemManager& systems) override;
    virtual StateResult doAbortWorkNVI(GameState& gameState, SystemManager& systems) override;

    virtual void printMessageTry() const override;
    virtual void printMessageDo() const override;
  };
 
} // end namespace