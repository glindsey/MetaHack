cmake_minimum_required(VERSION 3.10)

# Enable debug symbols by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug
      CACHE STRING "Choose the type of build (Debug or Release")
endif()

project(MetaHack)
set(EXECUTABLE_NAME "MetaHack")
set(CMAKE_CXX_STANDARD 14)
add_executable(${EXECUTABLE_NAME} main.cpp) # Further sources will be added
                                            # later in the file

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  set(LIBRARY_SUFFIX "-d")
else()
  set(LIBRARY_SUFFIX "")
endif()

# === Platform-Specific Libraries and Includes ================================

if(WIN32)
  set(TARGET_PLATFORM windows-msvc14-64bit)
  set(TARGET_LIBRARY_EXTENSION lib)
  set(
    TARGET_SPECIFIC_LIBS
    ${PROJECT_SOURCE_DIR}/libraries/sfml-deps/${TARGET_PLATFORM}/flac.${TARGET_LIBRARY_EXTENSION}
    ${PROJECT_SOURCE_DIR}/libraries/sfml-deps/${TARGET_PLATFORM}/freetype.${TARGET_LIBRARY_EXTENSION}
    ${PROJECT_SOURCE_DIR}/libraries/sfml-deps/${TARGET_PLATFORM}/jpeg.${TARGET_LIBRARY_EXTENSION}
    ${PROJECT_SOURCE_DIR}/libraries/sfml-deps/${TARGET_PLATFORM}/ogg.${TARGET_LIBRARY_EXTENSION}
    ${PROJECT_SOURCE_DIR}/libraries/sfml-deps/${TARGET_PLATFORM}/openal32.${TARGET_LIBRARY_EXTENSION}
    ${PROJECT_SOURCE_DIR}/libraries/sfml-deps/${TARGET_PLATFORM}/vorbis.${TARGET_LIBRARY_EXTENSION}
    ${PROJECT_SOURCE_DIR}/libraries/sfml-deps/${TARGET_PLATFORM}/vorbisenc.${TARGET_LIBRARY_EXTENSION}
    ${PROJECT_SOURCE_DIR}/libraries/sfml-deps/${TARGET_PLATFORM}/vorbisfile.${TARGET_LIBRARY_EXTENSION}
    )
  set(CMAKE_PREFIX_PATH "C:/Program Files/SFML")
  # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} <whatever else we want here>")
  # SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} <whatever else we want
  # here>")
elseif(APPLE)
  set(TARGET_PLATFORM osx-clang-64bit)
  set(TARGET_LIBRARY_PREFIX lib)
  set(TARGET_LIBRARY_EXTENSION a)
  # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} <whatever else we want here>")
  # pagezero_size, image_base are needed for LuaJIT to work properly See:
  # https://stackoverflow.com/questions/20858911/lua-open-returns-null-using-
  # luajit
  set(
    CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -pagezero_size 10000 -image_base 100000000 -framework CoreFoundation -framework CoreGraphics -framework IOKit -framework OpenGL -framework Cocoa -framework Carbon -framework freetype"
    )
  set(TARGET_SPECIFIC_LIBS /usr/local/lib/libjpeg.a iconv)
  set(CMAKE_MACOSX_BUNDLE TRUE)
elseif(UNIX)
  set(TARGET_PLATFORM linux-gcc-64bit)
  set(TARGET_LIBRARY_PREFIX lib)
  set(TARGET_LIBRARY_EXTENSION so)
  # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} <whatever else we want here>")
  # SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} <whatever else we want
  # here>")
endif()

message("TARGET_PLATFORM = ${TARGET_PLATFORM}")

set(LIBRARY_INCLUDES
    ${PROJECT_SOURCE_DIR}/libraries/cereal
    ${PROJECT_SOURCE_DIR}/libraries/luajit-2.0/include
    ${PROJECT_SOURCE_DIR}/libraries/nlohmann/include
    ${PROJECT_SOURCE_DIR}/libraries/easylogging++-9.95/include)

set(ADDITIONAL_SOURCES
    ${PROJECT_SOURCE_DIR}/libraries/easylogging++-9.95/include/easylogging++.cc)

