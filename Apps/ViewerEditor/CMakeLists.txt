set(TARGET_NAME ViewerEditor)


# just for qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_MODULE_PATH "D:/ThirdParty/cmake" ${CMAKE_MODULE_PATH})

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        OpenGL
        REQUIRED)
find_package(Qt5AdvancedDocking REQUIRED HINTS "D:/ThirdParty/QtADS/lib/cmake/qt5advanceddocking")
find_package(SARibbon REQUIRED HINTS "D:/ThirdParty/SARibbon/cmake")
find_package(VCG REQUIRED)
find_package(OpenSceneGraph 2.0.0 REQUIRED osgViewer osgDB osgUtil osgQt5 osgGA osgEarth osgText)
find_package(Boost COMPONENTS filesystem system thread REQUIRED)

# for qt
# set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE CPP_HEAD_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE CPP_SOUCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE UI_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)
set(QRC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Resources/resources.qrc)
set(APPRC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Resources/appicon.rc)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${CPP_HEAD_FILES} ${CPP_SOUCE_FILES} ${UI_FILES})

add_executable(${TARGET_NAME} ${CPP_HEAD_FILES} ${CPP_SOUCE_FILES} ${UI_FILES} ${QRC_FILE} ${APPRC_FILE})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER Apps)

target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

target_link_libraries(${TARGET_NAME} PRIVATE 
Qt5::Core
Qt5::Gui
Qt5::Widgets
Qt5::OpenGL
ads::qt5advanceddocking
Boost::system
Boost::filesystem
Boost::dynamic_linking
Boost::thread
${OPENSCENEGRAPH_LIBRARIES}
OpenMP::OpenMP_CXX
)

target_include_directories(
  ${TARGET_NAME} 
  PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Sources>
  ${OPENSCENEGRAPH_INCLUDE_DIRS}
)