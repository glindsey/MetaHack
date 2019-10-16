cmake_minimum_required(VERSION 3.1)

if(${CMAKE_VERSION} VERSION_LESS 3.13)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.13)
endif()

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")
include(cotire)

project(MetaHack VERSION 0.1
                 DESCRIPTION "Roguelike engine designed to be easily extensible"
                 LANGUAGES CXX)

set(EXECUTABLE_NAME "MetaHack")
set(CMAKE_CXX_STANDARD 14)
add_executable(MetaHack main.cpp) # Further sources will be added later in the file

# Enable debug symbols by default
set(default_build_type "Debug")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPE)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}"
      CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# === Platform-Specific Settings ==============================================

if(MSVC)
  set(TARGET_PLATFORM "msvc")
  set(RESOURCES_PATH $<TARGET_FILE_DIR:MetaHack>/resources)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX")
  # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} <whatever else we want here>")
endif()

if(APPLE)
  set(TARGET_PLATFORM "osx")
  set(RESOURCES_PATH $<TARGET_FILE_DIR:MetaHack>/../Resources)
  # pagezero_size, image_base are needed for LuaJIT to work properly See:
  # https://stackoverflow.com/questions/20858911/lua-open-returns-null-using-
  # luajit
  set(
    CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -pagezero_size 10000 -image_base 100000000 -framework CoreFoundation -framework CoreGraphics -framework IOKit -framework OpenGL -framework Cocoa -framework Carbon -framework freetype"
    )
  set(CMAKE_MACOSX_BUNDLE TRUE)
endif()

if(UNIX)
  set(TARGET_PLATFORM "linux")
  set(RESOURCES_PATH $<TARGET_FILE_DIR:MetaHack>/resources)
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} <whatever else we want here>")
  # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} <whatever else we want here>")
endif()

message("TARGET_PLATFORM = ${TARGET_PLATFORM}")

# === Auto-configure Libraries with CMake Modules =============================

# === Libraries ===============================================================

# --- OpenGL ------------------------------------------------------------------
find_package(OpenGL REQUIRED)

# Detect and add ICU
find_package(ICU COMPONENTS uc i18n REQUIRED) # can't find io on windows... ARGH
include_directories(${ICU_INCLUDE_DIRS})
target_link_libraries(MetaHack PRIVATE ${ICU_LIBRARIES})

# Detect and add SFML to the include and library paths
find_package(SFML 2 COMPONENTS network audio graphics window system REQUIRED)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(
    MetaHack PRIVATE
    sfml-network
    sfml-audio
    sfml-graphics
    sfml-window
    sfml-system
)

# Detect and add Boost to the include and library paths
set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.69 REQUIRED COMPONENTS chrono filesystem locale random system thread)
target_link_libraries(
    MetaHack PRIVATE
    Boost::chrono
    Boost::filesystem
    Boost::locale
    Boost::random
    Boost::system
    Boost::thread
)

# Detect and add SFGUI to the include and library paths
find_package(SFGUI 0.3 CONFIG REQUIRED)
if(NOT SFGUI_FOUND)
  message(FATAL_ERROR "Could not find SFGUI library")
endif()
include_directories(${SFGUI_INCLUDE_DIR})
target_link_libraries(MetaHack PRIVATE SFGUI::SFGUI)

# === LuaJIT Sources ==========================================================
find_library(LUAJIT_LIBRARY luajit libluajit lua51
             PATHS "${PROJECT_SOURCE_DIR}/libraries/luajit-2.0/${TARGET_PLATFORM}")

if(NOT LUAJIT_LIBRARY)
  message(STATUS "Tried: '${PROJECT_SOURCE_DIR}/libraries/luajit-2.0/${TARGET_PLATFORM}'")
  message(FATAL_ERROR "Could not find LuaJIT library")
endif()
include_directories("${PROJECT_SOURCE_DIR}/libraries/luajit-2.0/include")
target_link_libraries(MetaHack PRIVATE ${LUAJIT_LIBRARY})

# === Project Sources and Include Files =======================================

set(EXTERNAL_SOURCES
    ${PROJECT_SOURCE_DIR}/external/easylogging++-9.95/include/easylogging++.cc)

