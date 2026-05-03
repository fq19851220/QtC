cmake_minimum_required(VERSION 3.21)
project(WeCahtCloneChatGpt2分ui LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


# === Qt support ===
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OPTIONAL_COMPONENTS AxBase AxContainer Bluetooth Charts Concurrent Gui Help Multimedia Network OpenGL OpenGLWidgets PrintSupport SerialPort Sql StateMachine Svg SvgWidgets Test UiTools WebSockets Xml)

qt_add_executable(WeCahtCloneChatGpt2_ui
    qt_main.cpp
    _krnln_unity.cpp
    DLL.cpp
    代码/程序集.static.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/monocypher/monocypher.c
)
if(NOT ANDROID)
  set_target_properties(WeCahtCloneChatGpt2_ui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/$<CONFIG>"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/$<CONFIG>"
    BUNDLE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/$<CONFIG>"
  )
endif()
if(ANDROID)
  # Android package name, app label, and activity name
  set_target_properties(WeCahtCloneChatGpt2_ui PROPERTIES
    QT_ANDROID_PACKAGE_NAME "com.eqt.w_e_c_a_h_t_c_l_o_n_e_c_h_a_t_g_p_t_2_fen_u_i"
    QT_ANDROID_APPLICATION_LABEL [=[WeCahtCloneChatGpt2分ui]=]
  )
endif()
if(APPLE)
  set_target_properties(WeCahtCloneChatGpt2_ui PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME [=[WeCahtCloneChatGpt2分ui]=]
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.eqt.w_e_c_a_h_t_c_l_o_n_e_c_h_a_t_g_p_t_2_fen_u_i"
  )
endif()

set_source_files_properties(
    DLL.cpp
    代码/程序集.static.cpp
    PROPERTIES HEADER_FILE_ONLY TRUE
)

target_include_directories(WeCahtCloneChatGpt2_ui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/KrnlnLib
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/monocypher
)

if(MSVC)
  target_compile_options(WeCahtCloneChatGpt2_ui PRIVATE /utf-8)
  set_target_properties(WeCahtCloneChatGpt2_ui PROPERTIES VS_GLOBAL_VcpkgApplocalDeps false)
endif()
target_link_libraries(WeCahtCloneChatGpt2_ui PRIVATE Qt6::Core Qt6::Widgets $<$<TARGET_EXISTS:Qt6::AxBase>:Qt6::AxBase> $<$<TARGET_EXISTS:Qt6::AxContainer>:Qt6::AxContainer> $<$<TARGET_EXISTS:Qt6::Bluetooth>:Qt6::Bluetooth> $<$<TARGET_EXISTS:Qt6::Charts>:Qt6::Charts> $<$<TARGET_EXISTS:Qt6::Concurrent>:Qt6::Concurrent> $<$<TARGET_EXISTS:Qt6::Gui>:Qt6::Gui> $<$<TARGET_EXISTS:Qt6::Help>:Qt6::Help> $<$<TARGET_EXISTS:Qt6::Multimedia>:Qt6::Multimedia> $<$<TARGET_EXISTS:Qt6::Network>:Qt6::Network> $<$<TARGET_EXISTS:Qt6::OpenGL>:Qt6::OpenGL> $<$<TARGET_EXISTS:Qt6::OpenGLWidgets>:Qt6::OpenGLWidgets> $<$<TARGET_EXISTS:Qt6::PrintSupport>:Qt6::PrintSupport> $<$<TARGET_EXISTS:Qt6::SerialPort>:Qt6::SerialPort> $<$<TARGET_EXISTS:Qt6::Sql>:Qt6::Sql> $<$<TARGET_EXISTS:Qt6::StateMachine>:Qt6::StateMachine> $<$<TARGET_EXISTS:Qt6::Svg>:Qt6::Svg> $<$<TARGET_EXISTS:Qt6::SvgWidgets>:Qt6::SvgWidgets> $<$<TARGET_EXISTS:Qt6::Test>:Qt6::Test> $<$<TARGET_EXISTS:Qt6::UiTools>:Qt6::UiTools> $<$<TARGET_EXISTS:Qt6::WebSockets>:Qt6::WebSockets> $<$<TARGET_EXISTS:Qt6::Xml>:Qt6::Xml>)
target_compile_definitions(WeCahtCloneChatGpt2_ui PRIVATE E2CPP_USE_QT)

# === EQt Project Settings BEGIN ===
if(MSVC)
  # Set WeCahtCloneChatGpt2_ui as the default startup project
  set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT "WeCahtCloneChatGpt2_ui")
endif()
if(WIN32)
  set_target_properties(WeCahtCloneChatGpt2_ui PROPERTIES WIN32_EXECUTABLE FALSE)
endif()
# === EQt Project Settings END ===
