#include "stdafx.h"

#include "game/App.h"

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

INITIALIZE_EASYLOGGINGPP

int main(int argc, char* argv[])
{
  START_EASYLOGGINGPP(argc, argv);

  el::Loggers::reconfigureAllLoggers(
    el::ConfigurationType::Format,
    "[%logger] %loc %level: %msg");

  LOG(INFO) << "Entered main()";

  // Generate system default locale and set it globally.
  // This is done BEFORE setting CRT debug flags, because the ICU library
  // causes false positives (due to global destructors running after the
  // memory leak report is generated).
  // EDIT: Unfortunately that doesn't seem to fix the problem. Argh.
  bl::generator gen;
  std::locale::global(gen("en_US.UTF-8"));

  std::string workingDirectory { argv[0] };
  std::string logDirectory { workingDirectory + "/log" };
  std::string resourcesDirectory { workingDirectory + "/resources" };
  
  // If in MacOS, we get these in a somewhat different manner, because Apple has to be different.
  // (See https://stackoverflow.com/questions/516200/relative-paths-not-working-in-xcode-c?rq=1 for details)
#ifdef __APPLE__
  CFBundleRef mainBundle = CFBundleGetMainBundle();
  char path[PATH_MAX];
  
  CFURLRef executableURL = CFBundleCopyExecutableURL(mainBundle);
  if (!CFURLGetFileSystemRepresentation(executableURL, TRUE, (UInt8 *)path, PATH_MAX))
  {
    LOG(FATAL) << "Could not obtain resources directory name from CoreFoundation!";
  }
  logDirectory = std::string(path) + "/log";
  CFRelease(executableURL);
  
  CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
  if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
  {
    LOG(FATAL) << "Could not obtain resources directory name from CoreFoundation!";
  }
  resourcesDirectory = std::string(path);
  CFRelease(resourcesURL);
#endif

  std::cout << "Log directory is " << logDirectory << std::endl;
  std::cout << "Resources directory is " << resourcesDirectory << std::endl;
  
#ifdef _DEBUG
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  // Scoping everything here so it is properly destroyed before checking for memory leaks.
  {
    std::unique_ptr<sf::RenderWindow> app_window;
    std::unique_ptr<App> app;

#ifdef NDEBUG
    try
#endif
    {     
      // Check to make sure shaders are available.
      if (!sf::Shader::isAvailable())
      {
        LOG(FATAL) << "Shaders are not available on this platform";
      }

      // Create and open the main window.
      app_window.reset(NEW sf::RenderWindow(sf::VideoMode(1066, 600), "Magicule Saga"));

      auto settings = app_window->getSettings();
      LOG(INFO) << "depthBits = " << settings.depthBits;
      LOG(INFO) << "stencilBits = " << settings.stencilBits;
      LOG(INFO) << "antialiasingLevel = " << settings.antialiasingLevel;
      LOG(INFO) << "version = " << settings.majorVersion << "." << settings.minorVersion;
      LOG(INFO) << "attributeFlags = 0x" << hexify<uint32_t>(settings.attributeFlags);

      // Create and run the app instance.
      app.reset(NEW App(*app_window,
                        resourcesDirectory,
                        logDirectory));
      app->run();
    }
#ifdef NDEBUG
    catch (std::exception& e)
    {
      LOG(FATAL) << "Caught top-level exception: " << e.what();
    }
#endif
  }

  /// @todo Handle ICU cleanup gracefully; doesn't look like boost::locale takes care of it.

  return EXIT_SUCCESS;
}
