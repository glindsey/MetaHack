#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <string>

#include "EventHandler.h"
#include "Renderable.h"

// Forward declarations
class State;

// Implementation of a state machine that can change between states derived
// from the State abstract class.
class StateMachine :
  virtual public EventHandler,
  virtual public Renderable,
  virtual public boost::noncopyable
{
public:
  explicit StateMachine(std::string const& machine_name);
  virtual ~StateMachine();

  /// Get the name of this state machine.
  std::string const& get_name();

  /// Add a state to the machine.
  /// The new state must be allocated on the heap, as the ptr_map will take
  /// ownership of it when it is added.
  /// @param state Pointer to heap-allocated state to add to the machine.
  /// @return True if the state could be added, false otherwise.  <b>If the
  ///         state could not be added to the machine the instance will be
  ///         deleted.</b>
  bool add_state(State* state);

  /// Delete a state from the machine, by pointer.
  /// This function will fail if the requested state is the current one.
  /// @param state Pointer to state to delete.
  /// @return True if the state was found and deleted, false otherwise.
  bool delete_state(State* state);

  /// Delete a state from the machine, by name.
  /// This function will fail if the requested state is the current one.
  /// @param state_name Name of the state to delete.
  /// @return True if the state was found and deleted, false otherwise.
  bool delete_state(std::string const& state_name);

  /// Executes the current state.
  /// If no state is active, does nothing and returns false.
  void execute();

  /// Renders the current state on screen.
  /// @param target Target to render to.
  /// @param frame Frame number, used for animations
  /// @return True if we could render, false otherwise.
  bool render(sf::RenderTarget& target, int frame) override;

  /// Pass an event on to the current state.
  /// If no state is active, does nothing and returns EventResult::Ignored.
  EventResult handle_event(sf::Event& event) override;

  /// Try to change to a new state.
  /// If nullptr is passed, attempts to terminate existing state only; this is
  /// used by the state machine destructor.
  /// @param new_state Pointer to new state to change to.
  /// @return True if we could change to that state, false otherwise.
  bool change_to(State* new_state);

  /// Try to change to a new state.
  /// @param new_state_name Name of new state to change to.
  /// @return True if we could change to that state, false otherwise.
  bool change_to(std::string const& new_state_name);

  /// Get a pointer to the current state.
  /// @return Current state, or nullptr if there is no current state.
  State* get_current_state();

  /// Get the name of the current state.
  /// @return Name of current state, or "(none)" if no state is current.
  std::string const& get_current_state_name();

protected:
private:
  struct Impl;
  boost::scoped_ptr<Impl> pImpl;
};

#endif // STATEMACHINE_H
