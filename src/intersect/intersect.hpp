#ifndef INTERSECT_HPP
#define INTERSECT_HPP

#include "../core/core.hpp"
#include "../eds/eds.hpp"


namespace intersect::improved {
  bool has_intersection(eds::EDS &eds_w, eds::EDS &eds_q);
} // namespace improved

namespace intersect::naive {
    bool has_intersection(eds::EDS &eds_w, eds::EDS &eds_q);
} // namespace intersect::naive

#endif