set(EXTERNAL_INCLUDES
    ${PROJECT_SOURCE_DIR}/external/cereal
    ${PROJECT_SOURCE_DIR}/external/nlohmann/include
    ${PROJECT_SOURCE_DIR}/external/easylogging++-9.95/include)

set(PROJECT_SOURCES_ACTIONS
    ${PROJECT_SOURCE_DIR}/actions/Action.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionAttack.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionClose.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionDie.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionDrop.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionEat.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionFill.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionGet.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionHurl.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionInscribe.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionLock.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionMove.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionOpen.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionPutInto.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionQuaff.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionQueue.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionRead.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionShoot.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionTakeOff.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionTakeOut.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionTurn.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionUnlock.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionUnwield.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionUse.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionWait.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionWear.cpp
    ${PROJECT_SOURCE_DIR}/actions/ActionWield.cpp)

set(PROJECT_INCLUDES_ACTIONS
    ${PROJECT_SOURCE_DIR}/actions/Action.h
    ${PROJECT_SOURCE_DIR}/actions/ActionAttack.h
    ${PROJECT_SOURCE_DIR}/actions/ActionClose.h
    ${PROJECT_SOURCE_DIR}/actions/ActionCRTP.h
    ${PROJECT_SOURCE_DIR}/actions/ActionDie.h
    ${PROJECT_SOURCE_DIR}/actions/ActionDrop.h
    ${PROJECT_SOURCE_DIR}/actions/ActionEat.h
    ${PROJECT_SOURCE_DIR}/actions/ActionFill.h
    ${PROJECT_SOURCE_DIR}/actions/ActionGet.h
    ${PROJECT_SOURCE_DIR}/actions/ActionHurl.h
    ${PROJECT_SOURCE_DIR}/actions/ActionInscribe.h
    ${PROJECT_SOURCE_DIR}/actions/ActionLock.h
    ${PROJECT_SOURCE_DIR}/actions/ActionMove.h
    ${PROJECT_SOURCE_DIR}/actions/ActionOpen.h
    ${PROJECT_SOURCE_DIR}/actions/ActionPutInto.h
    ${PROJECT_SOURCE_DIR}/actions/ActionQuaff.h
    ${PROJECT_SOURCE_DIR}/actions/ActionQueue.h
    ${PROJECT_SOURCE_DIR}/actions/ActionRead.h
    ${PROJECT_SOURCE_DIR}/actions/ActionShoot.h
    ${PROJECT_SOURCE_DIR}/actions/ActionTakeOff.h
    ${PROJECT_SOURCE_DIR}/actions/ActionTakeOut.h
    ${PROJECT_SOURCE_DIR}/actions/ActionTurn.h
    ${PROJECT_SOURCE_DIR}/actions/ActionUnlock.h
    ${PROJECT_SOURCE_DIR}/actions/ActionUnwield.h
    ${PROJECT_SOURCE_DIR}/actions/ActionUse.h
    ${PROJECT_SOURCE_DIR}/actions/ActionWait.h
    ${PROJECT_SOURCE_DIR}/actions/ActionWear.h
    ${PROJECT_SOURCE_DIR}/actions/ActionWield.h)

set(PROJECT_SOURCES_COMPONENTS
    ${PROJECT_SOURCE_DIR}/components/ComponentActivity.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentAnimated.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentAppearance.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentBodyparts.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentCombustible.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentCorrodible.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentDigestiveSystem.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentEquippable.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentGender.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentGlobals.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentHealth.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentInventory.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentLightSource.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentLockable.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentMagicalBinding.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentManager.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentMaterialFlags.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentMatterState.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentMobility.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentOpenable.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentPhysical.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentPosition.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentSapience.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentSenseSight.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentSpacialMemory.cpp
    ${PROJECT_SOURCE_DIR}/components/ComponentTemplate.cpp)

set(PROJECT_SOURCES_COMPONENT_MODIFIERS
    ${PROJECT_SOURCE_DIR}/components/modifiers/Combustible.cpp
    ${PROJECT_SOURCE_DIR}/components/modifiers/Physical.cpp)

