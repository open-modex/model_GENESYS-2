/*
 * date_value_pairs_const.cc
 *
 *  Created on: Nov 16, 2015
 *      Author: pst
 */

#include <auxiliaries/date_value_pairs_const.h>

namespace aux {

double DateValuePairsConst::operator [](const SimulationClock::time_point& time_point) const {
  if (time_point >= time(static_cast<index_type>(0))) {
    return data(locate_time(time_point));
  } else {
    return 0.0;
  }
}

double DateValuePairsConst::Mean(SimulationClock::time_point tp_start,
                                 SimulationClock::time_point tp_end) const {
  double rval = 0.0;
  auto it = data().cbegin();
  if (tp_end > it->first) { // Mean != 0 only for tp_end past first timepoint of DVP
    if (tp_start > (--data().cend())->first) // tp_start is past last timepoint of DVP, Mean is equal to last value of DVP
      return (--data().cend())->second;
    if (tp_start > it->first) { // otherwise, value = 0 between tp_start and first timepoint of DVP
      while (it + 1 < data().cend() && it->first <= tp_start)
        ++it;
      rval += (it - 1)->second; // subtraction OK, since it gets incremented at least once in while loop just before
    }
    if (it->first >= tp_end)
      return rval;
    double current_base = (it->first - tp_start).count();
    double sum_of_prev_bases = current_base;
    while (it + 1 < data().cend() && (it + 1)->first < tp_end) {
      auto current_tp = it->first;
      auto current_value = it->second;
      current_base = ((++it)->first - current_tp).count();
      sum_of_prev_bases += current_base;
      rval += (current_base / sum_of_prev_bases) * (current_value - rval);
    }
    current_base = (tp_end - it->first).count();
    sum_of_prev_bases += current_base;
    rval += (current_base / sum_of_prev_bases) * (it->second - rval);
  }
  return rval;
}

} /* namespace aux */
