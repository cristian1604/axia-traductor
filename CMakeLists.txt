cmake_minimum_required(VERSION 3.16)
project(AxiaTraductor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

# wxWidgets 3.x (Ubuntu: libwxgtk3.2-dev; Windows/MSYS2: mingw-w64-x86_64-wxwidgets3.2-msw)
find_package(wxWidgets REQUIRED COMPONENTS xrc html adv core base)
include(${wxWidgets_USE_FILE})

# SFML 2.x, solo red y sistema (Ubuntu: libsfml-dev; MSYS2: mingw-w64-x86_64-sfml)
find_package(SFML 2 REQUIRED COMPONENTS network system)

add_executable(traductor WIN32
	Application.cpp
	MainWindow.cpp
	FileManager.cpp
	SyntaxColor.cpp
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
	wxfb_project.cpp
)

target_include_directories(traductor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(traductor PRIVATE ${wxWidgets_LIBRARIES} sfml-network sfml-system)
target_compile_options(traductor PRIVATE -Wall -Wno-unused-local-typedefs -Wno-unused-parameter)

if(WIN32)
	target_link_libraries(traductor PRIVATE ws2_32)
endif()

# El ejecutable busca "resources/" y "settings.json" en el directorio de trabajo,
# por lo que hay que ejecutarlo desde la raiz del repositorio (ver README).

# Tests (sin dependencia de wxWidgets): ctest --test-dir build
enable_testing()
add_executable(test_renumber tests/test_renumber.cpp Renumber.cpp)
add_test(NAME renumber COMMAND test_renumber ${CMAKE_CURRENT_SOURCE_DIR}/tests/data)
add_executable(test_translator tests/test_translator.cpp TranslatorCore.cpp Renumber.cpp)
add_test(NAME translator COMMAND test_translator ${CMAKE_CURRENT_SOURCE_DIR}/tests)
add_executable(test_sender tests/test_sender.cpp Sender.cpp Machines.cpp)
target_link_libraries(test_sender PRIVATE sfml-network sfml-system)
add_test(NAME sender COMMAND test_sender)
