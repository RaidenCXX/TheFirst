#pragma once

#include <string>

class Action
{
  std::string m_name      = "None";
  bool        m_type      = false;
  bool        m_inputType = true;     // If true input from Keyboard, if false input from joystick 
  public:
  Action(const std::string name, bool type);
  Action(const Action& action);
  Action(Action&& action);


  const std::string&  getName() const;
  const bool          getType() const;
};
