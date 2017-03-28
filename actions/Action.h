#pragma once

#include "stdafx.h"

#include "game/App.h"
#include "types/Direction.h"
#include "game/GameState.h"
#include "services/MessageLog.h"
#include "entity/EntityId.h"

namespace Actions
{
  // === FORWARD DECLARATIONS ===================================================
  class Action;
  class Entity;

  // === USING DECLARATIONS =====================================================
  using ActionCreator = std::function<std::unique_ptr<Action>(EntityId)>;
  using ActionMap = std::unordered_map<std::string, ActionCreator>;
  
  /// Struct containing the results of an action state.
  struct StateResult
  {
    static StateResult Success(unsigned int time = 0)
    {
      return{ true, time };
    }

    static StateResult Failure(unsigned int time = 0)
    {
      return{ false, time };
    }

    bool success;
    unsigned int elapsed_time;
  };

  /// Enum indicating the state of a particular action.
  enum class State
  {
    Pending,      ///< The action is waiting to be executed.
    PreBegin,     ///< Wait state prior to starting the action, to account for reaction time.
    InProgress,   ///< The action has been started and is currently in progress.
    Interrupted,  ///< The action was interrupted while in process and will be aborted.
    PostFinish,   ///< The action is finished and the entity is now in a recovery wait state.
    Processed     ///< The action is totally done and can be popped off the queue.
  };

  /// Possible action traits.
  enum class Trait
  {
    CanBeSubjectOnly,
    CanBeSubjectVerbObject,
    CanBeSubjectVerbBodypart,
    CanBeSubjectVerbDirection,
    CanBeSubjectVerbObjects,
    CanBeSubjectVerbObjectPrepositionTarget,
    CanBeSubjectVerbObjectsPrepositionTarget,
    CanBeSubjectVerbObjectPrepositionBodypart,
    CanBeSubjectVerbObjectPrepositionDirection,
    CanTakeAQuantity,
    SubjectCanBeInLimbo,
    SubjectCanNotBeInsideAnotherObject,
    SubjectMustBeAbleToMove,
    ObjectMustBeWielded,
    ObjectMustBeWorn,
    ObjectMustNotBeWielded,
    ObjectMustNotBeWorn,
    ObjectCanBeOutOfReach,
    ObjectCanBeSelf,
    ObjectMustBeEmpty,
    ObjectMustBeMovableBySubject,
    ObjectMustBeInInventory,
    ObjectMustNotBeEmpty,
    ObjectMustNotBeInInventory,
    ObjectMustBeLiquidCarrier,
    MemberCount
  };

  /// Class describing an action to execute.
  class Action
  {
  public:
    explicit Action(EntityId subject, std::string type, std::string verb);
    Action(Action const&) = delete;
    Action(Action&&) = delete;
    Action& operator=(Action const&) = delete;
    Action& operator=(Action&&) = delete;
    virtual ~Action();

    virtual std::unordered_set<Trait> const& getTraits() const;

    bool hasTrait(Trait trait) const;

    EntityId getSubject() const;

    void setObject(EntityId object);
    void setObjects(std::vector<EntityId> objects);

    std::vector<EntityId> const& getObjects() const;
    EntityId getObject() const;
    EntityId getSecondObject() const;

    bool process(AnyMap params);

    void setState(State state);
    State getState();

    void setTarget(EntityId entity) const;
    void setTarget(Direction direction) const;
    void setQuantity(unsigned int quantity) const;

    EntityId getTargetThing() const;
    Direction getTargetDirection() const;
    unsigned int getQuantity() const;

    std::string getType() const;

    /// Return the first-/second-person singular form of the verb to be performed.
    /// @todo English doesn't generally distinguish between 1st/2nd person
    ///       for past tense, but some other languages do. Assuming, of course,
    ///       we even NEED the 1st person conjugation.
    std::string getVerb2() const;

    /// Return the third-person singular form of the verb to be performed.
    std::string getVerb3() const;

    /// Return the present participle form of the verb to be performed.
    std::string getVerbing() const;

    /// Return the past form of the verb to be performed.
    /// @todo English doesn't generally distinguish between 2nd/3rd person
    ///       for past tense, but some other languages do.
    std::string getVerbed() const;

    /// Return the past participle form of the verb to be performed.
    std::string getVerbPP() const;

    /// Return the adjective form of the verb to be performed.
    /// (Obviously, some verbs don't make sense when conjugated in this way, 
    /// but the option is there nonetheless.)
    std::string getVerbable() const;

    /// A static method that registers an action subclass in a database.
    /// Required so that Lua scripts can instantiate Actions on Entities.
    static void registerActionAs(std::string key, ActionCreator creator);

