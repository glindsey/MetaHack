#pragma once

#include "services/IPaths.h"

/// Default pathname service.
/// (Really, I can't see ever needing a different pathname service, but it's nice to be consistent.)
class DefaultPaths : public IPaths
{
public:
  DefaultPaths();
  virtual ~DefaultPaths();
  
  /// Get const reference to the resources path.
  virtual std::string const& resources() override;
  
  /// Get const reference to the log path.
  virtual std::string const& logs() override;
  
protected:

private:
  std::string m_resourcesPath;
  std::string m_logsPath;
};

