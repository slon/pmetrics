#include <chrono>

namespace pm {

typedef std::chrono::duration<double, std::chrono::system_clock::period> duration_t;
typedef std::chrono::time_point<std::chrono::system_clock, duration_t> time_point_t;

}  // namespace pm
