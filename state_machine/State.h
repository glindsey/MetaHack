#ifndef STATE_H
#define STATE_H

#include "stdafx.h"

#include "Object.h"
#include "types/IRenderable.h"

// Forward declarations
class StateMachine;

class State :
  public RenderableToTexture,
  public Object
{
public:
  explicit State(StateMachine& state_machine,
                 std::unordered_set<EventID> const events);
  State(State const&) = delete;
  State(State&&) = delete;
  State& operator=(State const&) = delete;
  State& operator=(State&&) = delete;

  virtual ~State();

  // Get the name of this state.
  virtual std::string const& getName() = 0;

  // Initialize the state upon entering it.
  virtual bool initialize() = 0;

  // Execute while in the state.
  virtual void execute() = 0;

  // Terminate the state upon leaving it.
  virtual bool terminate() = 0;

  // Tell state machine to change to a new state.
  bool change_to(std::string const& new_state);

protected:
  // Get a reference to the state machine this state belongs to.
  StateMachine& getStateMachine();

private:
  // State machine that this state belongs to.
  StateMachine& m_state_machine;
};

#endif // STATE_H
