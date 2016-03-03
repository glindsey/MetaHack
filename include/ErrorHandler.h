#ifndef ERRORHANDLER_H_INCLUDED
#define ERRORHANDLER_H_INCLUDED

#include "stdafx.h"

#ifndef __FUNCTION_NAME__
#ifdef WIN32   //WINDOWS
#define __FUNCTION_NAME__   __FUNCTION__
#define snprintf _snprintf_s
#else          //*NIX
#define __FUNCTION_NAME__   __func__
#endif
#endif

#define TRACE(...)                                                        \
{                                                                         \
  char buf[1024];                                                         \
  snprintf(buf, 1024, __VA_ARGS__);                                       \
  ErrorHandler::instance().printTrace(buf, __FILE__, __LINE__,            \
                                      __FUNCTION_NAME__);                 \
}

#define LUA_TRACE(...)                                                    \
{                                                                         \
  char buf[1024];                                                         \
  snprintf(buf, 1024, __VA_ARGS__);                                       \
  ErrorHandler::instance().printTrace(buf, __FILE__, __LINE__,            \
                                      __FUNCTION_NAME__);                 \
}

#define MINOR_ERROR(...)                                                  \
{                                                                         \
  char buf[1024];                                                         \
  snprintf(buf, 1024, __VA_ARGS__);                                       \
  ErrorHandler::instance().handleMinorError(buf,                          \
                                             __FILE__,                    \
                                             __LINE__,                    \
                                             __FUNCTION_NAME__);          \
}

#define MAJOR_ERROR(...)                                                  \
{                                                                         \
  char buf[1024];                                                         \
  snprintf(buf, 1024, __VA_ARGS__);                                       \
  ErrorHandler::instance().handleMajorError(buf,                          \
                                             __FILE__,                    \
                                             __LINE__,                    \
                                             __FUNCTION_NAME__);          \
}

#define FATAL_ERROR(...)                                                  \
{                                                                         \
  char buf[1024];                                                         \
  snprintf(buf, 1024, __VA_ARGS__);                                       \
  ErrorHandler::instance().handleFatalError(buf,                          \
                                             __FILE__,                    \
                                             __LINE__,                    \
                                             __FUNCTION_NAME__);          \
}

#ifndef NDEBUG

#define ASSERT_NOT_NULL(ptr)                                              \
{                                                                         \
  if (ptr == nullptr)                                                     \
  {                                                                       \
    FATAL_ERROR("Invalid null pointer: \"" #ptr "\"");                    \
  }                                                                       \
}                                                                         \

#define ASSERT_CONDITION(condition)                                       \
{                                                                         \
  if (!(condition))                                                       \
  {                                                                       \
    FATAL_ERROR("Assertion failed: \"" #condition "\"");                  \
  }                                                                       \
}                                                                         \

#else
#define ASSERT_NOT_NULL(ptr)
#define ASSERT_CONDITION(condition)
#endif

class ErrorHandler :
  public boost::noncopyable
{
public:
  static ErrorHandler& instance();
  virtual ~ErrorHandler();

  void printTrace(char* buf,
                  char const* file,
                  int line,
                  char const* func);

  void handleMinorError(char* buf,
                        char const* file,
                        int line,
                        char const* func);

  void handleMajorError(char* buf,
                        char const* file,
                        int line,
                        char const* func);

  void handleFatalError(char* buf,
                        char const* file,
                        int line,
                        char const* func);

  void setErrorHandlingFlags(bool minor, bool major);

protected:
private:
  ErrorHandler();

  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // ERRORMACROS_H_INCLUDED
