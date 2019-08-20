#ifndef APPSTATE_H
#define APPSTATE_H

#include "types/IRenderable.h"
#include "state_machine/State.h"

// Forward declarations
class StateMachine;

class AppState :
  public State
{
public:
  AppState(StateMachine& state_machine,
           std::unordered_set<EventID> const events,
           std::string name);

  virtual ~AppState();

  virtual bool render(sf::RenderTexture& texture, int frame) override;

protected:
private:
};

#endif // APPSTATE_H
