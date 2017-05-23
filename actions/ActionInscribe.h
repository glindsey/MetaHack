#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "entity/EntityId.h"

/// Action subclass describing writing on something.
/// ActionInscribe is a little backwards due to the limitations of the
/// class system. It is taken to mean:
///     `subject` writes using finger on `target` (for writing in dust)
///     `subject` writes using `object` on `target` (for writing with item)
namespace Actions
{
  class ActionInscribe
    :
    public Action, public ActionCRTP<ActionInscribe>
  {
  private:
    ActionInscribe();
  public:
    explicit ActionInscribe(EntityId subject);
    virtual ~ActionInscribe();
    static ActionInscribe prototype;

    //virtual ReasonBool subjectIsCapable(GameState const& gameState) const override;
    //virtual ReasonBool objectIsAllowed(GameState const& gameState) const override;
    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(GameState& gameState, AnyMap& params) override;
    virtual StateResult doBeginWorkNVI(GameState& gameState, AnyMap& params) override;
    virtual StateResult doFinishWorkNVI(GameState& gameState, AnyMap& params) override;
    virtual StateResult doAbortWorkNVI(GameState& gameState, AnyMap& params) override;
  };

} // end namespace