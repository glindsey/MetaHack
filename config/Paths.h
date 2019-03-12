#pragma once

#include <boost/noncopyable.hpp>
#include <string>

namespace Config
{
  /// Static pathnames provider.
  class Paths : public boost::noncopyable
  {
  public:
    virtual ~Paths();

    /// Get instance reference.
    friend Paths& paths();

    /// Get const reference to the resources path.
    std::string const& resources();

    /// Get const reference to the log path.
    std::string const& logs();

  protected:
    Paths();

  private:
    std::string m_resourcesPath;
    std::string m_logsPath;
  };

  Paths& paths();
} // end namespace Config
