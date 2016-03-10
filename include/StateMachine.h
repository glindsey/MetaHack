#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "stdafx.h"

#include "Renderable.h"
#include "SFMLEventHandler.h"

// Forward declarations
class State;

// Implementation of a state machine that can change between states derived
// from the State abstract class.
class StateMachine :
  virtual public RenderableToTexture,
  virtual public SFMLEventHandler,
  virtual public boost::noncopyable
{
public:
  explicit StateMachine(StringKey const& machine_name);
  virtual ~StateMachine();

  /// Get the name of this state machine.
  StringKey const& get_name();

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
  bool delete_state(StringKey const& state_name);

  /// Executes the current state.
  /// If no state is active, does nothing and returns false.
  void execute();

  /// Renders the current state on screen.
  /// @param target Target to render to.
  /// @param frame Frame number, used for animations
  /// @return True if we could render, false otherwise.
  bool render(sf::RenderTexture& texture, int frame) override;

  /// Pass an event on to the current state.
  /// If no state is active, does nothing and returns SFMLEventResult::Ignored.
  SFMLEventResult handle_sfml_event(sf::Event& event) override;

  /// Try to change to a new state.
  /// If nullptr is passed, attempts to terminate existing state only; this is
  /// used by the state machine destructor.
  /// @param new_state Pointer to new state to change to.
  /// @return True if we could change to that state, false otherwise.
  bool change_to(State* new_state);

  /// Try to change to a new state.
  /// @param new_state_name Name of new state to change to.
  /// @return True if we could change to that state, false otherwise.
  bool change_to(StringKey const& new_state_name);

  /// Get a pointer to the current state.
  /// @return Current state, or nullptr if there is no current state.
  State* get_current_state();

  /// Get the name of the current state.
  /// @return Name of current state, or "(none)" if no state is current.
  StringKey const& get_current_state_name();

protected:
private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // STATEMACHINE_H