    /// A static method that checks if a key exists.
    static bool exists(std::string key);

    /// A static method that returns an Action associated with a key.
    /// If the requested key does not exist, throws an exception.
    static std::unique_ptr<Action> create(std::string key, EntityId subject);

    std::string maketr(std::string key) const;
    std::string maketr(std::string key, std::vector<std::string> optional_strings) const;

    /// A method for composing a string from a pattern for an action.
    std::string makeString(std::string pattern, std::vector<std::string> optional_strings) const;
    std::string makeString(std::string pattern) const;

    /// Get a const reference to the action map.
    static ActionMap const& getMap();

  protected:
    /// An Action without a subject; used for prototype registration only.
    Action(std::string type, std::string verb, ActionCreator creator);

    /// Perform work to be done at the start of the PreBegin state.
    /// This work typically consists of checking whether the action is possible.
    /// If StateResult::success is false, the action was not possible and is cancelled.
    /// If StateResult::success is true, the action is possible, and the target actor
    /// is busy for StateResult::elapsed_time before moving to the InProgress state.
    /// @note doPreBeginWork usually has multiple returns in order to avoid
    ///       the Dreaded Pyramid of Doom. It's just easier that way.
    /// @param params Map of parameters for the Action.
    /// @return StateResult indicating whether the Action continues.
    StateResult doPreBeginWork(AnyMap& params);

    /// Perform work to be done at the start of the InProgress state.
    /// This is where the action begins.
    /// If StateResult::success is false, the action could not begin and is cancelled.
    /// (However, the PreBegin reaction time has already elapsed, so this is more
    ///  like a critical failure when attempting the action, unlike a failure in
    ///   doPreBeginWork which just means the action couldn't be started at all.)
    /// If StateResult::success is true, the action began, and the target actor is
    /// busy for StateResult::elapsed_time while the action is performed. It then
    /// moves to the PostFinish state.
    /// @param params Map of parameters for the Action.
    /// @return StateResult indicating whether the Action continues.
    StateResult doBeginWork(AnyMap& params);

    /// Perform work to be done at the end of the InProgress state and the start
    /// of the PostFinish state.
    /// This is where the action ends, and is used for actions that are not
    /// instantaneous in game time, such as eating, drinking, reading, etc.
    /// StateResult::success is ignored.
    /// The target actor is busy for StateResult::elapsed_time as a "refractory"
    /// period.
    /// @param params Map of parameters for the Action.
    /// @return StateResult indicating the post-Action wait time.
    StateResult doFinishWork(AnyMap& params);

    /// Perform work to be done when an action in the InProgress state is aborted.
    /// This is called when an action is aborted.
    /// StateResult::success is ignored.
    /// The target actor is busy for StateResult::elapsed_time as a "refractory"
    /// period.
    /// @param params Map of parameters for the Action.
    /// @return StateResult indicating the post-Action wait time.
    StateResult doAbortWork(AnyMap& params);

    /// Overridable portion of doPreBeginWork().
    /// @param params Map of parameters for the Action.
    /// @return StateResult indicating whether the Action continues.
    virtual StateResult doPreBeginWorkNVI(AnyMap& params);

    /// Overridable portion of doBeginWork().
    /// @param params Map of parameters for the Action.
    /// @return StateResult indicating whether the Action continues.
    virtual StateResult doBeginWorkNVI(AnyMap& params);

    /// Overridable portion of doFinishWork().
    /// @param params Map of parameters for the Action.
    /// @return StateResult indicating the post-Action wait time.
    virtual StateResult doFinishWorkNVI(AnyMap& params);

    /// Overridable portion of doAbortWork().
    /// @param params Map of parameters for the Action.
    /// @return StateResult indicating the post-Action wait time.
    virtual StateResult doAbortWorkNVI(AnyMap& params);

    /// Describes the object(s) or direction in terms of the subject.
    /// This string will vary based on the presence of objects or a direction
    /// for the action, following this order:
    ///   * If only one object is seen, returns " [OBJECT]."
    ///     - If object = subject, returns " [SUBJECT-REFLEXIVE-PRONOUN]."
    ///     - If the quantity is greater than 1, returns "[QUANTITY] of the [OBJECTS]".
    ///   * If more than one object, returns " the items."
    ///   * If a direction is present, returns " [DIRECTION]."
    ///   * If none of these are present, returns an empty string.
    /// @note The string returned has a leading space on it unless no objects or
    ///       direction are found, in order to make message composition cleaner.
    ///
    /// This method can be overridden if necessary to customize the description for a
    /// particular action.
    virtual std::string getObjectString() const;

