add_library (balken INTERFACE)

find_package (Threads REQUIRED)
find_package (X11 REQUIRED)

target_link_libraries (balken
  INTERFACE Threads::Threads ${X11_LIBRARIES} blaze
  )

target_include_directories (balken
  INTERFACE . ${CMAKE_SOURCE_DIR}/thirdparty/CImg
)
