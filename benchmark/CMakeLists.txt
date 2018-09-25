add_executable(benchmark_filters
  benchmark_filters.cc
  )

target_link_libraries(benchmark_filters
  PRIVATE balken celero
  )
