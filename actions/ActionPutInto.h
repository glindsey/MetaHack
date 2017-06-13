#pragma once

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
    virtual StateResult doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments) override;
    virtual StateResult doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments) override;
    virtual StateResult doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments) override;
    virtual StateResult doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments) override;

    virtual void printMessageTry(Systems::Manager& systems, json& arguments) const override;
    virtual void printMessageDo(Systems::Manager& systems, json& arguments) const override;
  };
 
} // end namespace