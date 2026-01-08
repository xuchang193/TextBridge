qt5_add_resources(RES_SOURCES "resources.qrc")

set(PROJECT_SOURCES
  main.cpp
  inputwindow.cpp
  inputwindow.h
  keyboardhook.cpp
  keyboardhook.h
  historymanager.cpp
  historymanager.h
  trayiconmanager.cpp
  trayiconmanager.h
  bootmanager.cpp
  bootmanager.h
  ${RES_SOURCES}
)

if(WIN32)
  add_executable(TextBridge WIN32
    ${PROJECT_SOURCES}
  )
else()
  add_executable(TextBridge
    ${PROJECT_SOURCES}
  )
endif()

target_link_libraries(TextBridge PRIVATE Qt5::Widgets)