set(PROJECT_INCLUDES_COMPONENTS
    ${PROJECT_SOURCE_DIR}/components/ComponentBase.h
    ${PROJECT_SOURCE_DIR}/components/ComponentCRTP.h
    ${PROJECT_SOURCE_DIR}/components/ComponentActivity.h
    ${PROJECT_SOURCE_DIR}/components/ComponentAnimated.h
    ${PROJECT_SOURCE_DIR}/components/ComponentAppearance.h
    ${PROJECT_SOURCE_DIR}/components/ComponentBodyparts.h
    ${PROJECT_SOURCE_DIR}/components/ComponentCombustible.h
    ${PROJECT_SOURCE_DIR}/components/ComponentCorrodible.h
    ${PROJECT_SOURCE_DIR}/components/ComponentDigestiveSystem.h
    ${PROJECT_SOURCE_DIR}/components/ComponentEquippable.h
    ${PROJECT_SOURCE_DIR}/components/ComponentGender.h
    ${PROJECT_SOURCE_DIR}/components/ComponentGlobals.h
    ${PROJECT_SOURCE_DIR}/components/ComponentHealth.h
    ${PROJECT_SOURCE_DIR}/components/ComponentInventory.h
    ${PROJECT_SOURCE_DIR}/components/ComponentLightSource.h
    ${PROJECT_SOURCE_DIR}/components/ComponentLockable.h
    ${PROJECT_SOURCE_DIR}/components/ComponentMagicalBinding.h
    ${PROJECT_SOURCE_DIR}/components/ComponentManager.h
    ${PROJECT_SOURCE_DIR}/components/ComponentMap.h
    ${PROJECT_SOURCE_DIR}/components/ComponentMaterialFlags.h
    ${PROJECT_SOURCE_DIR}/components/ComponentMatterState.h
    ${PROJECT_SOURCE_DIR}/components/ComponentMobility.h
    ${PROJECT_SOURCE_DIR}/components/ComponentOpenable.h
    ${PROJECT_SOURCE_DIR}/components/ComponentPhysical.h
    ${PROJECT_SOURCE_DIR}/components/ComponentPosition.h
    ${PROJECT_SOURCE_DIR}/components/ComponentSapience.h
    ${PROJECT_SOURCE_DIR}/components/ComponentSenseSight.h
    ${PROJECT_SOURCE_DIR}/components/ComponentSpacialMemory.h
    ${PROJECT_SOURCE_DIR}/components/ComponentTemplate.h)

set(PROJECT_INCLUDES_COMPONENT_MODIFIERS
    ${PROJECT_SOURCE_DIR}/components/modifiers/Base.h
    ${PROJECT_SOURCE_DIR}/components/modifiers/Collection.h
    ${PROJECT_SOURCE_DIR}/components/modifiers/Combustible.h
    ${PROJECT_SOURCE_DIR}/components/modifiers/Physical.h)

set(PROJECT_SOURCES_CONFIG
    ${PROJECT_SOURCE_DIR}/config/Bible.cpp
    ${PROJECT_SOURCE_DIR}/config/Paths.cpp
    ${PROJECT_SOURCE_DIR}/config/Settings.cpp
    ${PROJECT_SOURCE_DIR}/config/Strings.cpp)

set(PROJECT_INCLUDES_CONFIG
    ${PROJECT_SOURCE_DIR}/config/Bible.h
    ${PROJECT_SOURCE_DIR}/config/Paths.h
    ${PROJECT_SOURCE_DIR}/config/Settings.h
    ${PROJECT_SOURCE_DIR}/config/Strings.h)

set(PROJECT_SOURCES_ENTITY ${PROJECT_SOURCE_DIR}/entity/EntityFactory.cpp
    ${PROJECT_SOURCE_DIR}/entity/EntityId.cpp)

set(PROJECT_INCLUDES_ENTITY ${PROJECT_SOURCE_DIR}/entity/EntityFactory.h
    ${PROJECT_SOURCE_DIR}/entity/EntityId.h)

