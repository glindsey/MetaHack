#ifndef STATE_H
#define STATE_H



#include "Object.h"
#include "types/IRenderable.h"

// Forward declarations
class StateMachine;

class State :
  public RenderableToTexture,
  public Object
{
public:
  State(StateMachine& state_machine,
        std::unordered_set<EventID> const events,
        std::string name);
  State(State const&) = delete;
  State(State&&) = delete;
  State& operator=(State const&) = delete;
  State& operator=(State&&) = delete;

  virtual ~State();

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
  StateMachine& m_stateMachine;
};

#endif // STATE_H
