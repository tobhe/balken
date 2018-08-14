// cpp
#include <queue>
#include <stack>

// external
#include <blaze/math/DynamicMatrix.h>

// own
#include "mser.h"

namespace balken {
namespace mser {
namespace detail {

/**
 * \brief Priority queue of pixels
 *
 * A priority queue of pixel elements, containing the pixels location as well
 * as the pixels grey level. Value type ist the pixels location, element access
 * operations return only the pixels location. The priority ignores the
 * location and only compares the pixels grey level values.
 *
 * \tparam  CoordT  The pixels coordinates (allows both 1d and 2d addressing)
 */
template <class CoordT, class Container = std::vector<CoordT>>
class pixel_priority_queue
{
private:
  struct Element;
  using queue_type =
    std::priority_queue<Element, std::vector<Element>, std::greater<Element>>;

public:
  using value_type      = CoordT;
  using reference       = value_type &;
  using const_reference = const value_type &;
  using size_type       = typename queue_type::size_type;

private:
  struct Element
  {
    using self_t = Element;

    // Constructor
    Element(CoordT p, int level) : p{p}, level{level} {}

    // Comparison (required for priority_queue)
    // only compares levels of elements, ignores location
    bool operator==(const self_t & other) const {
      return level == other.level;
    }
    bool operator!=(const self_t & other) const { return !(*this == other); }
    bool operator<(const self_t & other) const { return level < other.level; }
    bool operator>(const self_t & other) const { return level > other.level; }
    bool operator>=(const self_t & other) const {
      return level >= other.level;
    }
    bool operator<=(const self_t & other) const {
      return level <= other.level;
    }

    // Public members
    CoordT p;      // < pixel location
    int    level;  // < pixel level
  };

public:
  // Construction
  pixel_priority_queue()  = default;
  ~pixel_priority_queue() = default;

  // Element Access
  const_reference top() const { return _queue.top().p; }

  // Capacity
  inline bool      empty() const { return _queue.empty(); }
  inline size_type size() const { return _queue.size(); }

  // Modifiers
  void push(const value_type & value, int level) {
    _queue.push(Element{value, level});
  }

  void push(value_type && value, int level) {
    _queue.emplace(std::forward<value_type>(value), level);
  }

  void pop() { _queue.pop(); }

private:
  queue_type _queue;  // < priority_queue of pixel elements
};

void process_stack(const std::pair<int, int> p,
                   const int                 new_level,
                   std::vector<Region> &     stack) {
  // 1. Process component on top of stack
  do {
    auto top = stack.back();
    stack.pop_back();

    // 2. if new_level is smaller than the grey level of the second
    // component, set top_level to new_level and return
    if (new_level < stack.back().level) {
      stack.emplace_back(p, new_level);
      stack.back().merge(top);
      return;
    }

    // 3. Remove top of the stack and merge it into the second component
    stack.back().merge(top);
  } while (new_level > stack.back().level);
}

// Detection helper functions
template <class MatrixT>
inline bool is_valid_pixel(const MatrixT & img, const std::pair<int, int> p) {
  return (std::get<0>(p) >= 0 and
          static_cast<uint16_t>(std::get<0>(p)) < img.columns()) and
         (std::get<1>(p) >= 0 and
          static_cast<uint16_t>(std::get<1>(p)) < img.rows());
}

enum class State { new_region, neighbours };

}  // namespace detail

std::vector<Region> detect_regions(const blaze::DynamicMatrix<uint8_t> & in) {
  // Algorithm data structures
  detail::pixel_priority_queue<std::pair<int, int>> _queue;
  std::vector<bool>   _accessible(in.columns() * in.rows());
  std::vector<Region> _stack;

  // 1. Push dummy object on stack
  _stack.emplace_back(std::pair(-1, -1), 256);

  // 2. Make the source pixel accessible and store
  //    it's grey level in cur_level
  std::pair cur_pixel{0, 0};
  int       cur_level = in(std::get<0>(cur_pixel), std::get<1>(cur_pixel));
  _accessible[std::get<0>(cur_pixel) + in.rows() * std::get<1>(cur_pixel)] =
    true;

  detail::State state = detail::State::new_region;

  while (true) {
    switch (state) {
      case detail::State::new_region:
        // 3. Push an empty component with cur_level on stack
        _stack.emplace_back(cur_pixel, cur_level);
        state = detail::State::neighbours;
        break;
      case detail::State::neighbours: {
        for (int cols = -1; cols <= 1; ++cols) {
          for (int rows = -1; rows <= 1; ++rows) {
            if (cols || rows) {
              auto neigh = std::pair{std::get<0>(cur_pixel) + cols,
                                     std::get<1>(cur_pixel) + rows};

              // Check if the neighbour is accessible
              if (detail::is_valid_pixel(in, neigh) and
                  (!_accessible[std::get<0>(neigh) +
                                in.columns() * std::get<1>(neigh)])) {
                _accessible[std::get<0>(neigh) +
                            in.columns() * std::get<1>(neigh)] = true;

                // If the grey level is not lower than current, push it onto
                // the heap of boundary pixels
                auto neigh_level = in(std::get<0>(neigh), std::get<1>(neigh));
                if (neigh_level >= cur_level) {
                  _queue.push(
                    std::pair{std::get<0>(neigh), std::get<1>(neigh)},
                    neigh_level);

                } else {
                  // Enter current pixel into queue and make neigh current
                  _queue.push(
                    std::pair(std::get<0>(cur_pixel), std::get<1>(cur_pixel)),
                    cur_level);
                  std::get<0>(cur_pixel) = std::get<0>(neigh);
                  std::get<1>(cur_pixel) = std::get<1>(neigh);
                  cur_level              = neigh_level;
                  // Back to 3.
                  state = detail::State::new_region;
                  break;
                }
              }
            }
          }
          // for rows
          if (state == detail::State::new_region) { break; }
        }
        // for cols
        if (state == detail::State::new_region) { break; }

        // 5. Accumulate the current pixel to the component on top of the
        // stack
        _stack.back().accumulate(cur_pixel);

        // 6. If the heap is empty: stop
        if (_queue.empty()) { return _stack; }

        // pop the head of the heap
        cur_pixel = _queue.top();

        int top_level = in(std::get<0>(cur_pixel), std::get<1>(cur_pixel));
        _queue.pop();

        // If the head grey level is the same as the current_level, go to 4
        if (top_level != cur_level) {
          cur_level = top_level;

          // 7. The returned pixel is at a higher grey level, process
          // all components on stack until higher level is reached,
          // then go to 4
          // std::cout << "Processing stack: " << _stack.size() << '\n';
          detail::process_stack(cur_pixel, top_level, _stack);
        }
        break;
      }
    }
  }
}

}  // namespace mser
}  // namespace balken