set(PROJECT_SOURCES_GAME
    ${PROJECT_SOURCE_DIR}/game/App.cpp
    ${PROJECT_SOURCE_DIR}/game/AppState.cpp
    ${PROJECT_SOURCE_DIR}/game/AppStateGameMode.cpp
    ${PROJECT_SOURCE_DIR}/game/AppStateMainMenu.cpp
    ${PROJECT_SOURCE_DIR}/game/AppStateSplashScreen.cpp
    ${PROJECT_SOURCE_DIR}/game/GameState.cpp)

set(PROJECT_INCLUDES_GAME
    ${PROJECT_SOURCE_DIR}/game/App.h
    ${PROJECT_SOURCE_DIR}/game/AppState.h
    ${PROJECT_SOURCE_DIR}/game/AppStateGameMode.h
    ${PROJECT_SOURCE_DIR}/game/AppStateMainMenu.h
    ${PROJECT_SOURCE_DIR}/game/AppStateSplashScreen.h
    ${PROJECT_SOURCE_DIR}/game/GameState.h)

set(PROJECT_SOURCES_GAME_WINDOWS
    ${PROJECT_SOURCE_DIR}/game_windows/MessageLogView.cpp
    ${PROJECT_SOURCE_DIR}/game_windows/StatusArea.cpp)

set(PROJECT_INCLUDES_GAME_WINDOWS
    ${PROJECT_SOURCE_DIR}/game_windows/MessageLogView.h
    ${PROJECT_SOURCE_DIR}/game_windows/StatusArea.h)

set(PROJECT_SOURCES_INFRASTRUCTURE
    ${PROJECT_SOURCE_DIR}/design_patterns/AssertHelper.cpp
    ${PROJECT_SOURCE_DIR}/design_patterns/Event.cpp
    ${PROJECT_SOURCE_DIR}/design_patterns/Object.cpp
    ${PROJECT_SOURCE_DIR}/design_patterns/ObjectRegistry.cpp
    ${PROJECT_SOURCE_DIR}/design_patterns/Printable.cpp
    ${PROJECT_SOURCE_DIR}/lua/LuaObject.cpp
    ${PROJECT_SOURCE_DIR}/properties/PropertyDictionary.cpp
    ${PROJECT_SOURCE_DIR}/src/stdafx.cpp
    ${PROJECT_SOURCE_DIR}/state_machine/State.cpp
    ${PROJECT_SOURCE_DIR}/state_machine/StateMachine.cpp
    ${PROJECT_SOURCE_DIR}/tilesheet/TileSheet.cpp
    ${PROJECT_SOURCE_DIR}/utilities/MathUtils.cpp
    ${PROJECT_SOURCE_DIR}/utilities/RNGUtils.cpp
    ${PROJECT_SOURCE_DIR}/utilities/StringTransforms.cpp)

set(PROJECT_INCLUDES_INFRASTRUCTURE
    ${PROJECT_SOURCE_DIR}/design_patterns/AssertHelper.h
    ${PROJECT_SOURCE_DIR}/design_patterns/Event.h
    ${PROJECT_SOURCE_DIR}/design_patterns/Object.h
    ${PROJECT_SOURCE_DIR}/design_patterns/ObjectRegistry.h
    ${PROJECT_SOURCE_DIR}/design_patterns/Printable.h
    ${PROJECT_SOURCE_DIR}/design_patterns/Visitor.h
    ${PROJECT_SOURCE_DIR}/events/UIEvents.h
    ${PROJECT_SOURCE_DIR}/include/stdafx.h
    ${PROJECT_SOURCE_DIR}/lua/LuaFunctions-Entity.h
    ${PROJECT_SOURCE_DIR}/lua/LuaFunctions-Global.h
    ${PROJECT_SOURCE_DIR}/lua/LuaObject.h
    ${PROJECT_SOURCE_DIR}/lua/LuaTemplates.h
    ${PROJECT_SOURCE_DIR}/properties/PropertyDictionary.h
    ${PROJECT_SOURCE_DIR}/state_machine/State.h
    ${PROJECT_SOURCE_DIR}/state_machine/StateMachine.h
    ${PROJECT_SOURCE_DIR}/tilesheet/TileSheet.h
    ${PROJECT_SOURCE_DIR}/utilities/CommonFunctions.h
    ${PROJECT_SOURCE_DIR}/utilities/EnumIterator.h
    ${PROJECT_SOURCE_DIR}/utilities/GetLetterKey.h
    ${PROJECT_SOURCE_DIR}/utilities/JSONUtils.h
    ${PROJECT_SOURCE_DIR}/utilities/MathUtils.h
    ${PROJECT_SOURCE_DIR}/utilities/New.h
    ${PROJECT_SOURCE_DIR}/utilities/Ordinal.h
    ${PROJECT_SOURCE_DIR}/utilities/RNGUtils.h
    ${PROJECT_SOURCE_DIR}/utilities/Shortcuts.h
    ${PROJECT_SOURCE_DIR}/utilities/StringTransforms.h)

