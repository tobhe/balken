add_library( balken STATIC
  ${CMAKE_SOURCE_DIR}/src/mser.cc
  )

find_package( blaze REQUIRED )
find_package( Threads REQUIRED )
find_package( X11 REQUIRED )

target_link_libraries( balken
  PRIVATE Threads::Threads ${X11_LIBRARIES}
  PUBLIC blaze::blaze
  )

target_include_directories( balken
  PUBLIC .
  ${CMAKE_SOURCE_DIR}/thirdparty/CImg
)
