CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

# Enable debug symbols by default
IF (NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release")
ENDIF ()

PROJECT(MetaHack)
SET (CMAKE_CXX_STANDARD 14)

IF (CMAKE_BUILD_TYPE MATCHES "Debug")
  SET(LIBRARY_SUFFIX "-d")
ELSE ()
  SET(LIBRARY_SUFFIX "")
ENDIF ()

IF (WIN32)
  SET(TARGET_PLATFORM windows-msvc14-64bit)
  SET(TARGET_LIBRARY_EXTENSION lib)
  SET(TARGET_SPECIFIC_LIBS
    ${PROJECT_SOURCE_DIR}/libraries/sfml-2.4.2/${TARGET_PLATFORM}/flac.${TARGET_LIBRARY_EXTENSION}
    ${PROJECT_SOURCE_DIR}/libraries/sfml-2.4.2/${TARGET_PLATFORM}/freetype.${TARGET_LIBRARY_EXTENSION}
    ${PROJECT_SOURCE_DIR}/libraries/sfml-2.4.2/${TARGET_PLATFORM}/jpeg.${TARGET_LIBRARY_EXTENSION}
    ${PROJECT_SOURCE_DIR}/libraries/sfml-2.4.2/${TARGET_PLATFORM}/ogg.${TARGET_LIBRARY_EXTENSION}
    ${PROJECT_SOURCE_DIR}/libraries/sfml-2.4.2/${TARGET_PLATFORM}/openal32.${TARGET_LIBRARY_EXTENSION}
    ${PROJECT_SOURCE_DIR}/libraries/sfml-2.4.2/${TARGET_PLATFORM}/vorbis.${TARGET_LIBRARY_EXTENSION}
    ${PROJECT_SOURCE_DIR}/libraries/sfml-2.4.2/${TARGET_PLATFORM}/vorbisenc.${TARGET_LIBRARY_EXTENSION}
    ${PROJECT_SOURCE_DIR}/libraries/sfml-2.4.2/${TARGET_PLATFORM}/vorbisfile.${TARGET_LIBRARY_EXTENSION}
  )
  SET(CMAKE_PREFIX_PATH "C:/Program Files/SFML")
  #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} <whatever else we want here>")
  #SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} <whatever else we want here>")
ELSEIF (APPLE)
  SET(TARGET_PLATFORM osx-clang-64bit)
  SET(TARGET_LIBRARY_PREFIX lib)
  SET(TARGET_LIBRARY_EXTENSION a)
  #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} <whatever else we want here>")
  # pagezero_size, image_base are needed for LuaJIT to work properly
  # See: https://stackoverflow.com/questions/20858911/lua-open-returns-null-using-luajit
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pagezero_size 10000 -image_base 100000000 -framework CoreFoundation -framework CoreGraphics -framework IOKit -framework OpenGL -framework Cocoa -framework Carbon -framework freetype")
  SET(TARGET_SPECIFIC_LIBS /usr/local/lib/libjpeg.a iconv)
  SET(CMAKE_MACOSX_BUNDLE TRUE)
ELSEIF (LINUX)
  SET(TARGET_PLATFORM linux-gcc-64bit)
  SET(TARGET_LIBRARY_PREFIX lib)
  SET(TARGET_LIBRARY_EXTENSION so)
  #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} <whatever else we want here>")
  #SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} <whatever else we want here>")
ENDIF ()

MESSAGE("TARGET_PLATFORM = ${TARGET_PLATFORM}")

SET(LIBRARY_LINKDIRS
  ${PROJECT_SOURCE_DIR}/libraries/boost-1.66.0/${TARGET_PLATFORM}
  ${PROJECT_SOURCE_DIR}/libraries/luajit-2.0/${TARGET_PLATFORM}
  ${PROJECT_SOURCE_DIR}/libraries/nlohmann/${TARGET_PLATFORM}
  ${PROJECT_SOURCE_DIR}/libraries/sfml-2.4.2/${TARGET_PLATFORM}
  ${PROJECT_SOURCE_DIR}/libraries/sfgui-0.3.2/${TARGET_PLATFORM}
)

SET(LIBRARY_INCLUDES
  ${PROJECT_SOURCE_DIR}/libraries/boost-1.66.0/include
  ${PROJECT_SOURCE_DIR}/libraries/cereal
  ${PROJECT_SOURCE_DIR}/libraries/luajit-2.0/include
  ${PROJECT_SOURCE_DIR}/libraries/luajit-2.0/src
  ${PROJECT_SOURCE_DIR}/libraries/nlohmann/include
  ${PROJECT_SOURCE_DIR}/libraries/sfml-2.4.2/include
  ${PROJECT_SOURCE_DIR}/libraries/sfgui-0.3.2/include
  ${PROJECT_SOURCE_DIR}/libraries/easylogging++-9.95/include
)

SET(ADDITIONAL_SOURCES
  ${PROJECT_SOURCE_DIR}/libraries/easylogging++-9.95/include/easylogging++.cc
)

SET(LIBRARIES
  ${PROJECT_SOURCE_DIR}/libraries/boost-1.66.0/${TARGET_PLATFORM}/${TARGET_LIBRARY_PREFIX}boost_chrono.${TARGET_LIBRARY_EXTENSION}
  ${PROJECT_SOURCE_DIR}/libraries/boost-1.66.0/${TARGET_PLATFORM}/${TARGET_LIBRARY_PREFIX}boost_filesystem.${TARGET_LIBRARY_EXTENSION}
  ${PROJECT_SOURCE_DIR}/libraries/boost-1.66.0/${TARGET_PLATFORM}/${TARGET_LIBRARY_PREFIX}boost_locale.${TARGET_LIBRARY_EXTENSION}
  ${PROJECT_SOURCE_DIR}/libraries/boost-1.66.0/${TARGET_PLATFORM}/${TARGET_LIBRARY_PREFIX}boost_random.${TARGET_LIBRARY_EXTENSION}
  ${PROJECT_SOURCE_DIR}/libraries/boost-1.66.0/${TARGET_PLATFORM}/${TARGET_LIBRARY_PREFIX}boost_system.${TARGET_LIBRARY_EXTENSION}
  ${PROJECT_SOURCE_DIR}/libraries/boost-1.66.0/${TARGET_PLATFORM}/${TARGET_LIBRARY_PREFIX}boost_thread.${TARGET_LIBRARY_EXTENSION}
  ${PROJECT_SOURCE_DIR}/libraries/sfgui-0.3.2/${TARGET_PLATFORM}/${TARGET_LIBRARY_PREFIX}sfgui-s${LIBRARY_SUFFIX}.${TARGET_LIBRARY_EXTENSION}
  ${PROJECT_SOURCE_DIR}/libraries/luajit-2.0/${TARGET_PLATFORM}/${TARGET_LIBRARY_PREFIX}luajit.${TARGET_LIBRARY_EXTENSION}
)

MESSAGE("LIBRARY_LINKDIRS = ${LIBRARY_LINKDIRS}")
MESSAGE("LIBRARY_INCLUDES = ${LIBRARY_INCLUDES}")
MESSAGE("ADDITIONAL_SOURCES = ${ADDITIONAL_SOURCES}")
MESSAGE("LIBRARIES = ${LIBRARIES}")
MESSAGE("TARGET_SPECIFIC_LIBS = ${TARGET_SPECIFIC_LIBS}")

SET(PROJECT_SOURCES_ACTIONS
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
  ${PROJECT_SOURCE_DIR}/actions/ActionWield.cpp
)

SET(PROJECT_INCLUDES_ACTIONS
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
  ${PROJECT_SOURCE_DIR}/actions/ActionWield.h
)

SET(PROJECT_SOURCES_COMPONENTS
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
  ${PROJECT_SOURCE_DIR}/components/ComponentTemplate.cpp
)

SET(PROJECT_INCLUDES_COMPONENTS
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
  ${PROJECT_SOURCE_DIR}/components/ComponentTemplate.h
)

SET(PROJECT_SOURCES_ENTITY
  ${PROJECT_SOURCE_DIR}/entity/EntityFactory.cpp
  ${PROJECT_SOURCE_DIR}/entity/EntityId.cpp
)

SET(PROJECT_INCLUDES_ENTITY
  ${PROJECT_SOURCE_DIR}/entity/EntityFactory.h
  ${PROJECT_SOURCE_DIR}/entity/EntityId.h
)

SET(PROJECT_SOURCES_GAME
  ${PROJECT_SOURCE_DIR}/game/App.cpp
  ${PROJECT_SOURCE_DIR}/game/AppState.cpp
  ${PROJECT_SOURCE_DIR}/game/AppStateGameMode.cpp
  ${PROJECT_SOURCE_DIR}/game/AppStateMainMenu.cpp
  ${PROJECT_SOURCE_DIR}/game/AppStateSplashScreen.cpp
  ${PROJECT_SOURCE_DIR}/game/GameState.cpp
)

SET(PROJECT_INCLUDES_GAME
  ${PROJECT_SOURCE_DIR}/game/App.h
  ${PROJECT_SOURCE_DIR}/game/AppState.h
  ${PROJECT_SOURCE_DIR}/game/AppStateGameMode.h
  ${PROJECT_SOURCE_DIR}/game/AppStateMainMenu.h
  ${PROJECT_SOURCE_DIR}/game/AppStateSplashScreen.h
  ${PROJECT_SOURCE_DIR}/game/GameState.h
)

SET(PROJECT_SOURCES_GAME_WINDOWS
  ${PROJECT_SOURCE_DIR}/game_windows/MessageLogView.cpp
  ${PROJECT_SOURCE_DIR}/game_windows/StatusArea.cpp
)

SET(PROJECT_INCLUDES_GAME_WINDOWS
  ${PROJECT_SOURCE_DIR}/game_windows/MessageLogView.h
  ${PROJECT_SOURCE_DIR}/game_windows/StatusArea.h
)

SET(PROJECT_SOURCES_GUI
  ${PROJECT_SOURCE_DIR}/gui/GUICloseHandle.cpp
  ${PROJECT_SOURCE_DIR}/gui/GUICollapseHandle.cpp
  ${PROJECT_SOURCE_DIR}/gui/GUIDesktop.cpp
  ${PROJECT_SOURCE_DIR}/gui/GUILabel.cpp
  ${PROJECT_SOURCE_DIR}/gui/GUIObject.cpp
  ${PROJECT_SOURCE_DIR}/gui/GUIResizeHandle.cpp
  ${PROJECT_SOURCE_DIR}/gui/GUIShrinkHandle.cpp
  ${PROJECT_SOURCE_DIR}/gui/GUITitleBar.cpp
  ${PROJECT_SOURCE_DIR}/gui/GUIWindow.cpp
)

SET(PROJECT_INCLUDES_GUI
  ${PROJECT_SOURCE_DIR}/gui/GUICloseHandle.h
  ${PROJECT_SOURCE_DIR}/gui/GUICollapseHandle.h
  ${PROJECT_SOURCE_DIR}/gui/GUIDesktop.h
  ${PROJECT_SOURCE_DIR}/gui/GUILabel.h
  ${PROJECT_SOURCE_DIR}/gui/GUIObject.h
  ${PROJECT_SOURCE_DIR}/gui/GUIResizeHandle.h
  ${PROJECT_SOURCE_DIR}/gui/GUIShrinkHandle.h
  ${PROJECT_SOURCE_DIR}/gui/GUITitleBar.h
  ${PROJECT_SOURCE_DIR}/gui/GUIWindow.h
)

SET(PROJECT_SOURCES_INFRASTRUCTURE
  ${PROJECT_SOURCE_DIR}/main.cpp
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
  ${ADDITIONAL_SOURCES}
)

SET(PROJECT_INCLUDES_INFRASTRUCTURE
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
  ${PROJECT_SOURCE_DIR}/utilities/StringTransforms.h
)

SET(PROJECT_SOURCES_INVENTORY
  ${PROJECT_SOURCE_DIR}/inventory/InventoryArea.cpp
  ${PROJECT_SOURCE_DIR}/inventory/InventorySelection.cpp
  ${PROJECT_SOURCE_DIR}/inventory/InventorySlot.cpp
)

SET(PROJECT_INCLUDES_INVENTORY
  ${PROJECT_SOURCE_DIR}/inventory/InventoryArea.h
  ${PROJECT_SOURCE_DIR}/inventory/InventorySelection.h
  ${PROJECT_SOURCE_DIR}/inventory/InventorySlot.h
)

SET(PROJECT_SOURCES_MAP
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
  ${PROJECT_SOURCE_DIR}/maptile/MapTile.cpp
)

SET(PROJECT_INCLUDES_MAP
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
  ${PROJECT_SOURCE_DIR}/maptile/MapTile.h
)

SET(PROJECT_SOURCES_SERVICES
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
  ${PROJECT_SOURCE_DIR}/services/Strings.cpp
)

SET(PROJECT_INCLUDES_SERVICES
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
  ${PROJECT_SOURCE_DIR}/services/Strings.h
)

SET(PROJECT_SOURCES_SYSTEMS
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
  ${PROJECT_SOURCE_DIR}/systems/SystemTimekeeper.cpp
)

SET(PROJECT_INCLUDES_SYSTEMS
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
  ${PROJECT_SOURCE_DIR}/systems/SystemTimekeeper.h
)

SET(PROJECT_SOURCES_TYPES
  ${PROJECT_SOURCE_DIR}/types/BodyPart.cpp
  ${PROJECT_SOURCE_DIR}/types/Color.cpp
  ${PROJECT_SOURCE_DIR}/types/Direction.cpp
  ${PROJECT_SOURCE_DIR}/types/EntitySpecs.cpp
  ${PROJECT_SOURCE_DIR}/types/Gender.cpp
  ${PROJECT_SOURCE_DIR}/types/IntegerRange.cpp
  ${PROJECT_SOURCE_DIR}/types/MapMemoryChunk.cpp
  ${PROJECT_SOURCE_DIR}/types/ModifiableBool.cpp
  ${PROJECT_SOURCE_DIR}/types/ModifiableInt.cpp
  ${PROJECT_SOURCE_DIR}/types/Vec2.cpp
)

SET(PROJECT_INCLUDES_TYPES
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
  ${PROJECT_SOURCE_DIR}/types/Vec3.h
)

SET(PROJECT_SOURCES_VIEWS
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
  ${PROJECT_SOURCE_DIR}/views/Standard2DGraphicViews.cpp
)

SET(PROJECT_INCLUDES_VIEWS
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
  ${PROJECT_SOURCE_DIR}/views/Standard2DGraphicViews.h
)

SET(PROJECT_SOURCES
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
  ${PROJECT_SOURCES_VIEWS}
)

SET(PROJECT_INCLUDES
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
  ${PROJECT_INCLUDES_VIEWS}
)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/design_patterns
  ${PROJECT_SOURCE_DIR}/gui
  ${PROJECT_SOURCE_DIR}/include
  ${LIBRARY_INCLUDES}
)

