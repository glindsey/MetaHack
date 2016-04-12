#include "stdafx.h"

#include "LuaCalls.h"

template <> void push_value_to_lua(unsigned int value)
{
  lua_pushinteger(the_lua_state, static_cast<lua_Integer>(value));
}

template <> void push_value_to_lua(int value)
{
  lua_pushinteger(the_lua_state, static_cast<lua_Integer>(value));
}

template <> void push_value_to_lua(float value)
{
  lua_pushnumber(the_lua_state, static_cast<lua_Number>(value));
}

template <> void push_value_to_lua(double value)
{
  lua_pushnumber(the_lua_state, static_cast<lua_Number>(value));
}

template <> void push_value_to_lua(bool value)
{
  lua_pushboolean(the_lua_state, static_cast<int>(value));
}

template <> void push_value_to_lua(std::string value)
{
  lua_pushstring(the_lua_state, value.c_str());
}

template <> void push_value_to_lua(sf::Vector2u value)
{
  lua_pushnumber(the_lua_state, static_cast<lua_Integer>(value.x));
  lua_pushnumber(the_lua_state, static_cast<lua_Integer>(value.y));
}

template <> void push_value_to_lua(sf::Color value)
{
  lua_pushnumber(the_lua_state, static_cast<lua_Integer>(value.r));
  lua_pushnumber(the_lua_state, static_cast<lua_Integer>(value.g));
  lua_pushnumber(the_lua_state, static_cast<lua_Integer>(value.b));
  lua_pushnumber(the_lua_state, static_cast<lua_Integer>(value.a));
}

template <> unsigned int pop_value_from_lua()
{
  unsigned int return_value = lua_tointeger(the_lua_state, -1);
  lua_pop(the_lua_state, 1);
  return return_value;
}

template <> int pop_value_from_lua()
{
  int return_value = lua_tointeger(the_lua_state, -1);
  lua_pop(the_lua_state, 1);
  return return_value;
}

template <> float pop_value_from_lua()
{
  float return_value = static_cast<float>(lua_tonumber(the_lua_state, -1));
  lua_pop(the_lua_state, 1);
  return return_value;
}

template <> double pop_value_from_lua()
{
  double return_value = lua_tonumber(the_lua_state, -1);
  lua_pop(the_lua_state, 1);
  return return_value;
}

template <> bool pop_value_from_lua()
{
  bool return_value = (lua_toboolean(the_lua_state, -1) != 0);
  lua_pop(the_lua_state, 1);
  return return_value;
}

template <> std::string pop_value_from_lua()
{
  std::string return_value = std::string(lua_tostring(the_lua_state, -1));
  lua_pop(the_lua_state, 1);
  return return_value;
}

template <> sf::Vector2u pop_value_from_lua()
{
  sf::Vector2u return_value = sf::Vector2u(lua_tointeger(the_lua_state, -2),
                                           lua_tointeger(the_lua_state, -1));
  lua_pop(the_lua_state, 2);
  return return_value;
}

template <> sf::Color pop_value_from_lua()
{
  sf::Color return_value = sf::Color(lua_tointeger(the_lua_state, -4),
                                     lua_tointeger(the_lua_state, -3),
                                     lua_tointeger(the_lua_state, -2),
                                     lua_tointeger(the_lua_state, -1));
  lua_pop(the_lua_state, 4);
  return return_value;
}

template <> ActionResult pop_value_from_lua()
{
  ActionResult return_value = static_cast<ActionResult>(the_lua_instance.get_enum_value(-1));
  lua_pop(the_lua_state, 1);
  return return_value;
}

template<> unsigned int lua_stack_slots<unsigned int>() { return 1; }
template<> unsigned int lua_stack_slots<int>() { return 1; }
template<> unsigned int lua_stack_slots<float>() { return 1; }
template<> unsigned int lua_stack_slots<double>() { return 1; }
template<> unsigned int lua_stack_slots<std::string>() { return 1; }
template<> unsigned int lua_stack_slots<sf::Vector2u>() { return 2; }
template<> unsigned int lua_stack_slots<sf::Color>() { return 4; }
template<> unsigned int lua_stack_slots<ActionResult>() { return 1; }