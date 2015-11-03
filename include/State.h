#ifndef STATE_H
#define STATE_H

#include <boost/noncopyable.hpp>
#include <memory>
#include <string>

#include "EventHandler.h"
#include "Renderable.h"

// Forward declarations
class StateMachine;

class State :
  virtual public EventHandler,
  virtual public Renderable,
  virtual public boost::noncopyable
{
  public:
    State(StateMachine& state_machine);
    virtual ~State();

    // Get the name of this state.
    virtual std::string const& get_name() = 0;

    // Initialize the state upon entering it.
    virtual bool initialize() = 0;

    // Execute while in the state.
    virtual void execute() = 0;

    // Terminate the state upon leaving it.
    virtual bool terminate() = 0;

    // Tell state machine to change to a new state.
    bool change_to(std::string const& new_state);

  protected:
  private:
    // State machine that this state belongs to.
    StateMachine& m_state_machine;
};

#endif // STATE_H
