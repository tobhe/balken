add_executable (example
  example.cc
  )
target_link_libraries(example
  PRIVATE balken
  )

add_executable (histogram_example
  histogram_example.cc
  )
target_link_libraries(histogram_example
  PRIVATE balken
  )
