#ifndef AISTRATEGY_H
#define AISTRATEGY_H

#include <memory>

// Forward declarations
class Thing;

/// Pure virtual class representing an AI strategy.
class AIStrategy
{
  public:
    AIStrategy(Thing* thing_ptr);
    virtual ~AIStrategy();

    /// Execute this AI strategy.
    /// Analyzes the surroundings and queues Actions as appropriate.
    virtual void execute() = 0;

  protected:

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif // AISTRATEGY_H
