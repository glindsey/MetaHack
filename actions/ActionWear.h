#pragma once

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"
#include "types/BodyPart.h"

namespace Actions
{
  class ActionWear
    :
    public Action, public ActionCRTP<ActionWear>
  {
  private:
    ActionWear();
  public:
    explicit ActionWear(EntityId subject);
    virtual ~ActionWear();
    static ActionWear prototype;

    virtual ReasonBool subjectIsCapable(GameState const& gameState) const override;
    //virtual ReasonBool objectIsAllowed(GameState const& gameState) const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments) override;
    virtual StateResult doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments) override;
    virtual StateResult doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments) override;
    virtual StateResult doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments) override;

  private:
    BodyLocation m_bodyLocation;
  };

} // end namespace
