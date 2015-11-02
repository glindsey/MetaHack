#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <boost/format.hpp>
#include <exception>

class ExceptionMissingFile : public std::exception
{
public:
  ExceptionMissingFile(std::string fileType, 
                       std::string objectType,
                       std::string objectName) 
    : m_fileType(fileType),
      m_objectType(objectType),
      m_objectName(objectName) {}
  
  virtual ~ExceptionMissingFile() {}

  virtual char const * what() const
  {
    return (boost::format("The %s file for %s:%s was not found") 
      % m_fileType % m_objectType % m_objectName).str().c_str();
  }

private:
  std::string m_fileType;
  std::string m_objectType;
  std::string m_objectName;
};

#endif // EXCEPTIONS_H
