#ifndef INTERSECT_HPP
#define INTERSECT_HPP

#include "../core/core.hpp"
#include "../eds/eds.hpp"

// TODO: find a better name and namespacing
namespace intersect {

  namespace improved {
    bool has_intersection(eds::EDS &eds_w,
                          eds::EDS &eds_q,
                          n_core::Parameters parameters);
    } // namespace improved

    namespace naive {
      bool has_intersection(eds::EDS &eds_w, eds::EDS &eds_q);
    } // namespace naive

} //namespace intersect
#endif
