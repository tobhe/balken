#include <blaze/math/DynamicMatrix.h>
#include <celero/Celero.h>
#include "filter.h"

auto rand_img =
  blaze::Rand<blaze::DynamicMatrix<uint8_t, blaze::rowMajor>>().generate(1920,
                                                                         1080);

auto kernel = blaze::DynamicMatrix<uint8_t, blaze::rowMajor>(5, 5, 1);


CELERO_MAIN

BASELINE(BenchmarkFilters, gauss, 100, 10) {
  celero::DoNotOptimizeAway(balken::filter::gauss(rand_img));
}

BENCHMARK(BenchmarkFilters, sobel, 100, 10) {
  celero::DoNotOptimizeAway(
    balken::filter::convolve(rand_img, balken::filter::detail::sobel_x));
}

BENCHMARK(BenchmarkFilters, erode, 100, 10) {
  celero::DoNotOptimizeAway(balken::filter::erode(rand_img, kernel));
}

BENCHMARK(BenchmarkFilters, dilate, 100, 10) {
  celero::DoNotOptimizeAway(balken::filter::dilate(rand_img, kernel));
}

BENCHMARK(BenchmarkFilters, open, 100, 10) {
  celero::DoNotOptimizeAway(balken::filter::open(rand_img, kernel));
}
BENCHMARK(BenchmarkFilters, close, 100, 10) {
  celero::DoNotOptimizeAway(balken::filter::close(rand_img, kernel));
}
