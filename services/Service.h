#pragma once

#include <memory>

/// Static class which provides a particular service to the application.
/// The Service template parameter must contain a getDefault() static method that
/// returns a default object (usually a Null Object) that implements the interface.
/// (Could enforce this with some fancy SFINAE magic but it's probably not worth it.)
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
  /// Using one method for both of these is a massive kludge, but allows for 
  /// static data to be initialized in the header file instead of needing a 
  /// separate source file.
  static ServiceInterface& serviceInstance(ServiceInterface* service_ = nullptr, bool setValue = false)
  {
    static std::unique_ptr<ServiceInterface> service;

    if (setValue)
    {
      service.reset(service_);
    }

    if (!service)
    {
      service.reset(ServiceInterface::getDefault());
    }

    return *(service.get());
  }
  
};

/// Little template function so you don't have to type Service<IXXXXX>::get() 
/// to get a service each time.
/// This lets you type `S<IXXXX>()` instead, which is still clunky, but a bit
/// shorter.
template <typename ServiceInterface>
ServiceInterface& S()
{
  return Service<ServiceInterface>::get();
}


