add_library( balken STATIC
  ${CMAKE_SOURCE_DIR}/src/mser.cc
  )

find_package( blaze )
if( blaze_FOUND )
   target_link_libraries( balken PRIVATE blaze::blaze )
endif()

target_include_directories( balken
  PUBLIC .
)
