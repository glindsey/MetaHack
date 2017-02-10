#pragma once

#include <memory>

/// Static class which provides a particular service to the application.
/// The Service template parameter must contain a getNull() static method that
/// returns a Null Object that implements the interface.
template <class ServiceInterface>
class Service final
{
public:
  /// Return a reference to the service being provided.
  /// If no service has been provided yet, return a reference to a
  /// null service.
  /// @return A reference to the requested service.
  static ServiceInterface& get()
  {
    return serviceInstance();
  }

  /// Set the service instance to be provided.
  /// @param service_ The service to return when get() is called.
  /// @warning This class TAKES OWNERSHIP OF THE PASSED RAW POINTER.
  static void provide(ServiceInterface* service_)
  {
    serviceInstance(service_, true);
  }

protected:

private:
  /// Get/set a static member.
  /// Massive kludge, but allows for static data to be initialized in the
  /// header file instead of a source file.
  static ServiceInterface& serviceInstance(ServiceInterface* service_ = nullptr, bool setValue = false)
  {
    static std::unique_ptr<ServiceInterface> service;

    if (setValue)
    {
      service.reset(service_);
    }

    if (!service)
    {
      service.reset(ServiceInterface::getNull());
    }

    return *(service.get());
  }
  
};