set(ADDITIONAL_LIBRARIES
    ${PROJECT_SOURCE_DIR}/libraries/luajit-2.0/${TARGET_PLATFORM}/${TARGET_LIBRARY_PREFIX}luajit.${TARGET_LIBRARY_EXTENSION})

# === Auto-configure Libraries with CMake Modules =============================

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

# Detect and add SFML to the include and library paths
find_package(SFML 2 REQUIRED network audio graphics window system)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES}
                        ${SFML_DEPENDENCIES})
endif()

# Detect and add SFGUI to the include and library paths
find_package(SFGUI 0.3 REQUIRED)
if(SFGUI_FOUND)
  include_directories(${SFGUI_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFGUI_LIBRARY}
                        ${SFGUI_DEPENDENCIES})
endif()

# Detect and add Boost to the include and library paths
find_package(Boost 1.65 EXACT REQUIRED
             COMPONENTS chrono
                        filesystem
                        locale
                        random
                        system
                        thread)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(${EXECUTABLE_NAME} ${Boost_LIBRARIES})
endif()

target_link_libraries(${EXECUTABLE_NAME} ${TARGET_SPECIFIC_LIBS} ${ADDITIONAL_LIBRARIES})

# === Project Sources and Include Files =======================================

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

set(PROJECT_INCLUDES_COMPONENTS
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

set(PROJECT_SOURCES_GUI
    ${PROJECT_SOURCE_DIR}/gui/GUICloseHandle.cpp
    ${PROJECT_SOURCE_DIR}/gui/GUICollapseHandle.cpp
    ${PROJECT_SOURCE_DIR}/gui/GUIDesktop.cpp
    ${PROJECT_SOURCE_DIR}/gui/GUILabel.cpp
    ${PROJECT_SOURCE_DIR}/gui/GUIObject.cpp
    ${PROJECT_SOURCE_DIR}/gui/GUIResizeHandle.cpp
    ${PROJECT_SOURCE_DIR}/gui/GUIShrinkHandle.cpp
    ${PROJECT_SOURCE_DIR}/gui/GUITitleBar.cpp
    ${PROJECT_SOURCE_DIR}/gui/GUIWindow.cpp)

set(PROJECT_INCLUDES_GUI
    ${PROJECT_SOURCE_DIR}/gui/GUICloseHandle.h
    ${PROJECT_SOURCE_DIR}/gui/GUICollapseHandle.h
    ${PROJECT_SOURCE_DIR}/gui/GUIDesktop.h
    ${PROJECT_SOURCE_DIR}/gui/GUILabel.h
    ${PROJECT_SOURCE_DIR}/gui/GUIObject.h
    ${PROJECT_SOURCE_DIR}/gui/GUIResizeHandle.h
    ${PROJECT_SOURCE_DIR}/gui/GUIShrinkHandle.h
    ${PROJECT_SOURCE_DIR}/gui/GUITitleBar.h
    ${PROJECT_SOURCE_DIR}/gui/GUIWindow.h)

set(PROJECT_SOURCES_INFRASTRUCTURE
    ${PROJECT_SOURCE_DIR}/design_patterns/AssertHelper.cpp
    ${PROJECT_SOURCE_DIR}/design_patterns/Event.cpp
    ${PROJECT_SOURCE_DIR}/design_patterns/Object.cpp
    ${PROJECT_SOURCE_DIR}/keybuffer/KeyBuffer.cpp
    ${PROJECT_SOURCE_DIR}/lua/LuaObject.cpp
    ${PROJECT_SOURCE_DIR}/properties/PropertyDictionary.cpp
    ${PROJECT_SOURCE_DIR}/src/stdafx.cpp
    ${PROJECT_SOURCE_DIR}/state_machine/State.cpp
    ${PROJECT_SOURCE_DIR}/state_machine/StateMachine.cpp
    ${PROJECT_SOURCE_DIR}/tilesheet/TileSheet.cpp
    ${PROJECT_SOURCE_DIR}/utilities/MathUtils.cpp
    ${PROJECT_SOURCE_DIR}/utilities/StringTransforms.cpp
    ${ADDITIONAL_SOURCES})

set(PROJECT_INCLUDES_INFRASTRUCTURE
    ${PROJECT_SOURCE_DIR}/design_patterns/AssertHelper.h
    ${PROJECT_SOURCE_DIR}/design_patterns/Event.h
    ${PROJECT_SOURCE_DIR}/design_patterns/Object.h
    ${PROJECT_SOURCE_DIR}/design_patterns/Serializable.h
    ${PROJECT_SOURCE_DIR}/design_patterns/Visitor.h
    ${PROJECT_SOURCE_DIR}/events/UIEvents.h
    ${PROJECT_SOURCE_DIR}/include/stdafx.h
    ${PROJECT_SOURCE_DIR}/keybuffer/IKeyBuffer.h
    ${PROJECT_SOURCE_DIR}/keybuffer/KeyBuffer.h
    ${PROJECT_SOURCE_DIR}/keybuffer/NullKeyBuffer.h
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

set(PROJECT_SOURCES_SERVICES
    ${PROJECT_SOURCE_DIR}/services/DefaultPaths.cpp
    ${PROJECT_SOURCE_DIR}/services/FallbackConfigSettings.cpp
    ${PROJECT_SOURCE_DIR}/services/FileSystemGameRules.cpp
    ${PROJECT_SOURCE_DIR}/services/IConfigSettings.cpp
    ${PROJECT_SOURCE_DIR}/services/IGameRules.cpp
    ${PROJECT_SOURCE_DIR}/services/IGraphicViews.cpp
    ${PROJECT_SOURCE_DIR}/services/IMessageLog.cpp
    ${PROJECT_SOURCE_DIR}/services/IPaths.cpp
    ${PROJECT_SOURCE_DIR}/services/IStrings.cpp
    ${PROJECT_SOURCE_DIR}/services/MessageLog.cpp
    ${PROJECT_SOURCE_DIR}/services/Strings.cpp)

set(PROJECT_INCLUDES_SERVICES
    ${PROJECT_SOURCE_DIR}/services/DefaultPaths.h
    ${PROJECT_SOURCE_DIR}/services/FallbackConfigSettings.h
    ${PROJECT_SOURCE_DIR}/services/FileSystemGameRules.h
    ${PROJECT_SOURCE_DIR}/services/IConfigSettings.h
    ${PROJECT_SOURCE_DIR}/services/IGameRules.h
    ${PROJECT_SOURCE_DIR}/services/IGraphicViews.h
    ${PROJECT_SOURCE_DIR}/services/IMessageLog.h
    ${PROJECT_SOURCE_DIR}/services/IPaths.h
    ${PROJECT_SOURCE_DIR}/services/IStrings.h
    ${PROJECT_SOURCE_DIR}/services/MessageLog.h
    ${PROJECT_SOURCE_DIR}/services/NullGameRules.h
    ${PROJECT_SOURCE_DIR}/services/NullMessageLog.h
    ${PROJECT_SOURCE_DIR}/services/NullStrings.h
    ${PROJECT_SOURCE_DIR}/services/Service.h
    ${PROJECT_SOURCE_DIR}/services/Strings.h)

set(PROJECT_SOURCES_SYSTEMS
    ${PROJECT_SOURCE_DIR}/systems/Manager.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemChoreographer.cpp
    ${PROJECT_SOURCE_DIR}/systems/SystemDirector.cpp
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
    ${PROJECT_SOURCE_DIR}/types/IntegerRange.cpp
    ${PROJECT_SOURCE_DIR}/types/MapMemoryChunk.cpp
    ${PROJECT_SOURCE_DIR}/types/ModifiableBool.cpp
    ${PROJECT_SOURCE_DIR}/types/ModifiableInt.cpp
    ${PROJECT_SOURCE_DIR}/types/Vec2.cpp)

set(PROJECT_INCLUDES_TYPES
    ${PROJECT_SOURCE_DIR}/types/Beatitude.h
    ${PROJECT_SOURCE_DIR}/types/BodyPart.h
    ${PROJECT_SOURCE_DIR}/types/Clamped.h
    ${PROJECT_SOURCE_DIR}/types/Color.h
    ${PROJECT_SOURCE_DIR}/types/common.h
    ${PROJECT_SOURCE_DIR}/types/Direction.h
    ${PROJECT_SOURCE_DIR}/types/DirGrid.h
    ${PROJECT_SOURCE_DIR}/types/EntitySpecs.h
    ${PROJECT_SOURCE_DIR}/types/GameObject.h
    ${PROJECT_SOURCE_DIR}/types/Gender.h
    ${PROJECT_SOURCE_DIR}/types/GeoVector.h
    ${PROJECT_SOURCE_DIR}/types/Grid2D.h
    ${PROJECT_SOURCE_DIR}/types/Grid3D.h
    ${PROJECT_SOURCE_DIR}/types/IntegerRange.h
    ${PROJECT_SOURCE_DIR}/types/IRenderable.h
    ${PROJECT_SOURCE_DIR}/types/LightInfluence.h
    ${PROJECT_SOURCE_DIR}/types/MapMemoryChunk.h
    ${PROJECT_SOURCE_DIR}/types/ModifiableBool.h
    ${PROJECT_SOURCE_DIR}/types/ModifiableInt.h
    ${PROJECT_SOURCE_DIR}/types/Modifier.h
    ${PROJECT_SOURCE_DIR}/types/MouseButtonInfo.h
    ${PROJECT_SOURCE_DIR}/types/Rect.h
    ${PROJECT_SOURCE_DIR}/types/ShaderEffect.h
    ${PROJECT_SOURCE_DIR}/types/SpritePrototype.h
    ${PROJECT_SOURCE_DIR}/types/Vec2.h
    ${PROJECT_SOURCE_DIR}/types/Vec3.h)

set(PROJECT_SOURCES_VIEWS
    # ${PROJECT_SOURCE_DIR}/views/EntityFancyAsciiView.cpp
    ${PROJECT_SOURCE_DIR}/views/EntityNullView.cpp
    ${PROJECT_SOURCE_DIR}/views/EntityStandard2DView.cpp
    ${PROJECT_SOURCE_DIR}/views/EntityView.cpp
    # ${PROJECT_SOURCE_DIR}/views/FancyAsciiGraphicViews.cpp
    # ${PROJECT_SOURCE_DIR}/views/MapFancyAsciiView.cpp
    ${PROJECT_SOURCE_DIR}/views/MapNullView.cpp
    ${PROJECT_SOURCE_DIR}/views/MapStandard2DView.cpp
    # ${PROJECT_SOURCE_DIR}/views/MapTileFancyAsciiView.cpp
    ${PROJECT_SOURCE_DIR}/views/MapTileNullView.cpp
    ${PROJECT_SOURCE_DIR}/views/MapTileStandard2DView.cpp
    ${PROJECT_SOURCE_DIR}/views/MapView.cpp
    ${PROJECT_SOURCE_DIR}/views/Standard2DGraphicViews.cpp)

set(PROJECT_INCLUDES_VIEWS
    # ${PROJECT_SOURCE_DIR}/views/EntityFancyAsciiView.h
    ${PROJECT_SOURCE_DIR}/views/EntityNullView.h
    ${PROJECT_SOURCE_DIR}/views/EntityStandard2DView.h
    ${PROJECT_SOURCE_DIR}/views/EntityView.h
    # ${PROJECT_SOURCE_DIR}/views/FancyAsciiGraphicViews.h
    # ${PROJECT_SOURCE_DIR}/views/MapFancyAsciiView.h
    ${PROJECT_SOURCE_DIR}/views/MapNullView.h
    ${PROJECT_SOURCE_DIR}/views/MapStandard2DView.h
    # ${PROJECT_SOURCE_DIR}/views/MapTileFancyAsciiView.h
    ${PROJECT_SOURCE_DIR}/views/MapTileNullView.h
    ${PROJECT_SOURCE_DIR}/views/MapTileStandard2DView.h
    ${PROJECT_SOURCE_DIR}/views/MapTileView.h
    ${PROJECT_SOURCE_DIR}/views/MapView.h
    ${PROJECT_SOURCE_DIR}/views/NullGraphicViews.h
    ${PROJECT_SOURCE_DIR}/views/Standard2DGraphicViews.h)

set(PROJECT_SOURCES
    ${PROJECT_SOURCES_ACTIONS}
    ${PROJECT_SOURCES_COMPONENTS}
    ${PROJECT_SOURCES_ENTITY}
    ${PROJECT_SOURCES_GAME}
    ${PROJECT_SOURCES_GAME_WINDOWS}
    ${PROJECT_SOURCES_GUI}
    ${PROJECT_SOURCES_INFRASTRUCTURE}
    ${PROJECT_SOURCES_INVENTORY}
    ${PROJECT_SOURCES_MAP}
    ${PROJECT_SOURCES_SERVICES}
    ${PROJECT_SOURCES_SYSTEMS}
    ${PROJECT_SOURCES_TYPES}
    ${PROJECT_SOURCES_VIEWS})

set(PROJECT_INCLUDES
    ${PROJECT_INCLUDES_ACTIONS}
    ${PROJECT_INCLUDES_COMPONENTS}
    ${PROJECT_INCLUDES_ENTITY}
    ${PROJECT_INCLUDES_GAME}
    ${PROJECT_INCLUDES_GAME_WINDOWS}
    ${PROJECT_INCLUDES_GUI}
    ${PROJECT_INCLUDES_INFRASTRUCTURE}
    ${PROJECT_INCLUDES_INVENTORY}
    ${PROJECT_INCLUDES_MAP}
    ${PROJECT_INCLUDES_SERVICES}
    ${PROJECT_INCLUDES_SYSTEMS}
    ${PROJECT_INCLUDES_TYPES}
    ${PROJECT_INCLUDES_VIEWS})

include_directories(${PROJECT_SOURCE_DIR}
                    ${PROJECT_SOURCE_DIR}/design_patterns
                    ${PROJECT_SOURCE_DIR}/gui
                    ${PROJECT_SOURCE_DIR}/include
                    ${LIBRARY_INCLUDES})

# === Putting It All Together =================================================

target_sources(MetaHack
               PUBLIC
               ${PROJECT_SOURCES}
               ${PROJECT_INCLUDES}
               ${ADDITIONAL_SOURCES})

source_group("Actions"
             FILES
             ${PROJECT_SOURCES_ACTIONS}
             ${PROJECT_INCLUDES_ACTIONS})
source_group("Components"
             FILES
             ${PROJECT_SOURCES_COMPONENTS}
             ${PROJECT_INCLUDES_COMPONENTS})
source_group("Entities"
             FILES
             ${PROJECT_SOURCES_ENTITY}
             ${PROJECT_INCLUDES_ENTITY})
source_group("Game" FILES ${PROJECT_SOURCES_GAME} ${PROJECT_INCLUDES_GAME})
source_group("Game Windows"
             FILES
             ${PROJECT_SOURCES_GAME_WINDOWS}
             ${PROJECT_INCLUDES_GAME_WINDOWS})
source_group("GUI" FILES ${PROJECT_SOURCES_GUI} ${PROJECT_INCLUDES_GUI})
source_group("Infrastructure"
             FILES
             ${PROJECT_SOURCES_INFRASTRUCTURE}
             ${PROJECT_INCLUDES_INFRASTRUCTURE})
source_group("Inventory"
             FILES
             ${PROJECT_SOURCES_INVENTORY}
             ${PROJECT_INCLUDES_INVENTORY})
source_group("Map" FILES ${PROJECT_SOURCES_MAP} ${PROJECT_INCLUDES_MAP})
source_group("Services"
             FILES
             ${PROJECT_SOURCES_SERVICES}
             ${PROJECT_INCLUDES_SERVICES})
source_group("Systems"
             FILES
             ${PROJECT_SOURCES_SYSTEMS}
             ${PROJECT_INCLUDES_SYSTEMS})
source_group("Types" FILES ${PROJECT_SOURCES_TYPES} ${PROJECT_INCLUDES_TYPES})
source_group("Views" FILES ${PROJECT_SOURCES_VIEWS} ${PROJECT_INCLUDES_VIEWS})

if(APPLE)
  set(RESOURCES_PATH $<TARGET_FILE_DIR:MetaHack>/../Resources)
else()
  set(RESOURCES_PATH $<TARGET_FILE_DIR:MetaHack>/resources)
endif()

add_custom_command(TARGET MetaHack POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                           ${PROJECT_SOURCE_DIR}/resources ${RESOURCES_PATH})

message("LIBRARY_INCLUDES = ${LIBRARY_INCLUDES}")
message("ADDITIONAL_SOURCES = ${ADDITIONAL_SOURCES}")
message("LIBRARIES = ${LIBRARIES}")
message("TARGET_SPECIFIC_LIBS = ${TARGET_SPECIFIC_LIBS}")

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