    /// Describes the target in terms of the subject or object.
    /// This string will vary based on the presence of objects or a direction
    /// for the action, following this order:
    ///   * If the target equals the subject, returns "[SUBJECT-REFLEXIVE-PRONOUN]".
    ///   * If the target equals the object, returns "[OBJECT-REFLEXIVE-PRONOUN]".
    ///   * Otherwise returns the target description.
    ///
    /// This method can be overridden if necessary to customize the description for a
    /// particular action.
    virtual std::string getTargetString() const;

    /// Print a "[SUBJECT] try to [VERB]" message.
    /// The message will vary based on the presence of objects or a direction
    /// for the action, using get_object_string().
    /// "Try to" will be conjugated for the subject as "tries to" if needed.
    ///
    /// This method can be overridden if necessary to customize the message for a
    /// particular action.
    virtual void printMessageTry() const;

    /// Print a "[SUBJECT] [VERB]" message.
    /// The message will vary based on the presence of objects or a direction
    /// for the action, using get_object_string().
    /// VERB will be conjugated for the subject as needed.
    ///
    /// This method can be overridden if necessary to customize the message for a
    /// particular action.
    virtual void printMessageDo() const;

    /// Print a "[SUBJECT] begin to [VERB]" message.
    /// The message will vary based on the presence of objects or a direction
    /// for the action, using get_object_string().
    /// "Begin to" will be conjugated for the subject as "begins to" if needed.
    ///
    /// This method can be overridden if necessary to customize the message for a
    /// particular action.
    virtual void printMessageBegin() const;

    /// Print a "[SUBJECT] stop [VERBING]" message.
    /// The message will vary based on the presence of objects or a direction
    /// for the action, using get_object_string().
    /// "Stop" will be conjugated for the subject as "stops" if needed.
    ///
    /// This method can be overridden if necessary to customize the message for a
    /// particular action.
    virtual void printMessageStop() const;

    /// Print a "[SUBJECT] finish [VERBING]" message.
    /// The message will vary based on the presence of objects or a direction
    /// for the action, using get_object_string().
    /// "Finish" will be conjugated for the subject as "finishes" if needed.
    ///
    /// This method can be overridden if necessary to customize the message for a
    /// particular action.
    virtual void printMessageFinish() const;

    /// Print a "[SUBJECT] can't [VERB] that!" message.
    /// The message will vary based on the presence of objects or a direction
    /// for the action, using get_object_string().
    /// "Finish" will be conjugated for the subject as "finishes" if needed.
    ///
    /// @todo Finish implementing me, right now the default implementation is too simple.
    /// This method can be overridden if necessary to customize the message for a
    /// particular action.
    virtual void printMessageCant() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    static ActionMap action_map;
  };

  inline std::ostream& operator<<(std::ostream& os, State const& s)
  {
    switch (s)
    {
      case State::Pending:      os << "Pending"; break;
      case State::PreBegin:     os << "PreBegin"; break;
      case State::InProgress:   os << "InProgress"; break;
      case State::Interrupted:  os << "Interrupted"; break;
      case State::PostFinish:   os << "PostFinish"; break;
      case State::Processed:    os << "Processed"; break;
      default:                  os << "??? (" << static_cast<unsigned int>(s) << ")"; break;
    }

    return os;
  }
} // end namespace

/* Header template for subclasses: 

namespace Actions
{
  class ActionXXXX
  :
  public Action, public ActionCRTP<ActionXXXX>
  {
  private:
    ActionXXXX();
  public:
    explicit ActionXXXX(EntityId subject);
    virtual ~ActionXXXX();
    static ActionXXXX prototype;

    virtual std::unordered_set<Trait> const& getTraits() const override;

  protected:
    virtual StateResult doPreBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doBeginWorkNVI(AnyMap& params) override;
    virtual StateResult doFinishWorkNVI(AnyMap& params) override;
    virtual StateResult doAbortWorkNVI(AnyMap& params) override;
  };

} // end namespace
*/

/* Source boilerplate to copy to subclasses: 

ActionXXXX ActionXXXX::prototype;
ActionXXXX::ActionXXXX() : Action("xxxx", "XXXX", ActionXXXX::create_) {}
ActionXXXX::ActionXXXX(EntityId subject) : Action(subject, "xxxx", "XXXX") {}
ActionXXXX::~ActionXXXX() {}

std::unordered_set<Trait> const & ActionXXXX::getTraits() const
{
  static std::unordered_set<Trait> traits =
  {
    Trait::XXXX,
    Trait::YYYY
  };

  return traits;
}

*/