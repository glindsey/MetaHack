#pragma once

#include <memory>

#include <boost/ptr_container/ptr_deque.hpp>
#include <queue>

#include "json.hpp"
using json = ::nlohmann::json;

namespace Actions
{

  // Forward declarations
  class Action;

  class ActionQueue
  {
  public:
    ActionQueue();
    ~ActionQueue();

    friend void from_json(json const& j, ActionQueue& obj);
    friend void to_json(json& j, ActionQueue const& obj);

    /// Push an Action onto the queue.
    void push(std::unique_ptr<Action>&& action);

    /// Push an Action onto the queue.
    /// @note The queue *takes ownership* of the Action.
    void push(Action* action);

    Action& front();
    Action const& front() const;
    void pop();
    void clear();
    bool empty() const;
    size_t size() const;

  protected:

  private:
    boost::ptr_deque<Action> m_actions;
  };

} // end namespace