set(PROJECT_SOURCES_INVENTORY
    ${PROJECT_SOURCE_DIR}/inventory/InventoryArea.cpp
    ${PROJECT_SOURCE_DIR}/inventory/InventorySelection.cpp
    ${PROJECT_SOURCE_DIR}/inventory/InventorySlot.cpp)

set(PROJECT_INCLUDES_INVENTORY
    ${PROJECT_SOURCE_DIR}/inventory/InventoryArea.h
    ${PROJECT_SOURCE_DIR}/inventory/InventorySelection.h
    ${PROJECT_SOURCE_DIR}/inventory/InventorySlot.h)

set(PROJECT_SOURCES_MAP
    ${PROJECT_SOURCE_DIR}/map/Map.cpp
    ${PROJECT_SOURCE_DIR}/map/MapCorridor.cpp
    ${PROJECT_SOURCE_DIR}/map/MapDiamond.cpp
    ${PROJECT_SOURCE_DIR}/map/MapDonutRoom.cpp
    ${PROJECT_SOURCE_DIR}/map/MapFactory.cpp
    ${PROJECT_SOURCE_DIR}/map/MapFeature.cpp
    ${PROJECT_SOURCE_DIR}/map/MapGenerator.cpp
    ${PROJECT_SOURCE_DIR}/map/MapLRoom.cpp
    ${PROJECT_SOURCE_DIR}/map/MapMemory.cpp
    ${PROJECT_SOURCE_DIR}/map/MapRoom.cpp
    ${PROJECT_SOURCE_DIR}/maptile/MapTile.cpp)

set(PROJECT_INCLUDES_MAP
    ${PROJECT_SOURCE_DIR}/map/Map.h
    ${PROJECT_SOURCE_DIR}/map/MapCorridor.h
    ${PROJECT_SOURCE_DIR}/map/MapDiamond.h
    ${PROJECT_SOURCE_DIR}/map/MapDonutRoom.h
    ${PROJECT_SOURCE_DIR}/map/MapFactory.h
    ${PROJECT_SOURCE_DIR}/map/MapFeature.h
    ${PROJECT_SOURCE_DIR}/map/MapGenerator.h
    ${PROJECT_SOURCE_DIR}/map/MapLRoom.h
    ${PROJECT_SOURCE_DIR}/map/MapMemory.h
    ${PROJECT_SOURCE_DIR}/map/MapRoom.h
    ${PROJECT_SOURCE_DIR}/maptile/MapTile.h)

set(PROJECT_SOURCES_OBJECTS
    ${PROJECT_SOURCE_DIR}/objects/GameLog.cpp
    )

set(PROJECT_INCLUDES_OBJECTS
    ${PROJECT_SOURCE_DIR}/objects/GameLog.h
    )

set(PROJECT_SOURCES_SFGUI_EXTENSIONS
    ${PROJECT_SOURCE_DIR}/sfgui_extensions/EntryPlus.cpp)

set(PROJECT_INCLUDES_SFGUI_EXTENSIONS
    ${PROJECT_SOURCE_DIR}/sfgui_extensions/EntryPlus.hpp)

