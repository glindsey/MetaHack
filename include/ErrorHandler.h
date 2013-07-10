#ifndef ERRORHANDLER_H_INCLUDED
#define ERRORHANDLER_H_INCLUDED

#include <boost/noncopyable.hpp>
#include <cstdio>
#include <memory>

#define TRACE(...)                                                        \
{                                                                         \
  char buf[1024];                                                         \
  snprintf(buf, 1024, __VA_ARGS__);                                       \
  ErrorHandler::instance().printTrace(buf, __FILE__, __LINE__, __func__); \
}

#define MINOR_ERROR(...)                                                  \
{                                                                         \
  char buf[1024];                                                         \
  snprintf(buf, 1024, __VA_ARGS__);                                       \
  ErrorHandler::instance().handleMinorError(buf,                          \
                                             __FILE__,                    \
                                             __LINE__,                    \
                                             __func__);                   \
}

#define MAJOR_ERROR(...)                                                  \
{                                                                         \
  char buf[1024];                                                         \
  snprintf(buf, 1024, __VA_ARGS__);                                       \
  ErrorHandler::instance().handleMajorError(buf,                          \
                                             __FILE__,                    \
                                             __LINE__,                    \
                                             __func__);                   \
}

#define FATAL_ERROR(...)                                                  \
{                                                                         \
  char buf[1024];                                                         \
  snprintf(buf, 1024, __VA_ARGS__);                                       \
  ErrorHandler::instance().handleFatalError(buf,                          \
                                             __FILE__,                    \
                                             __LINE__,                    \
                                             __func__);                   \
}

#ifndef NDEBUG

#define ASSERT_NOT_NULL(ptr, ptrName)                                     \
{                                                                         \
  if (ptr == nullptr)                                                     \
  {                                                                       \
    FATAL_ERROR("Pointer \"%s\" is null!", ptrName);                      \
  }                                                                       \
}                                                                         \

#else
#define ASSERT_NOT_NULL(ptr, ptrName)
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
    std::unique_ptr<Impl> impl;
};

#endif // ERRORMACROS_H_INCLUDED
