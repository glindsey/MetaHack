#include "stdafx.h"

#include "actions/ActionQueue.h"

#include "actions/Action.h"

namespace Actions
{

  ActionQueue::ActionQueue()
  {}

  ActionQueue::~ActionQueue()
  {}

  void ActionQueue::push(std::unique_ptr<Action>&& action)
  {
    m_actions.push_back(action.release());
  }

  void ActionQueue::push(Action* action)
  {
    m_actions.push_back(action);
  }

  Action& ActionQueue::front()
  {
    return m_actions.front();
  }

  Action const& ActionQueue::front() const
  {
    return m_actions.front();
  }

  void ActionQueue::pop()
  {
    m_actions.pop_front();
  }

  void ActionQueue::clear()
  {
    m_actions.clear();
  }

  bool ActionQueue::empty() const
  {
    return m_actions.empty();
  }

  size_t ActionQueue::size() const
  {
    return m_actions.size();
  }

  void from_json(json const& j, ActionQueue& obj)
  {
    obj = ActionQueue();
    /// @todo WRITE ME
  }

  void to_json(json& j, ActionQueue const& obj)
  {
    j = json::array();    
    /// @todo WRITE ME
  }

} // end namespace