set(PROJECT_SOURCES_SCENE_GRAPH
    ${PROJECT_SOURCE_DIR}/scene_graph/SceneNode.cpp
    ${PROJECT_SOURCE_DIR}/scene_graph/Artist.cpp
    ${PROJECT_SOURCE_DIR}/scene_graph/Canvas.cpp
    ${PROJECT_SOURCE_DIR}/scene_graph/DataTie.cpp
    ${PROJECT_SOURCE_DIR}/scene_graph/EventProcessor.cpp
    ${PROJECT_SOURCE_DIR}/scene_graph/NodeOrganizer.cpp
    ${PROJECT_SOURCE_DIR}/scene_graph/OwnedByNode.cpp)

set(PROJECT_INCLUDES_SCENE_GRAPH
    ${PROJECT_SOURCE_DIR}/scene_graph/SceneNode.h
    ${PROJECT_SOURCE_DIR}/scene_graph/Artist.h
    ${PROJECT_SOURCE_DIR}/scene_graph/Canvas.h
    ${PROJECT_SOURCE_DIR}/scene_graph/DataTie.h
    ${PROJECT_SOURCE_DIR}/scene_graph/EventProcessor.h
    ${PROJECT_SOURCE_DIR}/scene_graph/NodeOrganizer.h
    ${PROJECT_SOURCE_DIR}/scene_graph/OwnedByNode.h)

set(PROJECT_SOURCES_SYSTEMS
    ${PROJECT_SOURCE_DIR}/systems/Manager.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemChoreographer.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemDirector.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemEditor.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemFluidics.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemGeometry.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemGrimReaper.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemJanitor.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemLighting.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemLuaLiaison.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemMechanics.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemNarrator.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemSenseSight.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemThermodynamics.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemTimekeeper.cpp)

set(PROJECT_INCLUDES_SYSTEMS
    ${PROJECT_SOURCE_DIR}/systems/Base.h
    ${PROJECT_SOURCE_DIR}/systems/CRTP.h
    ${PROJECT_SOURCE_DIR}/systems/Manager.h
    ${PROJECT_SOURCE_DIR}/systems/SystemChoreographer.h
    ${PROJECT_SOURCE_DIR}/systems/SystemDirector.h
    ${PROJECT_SOURCE_DIR}/systems/SystemEditor.h
    ${PROJECT_SOURCE_DIR}/systems/SystemFluidics.h
    ${PROJECT_SOURCE_DIR}/systems/SystemGeometry.h
    ${PROJECT_SOURCE_DIR}/systems/SystemGrimReaper.h
    ${PROJECT_SOURCE_DIR}/systems/SystemJanitor.h
    ${PROJECT_SOURCE_DIR}/systems/SystemLighting.h
    ${PROJECT_SOURCE_DIR}/systems/SystemLuaLiaison.h
    ${PROJECT_SOURCE_DIR}/systems/SystemMechanics.h
    ${PROJECT_SOURCE_DIR}/systems/SystemNarrator.h
    ${PROJECT_SOURCE_DIR}/systems/SystemSenseSight.h
    ${PROJECT_SOURCE_DIR}/systems/SystemThermodynamics.h
    ${PROJECT_SOURCE_DIR}/systems/SystemTimekeeper.h)

set(PROJECT_SOURCES_TYPES
    ${PROJECT_SOURCE_DIR}/types/BodyPart.cpp
    ${PROJECT_SOURCE_DIR}/types/Color.cpp
    ${PROJECT_SOURCE_DIR}/types/Direction.cpp
    ${PROJECT_SOURCE_DIR}/types/EntitySpecs.cpp
    ${PROJECT_SOURCE_DIR}/types/Gender.cpp
    ${PROJECT_SOURCE_DIR}/types/MapMemoryChunk.cpp
    ${PROJECT_SOURCE_DIR}/types/Vec2.cpp)