SET(EXECUTABLE_NAME "MetaHack")
ADD_EXECUTABLE(MetaHack ${PROJECT_SOURCES} ${PROJECT_INCLUDES} ${ADDITIONAL_SOURCES})

SOURCE_GROUP("Actions" FILES ${PROJECT_SOURCES_ACTIONS} ${PROJECT_INCLUDES_ACTIONS})
SOURCE_GROUP("Components" FILES ${PROJECT_SOURCES_COMPONENTS} ${PROJECT_INCLUDES_COMPONENTS})
SOURCE_GROUP("Entities" FILES ${PROJECT_SOURCES_ENTITY} ${PROJECT_INCLUDES_ENTITY})
SOURCE_GROUP("Game" FILES ${PROJECT_SOURCES_GAME} ${PROJECT_INCLUDES_GAME})
SOURCE_GROUP("Game Windows" FILES ${PROJECT_SOURCES_GAME_WINDOWS} ${PROJECT_INCLUDES_GAME_WINDOWS})
SOURCE_GROUP("GUI" FILES ${PROJECT_SOURCES_GUI} ${PROJECT_INCLUDES_GUI})
SOURCE_GROUP("Infrastructure" FILES ${PROJECT_SOURCES_INFRASTRUCTURE} ${PROJECT_INCLUDES_INFRASTRUCTURE})
SOURCE_GROUP("Inventory" FILES ${PROJECT_SOURCES_INVENTORY} ${PROJECT_INCLUDES_INVENTORY})
SOURCE_GROUP("Map" FILES ${PROJECT_SOURCES_MAP} ${PROJECT_INCLUDES_MAP})
SOURCE_GROUP("Services" FILES ${PROJECT_SOURCES_SERVICES} ${PROJECT_INCLUDES_SERVICES})
SOURCE_GROUP("Systems" FILES ${PROJECT_SOURCES_SYSTEMS} ${PROJECT_INCLUDES_SYSTEMS})
SOURCE_GROUP("Types" FILES ${PROJECT_SOURCES_TYPES} ${PROJECT_INCLUDES_TYPES})
SOURCE_GROUP("Views" FILES ${PROJECT_SOURCES_VIEWS} ${PROJECT_INCLUDES_VIEWS})

IF (APPLE)
  SET(RESOURCES_PATH $<TARGET_FILE_DIR:MetaHack>/../Resources)
ELSE ()
  SET(RESOURCES_PATH $<TARGET_FILE_DIR:MetaHack>/resources)
ENDIF ()

ADD_CUSTOM_COMMAND(TARGET MetaHack POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${PROJECT_SOURCE_DIR}/resources ${RESOURCES_PATH})

# Detect and add SFML
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
FIND_PACKAGE(SFML 2 REQUIRED network audio graphics window system)
IF (SFML_FOUND)
  INCLUDE_DIRECTORIES(${SFML_INCLUDE_DIR})
  TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
ENDIF ()

TARGET_LINK_LIBRARIES(MetaHack ${TARGET_SPECIFIC_LIBS} ${LIBRARIES})
