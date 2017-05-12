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

    void push(std::unique_ptr<Action>&& action);
    Action& front();
    Action const& front() const;
    void pop();
    void clear();
    bool empty();
    size_t size();

  protected:

  private:
    boost::ptr_deque<Action> m_actions;
  };

} // end namespace