set(PROJECT_INCLUDES_TYPES
    ${PROJECT_SOURCE_DIR}/types/Beatitude.h
    ${PROJECT_SOURCE_DIR}/types/BodyPart.h
    ${PROJECT_SOURCE_DIR}/types/Clamped.h
    ${PROJECT_SOURCE_DIR}/types/Color.h
    ${PROJECT_SOURCE_DIR}/types/common.h
    ${PROJECT_SOURCE_DIR}/types/DequeMap.h
    ${PROJECT_SOURCE_DIR}/types/Direction.h
    ${PROJECT_SOURCE_DIR}/types/DirGrid.h
    ${PROJECT_SOURCE_DIR}/types/EntitySpecs.h
    ${PROJECT_SOURCE_DIR}/types/Gender.h
    ${PROJECT_SOURCE_DIR}/types/GeoVector.h
    ${PROJECT_SOURCE_DIR}/types/Grid2D.h
    ${PROJECT_SOURCE_DIR}/types/Grid3D.h
    ${PROJECT_SOURCE_DIR}/types/IRenderable.h
    ${PROJECT_SOURCE_DIR}/types/LightInfluence.h
    ${PROJECT_SOURCE_DIR}/types/MapMemoryChunk.h
    ${PROJECT_SOURCE_DIR}/types/Modifier.h
    ${PROJECT_SOURCE_DIR}/types/MouseButtonInfo.h
    ${PROJECT_SOURCE_DIR}/types/Rect.h
    ${PROJECT_SOURCE_DIR}/types/ShaderEffect.h
    ${PROJECT_SOURCE_DIR}/types/SpritePrototype.h
    ${PROJECT_SOURCE_DIR}/types/Vec2.h
    ${PROJECT_SOURCE_DIR}/types/Vec3.h)

set(PROJECT_SOURCES_VIEWS
    ${PROJECT_SOURCE_DIR}/views/EntityCollectionGUIListView.cpp
    ${PROJECT_SOURCE_DIR}/views/EntityCollectionGUIView.cpp
    ${PROJECT_SOURCE_DIR}/views/EntityGUIIconView.cpp
    ${PROJECT_SOURCE_DIR}/views/EntityGUIView.cpp
    ${PROJECT_SOURCE_DIR}/views/EntityView2D.cpp
    ${PROJECT_SOURCE_DIR}/views/EntityView.cpp
    ${PROJECT_SOURCE_DIR}/views/MapView2D.cpp
    ${PROJECT_SOURCE_DIR}/views/MapTileView2D.cpp
    ${PROJECT_SOURCE_DIR}/views/MapView.cpp)

set(PROJECT_INCLUDES_VIEWS
    ${PROJECT_SOURCE_DIR}/views/EntityCollectionGUIListView.h
    ${PROJECT_SOURCE_DIR}/views/EntityCollectionGUIView.h
    ${PROJECT_SOURCE_DIR}/views/EntityGUIIconView.h
    ${PROJECT_SOURCE_DIR}/views/EntityGUIView.h
    ${PROJECT_SOURCE_DIR}/views/EntityView2D.h
    ${PROJECT_SOURCE_DIR}/views/EntityView.h
    ${PROJECT_SOURCE_DIR}/views/MapView2D.h
    ${PROJECT_SOURCE_DIR}/views/MapTileView.h
    ${PROJECT_SOURCE_DIR}/views/MapTileView2D.h
    ${PROJECT_SOURCE_DIR}/views/MapView.h)

set(PROJECT_SOURCES
    ${EXTERNAL_SOURCES}
    ${PROJECT_SOURCES_ACTIONS}
    ${PROJECT_SOURCES_COMPONENTS}
    ${PROJECT_SOURCES_COMPONENT_MODIFIERS}
    ${PROJECT_SOURCES_CONFIG}
    ${PROJECT_SOURCES_ENTITY}
    ${PROJECT_SOURCES_GAME}
    ${PROJECT_SOURCES_GAME_WINDOWS}
    ${PROJECT_SOURCES_INFRASTRUCTURE}
    ${PROJECT_SOURCES_INVENTORY}
    ${PROJECT_SOURCES_MAP}
    ${PROJECT_SOURCES_OBJECTS}
    ${PROJECT_SOURCES_SCENE_GRAPH}
    ${PROJECT_SOURCES_SFGUI_EXTENSIONS}
    ${PROJECT_SOURCES_SYSTEMS}
    ${PROJECT_SOURCES_TYPES}
    ${PROJECT_SOURCES_VIEWS})

