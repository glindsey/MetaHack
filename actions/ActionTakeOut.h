#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"

/// Action class for taking an item out of another item.
/// Why does this item have the "object can be out of reach" trait? Well, the
/// object is inside a container, so it is technically "out of reach" as far
/// as the game engine is concerned, until it is removed from the container.
/// We DO check if the CONTAINER is within reach, which is what really matters.
namespace Actions
{
  class ActionTakeOut
    :
    public Action, public ActionCRTP<ActionTakeOut>
  {
  private:
    ActionTakeOut();
  public:
    explicit ActionTakeOut(EntityId subject);
    virtual ~ActionTakeOut();
    static ActionTakeOut prototype;

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