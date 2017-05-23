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
    virtual StateResult doPreBeginWorkNVI(GameState& gameState, AnyMap& params) override;
    virtual StateResult doBeginWorkNVI(GameState& gameState, AnyMap& params) override;
    virtual StateResult doFinishWorkNVI(GameState& gameState, AnyMap& params) override;
    virtual StateResult doAbortWorkNVI(GameState& gameState, AnyMap& params) override;

    virtual void printMessageTry() const override;
    virtual void printMessageDo() const override;
  };
 
} // end namespace