set(PROJECT_INCLUDES
    ${EXTERNAL_INCLUDES}
    ${PROJECT_INCLUDES_ACTIONS}
    ${PROJECT_INCLUDES_COMPONENTS}
    ${PROJECT_INCLUDES_COMPONENT_MODIFIERS}
    ${PROJECT_INCLUDES_CONFIG}
    ${PROJECT_INCLUDES_ENTITY}
    ${PROJECT_INCLUDES_GAME}
    ${PROJECT_INCLUDES_GAME_WINDOWS}
    ${PROJECT_INCLUDES_INFRASTRUCTURE}
    ${PROJECT_INCLUDES_INVENTORY}
    ${PROJECT_INCLUDES_MAP}
    ${PROJECT_INCLUDES_OBJECTS}
    ${PROJECT_INCLUDES_SCENE_GRAPH}
    ${PROJECT_INCLUDES_SFGUI_EXTENSIONS}
    ${PROJECT_INCLUDES_SYSTEMS}
    ${PROJECT_INCLUDES_TYPES}
    ${PROJECT_INCLUDES_VIEWS})

include_directories(${PROJECT_SOURCE_DIR}
                    ${PROJECT_SOURCE_DIR}/design_patterns
                    ${PROJECT_SOURCE_DIR}/include
                    ${EXTERNAL_INCLUDES})

# === Putting It All Together =================================================

target_sources(MetaHack
               PUBLIC
               ${PROJECT_SOURCES}
               ${PROJECT_INCLUDES})

source_group("External"
             FILES
             ${EXTERNAL_SOURCES}
             ${EXTERNAL_INCLUDES})

source_group("Actions"
             FILES
             ${PROJECT_SOURCES_ACTIONS}
             ${PROJECT_INCLUDES_ACTIONS})
source_group("Components"
             FILES
             ${PROJECT_SOURCES_COMPONENTS}
             ${PROJECT_INCLUDES_COMPONENTS}
             ${PROJECT_SOURCES_COMPONENT_MODIFIERS}
             ${PROJECT_INCLUDES_COMPONENT_MODIFIERS})
source_group("Config"
             FILES
             ${PROJECT_SOURCES_CONFIG}
             ${PROJECT_INCLUDES_CONFIG})
source_group("Entities"
             FILES
             ${PROJECT_SOURCES_ENTITY}
             ${PROJECT_INCLUDES_ENTITY})
source_group("Game"
             FILES
             ${PROJECT_SOURCES_GAME}
             ${PROJECT_INCLUDES_GAME})
source_group("Game Windows"
             FILES
             ${PROJECT_SOURCES_GAME_WINDOWS}
             ${PROJECT_INCLUDES_GAME_WINDOWS})
source_group("Infrastructure"
             FILES
             ${PROJECT_SOURCES_INFRASTRUCTURE}
             ${PROJECT_INCLUDES_INFRASTRUCTURE})
source_group("Inventory"
             FILES
             ${PROJECT_SOURCES_INVENTORY}
             ${PROJECT_INCLUDES_INVENTORY})
source_group("Map"
             FILES
             ${PROJECT_SOURCES_MAP}
             ${PROJECT_INCLUDES_MAP})
source_group("Objects"
             FILES
             ${PROJECT_SOURCES_OBJECTS}
             ${PROJECT_INCLUDES_OBJECTS})
source_group("Scene Graph"
             FILES
             ${PROJECT_SOURCES_SCENE_GRAPH}
             ${PROJECT_INCLUDES_SCENE_GRAPH})
source_group("SFGUI Extensions"
             FILES
             ${PROJECT_SOURCES_SFGUI_EXTENSIONS}
             ${PROJECT_INCLUDES_SFGUI_EXTENSIONS})
source_group("Systems"
             FILES
             ${PROJECT_SOURCES_SYSTEMS}
             ${PROJECT_INCLUDES_SYSTEMS})
source_group("Types"
             FILES
             ${PROJECT_SOURCES_TYPES}
             ${PROJECT_INCLUDES_TYPES})
source_group("Views"
             FILES
             ${PROJECT_SOURCES_VIEWS}
             ${PROJECT_INCLUDES_VIEWS})

set_property(TARGET MetaHack PROPERTY
             VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_custom_command(TARGET MetaHack POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                           ${PROJECT_SOURCE_DIR}/resources ${RESOURCES_PATH})

cotire(MetaHack)

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
