cmake_minimum_required(VERSION 3.16)
project(AxiaTraductor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)   # Clipper2 (booleanas de polígonos del simulador) pide C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

# ---- wxWidgets 3.2 -----------------------------------------------------------
# Primero en modo config (MSYS2, vcpkg: instala wxWidgetsConfig.cmake) y si no,
# en modo módulo (Ubuntu/Debian: wx-config).
find_package(wxWidgets CONFIG QUIET)
if(TARGET wx::core)
	set(WX_LIBS wx::gl wx::stc wx::xrc wx::html wx::core wx::base)
	set(WX_USE_FILE "")
else()
	find_package(wxWidgets REQUIRED COMPONENTS gl stc xrc html adv core base)
	set(WX_LIBS ${wxWidgets_LIBRARIES})
	set(WX_USE_FILE ${wxWidgets_USE_FILE})
endif()
# OpenGL de función fija para la vista 3D de la pieza (wxGLCanvas)
find_package(OpenGL REQUIRED)

# ---- SFML 2 (network, system) ------------------------------------------------
# Ubuntu 24.04 trae SFML 2.6 (libsfml-dev). MSYS2 ya distribuye SFML 3, cuya API
# es distinta, así que si no hay un SFML 2 instalado se compila desde el código
# fuente (solo los módulos network y system, en unos segundos).
find_package(SFML 2 QUIET COMPONENTS network system)
if(NOT SFML_FOUND)
	message(STATUS "SFML 2 no encontrado: se descarga y compila SFML 2.6.2 (network + system)")
	include(FetchContent)
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
	set(SFML_BUILD_GRAPHICS OFF CACHE BOOL "" FORCE)
	set(SFML_BUILD_WINDOW OFF CACHE BOOL "" FORCE)
	set(SFML_BUILD_NETWORK ON CACHE BOOL "" FORCE)
	set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
	set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(SFML_INSTALL_PKGCONFIG_FILES OFF CACHE BOOL "" FORCE)
	FetchContent_Declare(SFML
		GIT_REPOSITORY https://github.com/SFML/SFML.git
		GIT_TAG 2.6.2
		GIT_SHALLOW TRUE)
	FetchContent_MakeAvailable(SFML)
endif()

# ---- Clipper2 (booleanas de polígonos del simulador de pieza) ----------------
add_library(clipper2 STATIC
	third_party/clipper2/clipper.engine.cpp
	third_party/clipper2/clipper.offset.cpp
	third_party/clipper2/clipper.rectclip.cpp
	third_party/clipper2/clipper.triangulation.cpp
)
target_include_directories(clipper2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/clipper2)
target_compile_options(clipper2 PRIVATE -w)   # código de terceros: sin avisos

# ---- Aplicación --------------------------------------------------------------
set(APP_SOURCES
	wxApplication.cpp
	wxMainWindow.cpp
	FileManager.cpp
	wxCncEditor.cpp
	CncSyntax.cpp
	CncPath.cpp
	TurnStock.cpp
	ToolsFile.cpp
	wxPlotPanel.cpp
	wxTurnView3D.cpp
	Translator.cpp
	TranslatorCore.cpp
	Renumber.cpp
	wxOptions.cpp
	wxSearch.cpp
	wxSearchReplace.cpp
	wxAbout.cpp
	wxActualizaciones.cpp
	wxSendWindow.cpp
	Sender.cpp
	Machines.cpp
	AppPaths.cpp
)
if(WIN32)
	# Icono de la aplicación y manifiesto (controles comunes v6)
	list(APPEND APP_SOURCES traductor.rc)
	# El .rc incluye el manifiesto: si cambia, hay que recompilar el recurso
	set_source_files_properties(traductor.rc PROPERTIES OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/manifest.xml)
endif()

# Código de la interfaz generado por wxUiEditor (traductor.wxui): la lista la
# escribe el propio diseñador en wxui_code.cmake al generar el código.
include(${CMAKE_CURRENT_SOURCE_DIR}/wxui_code.cmake)
add_executable(traductor WIN32 ${APP_SOURCES} ${wxue_generated_code})
if(WX_USE_FILE)
	include(${WX_USE_FILE})
endif()
target_include_directories(traductor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
find_package(Threads REQUIRED)   # consulta de actualizaciones en un hilo
target_link_libraries(traductor PRIVATE ${WX_LIBS} OpenGL::GL sfml-network sfml-system Threads::Threads clipper2)
target_compile_options(traductor PRIVATE -Wall -Wno-unused-local-typedefs -Wno-unused-parameter)

if(WIN32)
	target_link_libraries(traductor PRIVATE ws2_32)
	if(MINGW)
		# Runtime de GCC enlazado estático: menos DLLs que distribuir
		target_link_options(traductor PRIVATE -static-libgcc -static-libstdc++)
	endif()
endif()

# La aplicación fija su directorio de trabajo en la carpeta del ejecutable y
# carga los iconos de "resources/" desde ahí: se copian junto al ejecutable.
add_custom_command(TARGET traductor POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:traductor>/resources
	COMMENT "Copiando resources/ junto al ejecutable")

# ---- Tests (sin dependencia de wxWidgets): ctest --test-dir build ------------
enable_testing()
add_executable(test_syntax tests/test_syntax.cpp CncSyntax.cpp)
add_test(NAME syntax COMMAND test_syntax)
add_executable(test_renumber tests/test_renumber.cpp Renumber.cpp)
add_test(NAME renumber COMMAND test_renumber ${CMAKE_CURRENT_SOURCE_DIR}/tests/data)
add_executable(test_translator tests/test_translator.cpp TranslatorCore.cpp Renumber.cpp)
add_test(NAME translator COMMAND test_translator ${CMAKE_CURRENT_SOURCE_DIR}/tests)
add_executable(test_sender tests/test_sender.cpp Sender.cpp Machines.cpp)
target_link_libraries(test_sender PRIVATE sfml-network sfml-system)
if(WIN32)
	target_link_libraries(test_sender PRIVATE ws2_32)
endif()
add_test(NAME sender COMMAND test_sender)
add_executable(test_path tests/test_path.cpp CncPath.cpp)
add_test(NAME path COMMAND test_path ${CMAKE_CURRENT_SOURCE_DIR}/tests)
add_executable(test_stock tests/test_stock.cpp TurnStock.cpp ToolsFile.cpp CncPath.cpp)
target_link_libraries(test_stock PRIVATE clipper2)
add_test(NAME stock COMMAND test_stock ${CMAKE_CURRENT_SOURCE_DIR}/tests)
# Herramienta de depuración del intérprete: cncpath_dump <programa> [8025|8035|fanuc]
add_executable(cncpath_dump tests/cncpath_dump.cpp CncPath.cpp TurnStock.cpp)
target_link_libraries(cncpath_dump PRIVATE clipper2)
# Vista previa del graficador con wxWidgets: plot_preview <programa> [8025|8035|fanuc]
add_executable(plot_preview WIN32 tests/plot_preview.cpp wxPlotPanel.cpp wxTurnView3D.cpp CncPath.cpp TurnStock.cpp)
target_include_directories(plot_preview PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(plot_preview PRIVATE ${WX_LIBS} OpenGL::GL clipper2)
target_compile_options(plot_preview PRIVATE -Wall -Wno-unused-local-typedefs -Wno-unused-parameter)
if(MINGW)
	target_link_options(plot_preview PRIVATE -static-libgcc -static-libstdc++)
endif()
