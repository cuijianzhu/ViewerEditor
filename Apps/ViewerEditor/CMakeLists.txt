# 查找 Qt 包信息备用
set(TARGET_NAME ViewerEditor)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

find_package(Qt5 COMPONENTS Core Widgets Gui OpenGL Concurrent CONFIG REQUIRED)
find_package(Boost REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets Gui OpenGL CONFIG REQUIRED)
find_package(TBB REQUIRED)
find_package(vcglib REQUIRED)
# 开启 MOC
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


# 扫描当前源代码文件列表

file(GLOB_RECURSE CPP_HEAD_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE CPP_SOUCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE UI_SOUCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)
set(OSGQT_HEAD 
${CMAKE_CURRENT_SOURCE_DIR}/Sources/Vendors/osgQt/osgQOpenGL/osgQOpenGLWidget
${CMAKE_CURRENT_SOURCE_DIR}/Sources/Vendors/osgQt/osgQOpenGL/osgQOpenGLWindow
${CMAKE_CURRENT_SOURCE_DIR}/Sources/Vendors/osgQt/osgQOpenGL/OSGRenderer
)
set(QRC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Resources/resources.qrc)
set(APPRC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Resources/appicon.rc)
set(Translatefile ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Translations/zh_CN.ts)

QT5_WRAP_CPP(MOC_SOURCES ${OSGQT_HEAD})


source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${CPP_HEAD_FILES} ${CPP_SOUCE_FILES} ${OSGQT_HEAD})

# add_executable(${TARGET_NAME} ${CPP_HEAD_FILES} ${CPP_SOUCE_FILES} ${UI_SOUCE_FILES} ${MOC_SOURCES} ${OSGQT_HEAD} ${QRC_FILE} ${APPRC_FILE} ${Translatefile})
add_executable(${TARGET_NAME} ${CPP_HEAD_FILES} ${CPP_SOUCE_FILES} ${UI_SOUCE_FILES} ${MOC_SOURCES} ${OSGQT_HEAD})
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER Apps)
# 链接 Conan 指向的 Qt 库文件
target_link_libraries(${TARGET_NAME} PRIVATE
Qt5::Core
Qt5::Gui
Qt5::Widgets
Qt5::OpenGL
Qt5::Concurrent
Boost::system
Boost::filesystem
Boost::dynamic_linking
Boost::thread
TBB::TBB
vcglib::vcglib
CONAN_PKG::openscenegraph
)
target_include_directories(${TARGET_NAME} PRIVATE
${CMAKE_CURRENT_LIST_DIR}/Sources/Vendors/osgQt
${CMAKE_CURRENT_LIST_DIR}/Sources
)


find_program(DEPLOYQT_EXECUTABLE windeployqt)
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
  COMMAND ${DEPLOYQT_EXECUTABLE} ${CMAKE_BINARY_DIR}/bin/${TARGET_NAME}.exe
  COMMENT "Deploying qt dependencies after build..."
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)


add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/Resources
        ${CMAKE_BINARY_DIR}/bin
    COMMENT "Copying resources to output directory"
)

find_program(LUPDATE_EXE lupdate)
add_custom_target(
      TranslateUpdate
      COMMAND ${LUPDATE_EXE} -recursive ./ -ts ./QResources/Translations/zh_CN.ts
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      VERBATIM)

find_program(LRELEASE_EXE lrelease)
add_custom_target(
      TranslateRelease
      COMMAND ${LRELEASE_EXE} ./QResources/Translations/zh_CN.ts
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      VERBATIM)

set_target_properties(TranslateUpdate PROPERTIES FOLDER Apps)
set_target_properties(TranslateRelease PROPERTIES FOLDER Apps)

target_compile_definitions(${TARGET_NAME} PUBLIC "NOMINMAX")