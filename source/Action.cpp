#include "Action.h"

Action::Action(const std::string name, bool type)
  :m_name(name), m_type(type)
{}

Action::Action(const Action& action)
{
  m_name = action.m_name;
}
Action::Action(Action&& action)
{
  m_name = std::move(action.m_name);
}


const std::string& Action::getName() const
{
  return m_name;
}

const bool Action::getType() const
{
  return m_type;
}
