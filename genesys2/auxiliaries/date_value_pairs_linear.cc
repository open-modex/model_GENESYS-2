/*
 * date_value_pairs_linear.cc
 *
 *  Created on: Nov 16, 2015
 *      Author: pst
 */

#include <auxiliaries/date_value_pairs_linear.h>

namespace aux {

double DateValuePairsLinear::operator [](const SimulationClock::time_point& time_point) const {
  if (time_point >= time(static_cast<index_type>(0))) {
    if (time_point < time(pair_count() - 1)) {
      double fraction = 0.;
      auto index = locate_time(time_point, fraction);
      return (interpolate_linear(data(index), data(index + 1), fraction));
    } else {
      return (data(pair_count() - 1));
    }
  } else {
    return (0.0);
  }
}

double DateValuePairsLinear::Mean(SimulationClock::time_point tp_start,
                                  SimulationClock::time_point tp_end) const {
  double rval = 0.0;
  auto it = data().cbegin();
  if (tp_end > it->first) { // Mean != 0 only for 'tp_end' past first timepoint of DVP
    if (tp_start > (--data().cend())->first) // tp_start is past last timepoint of DVP, Mean is equal to last value of DVP
      return (--data().cend())->second;
    double current_base = (it->first - tp_start).count();
    if (tp_start > it->first) { // otherwise, value = 0 between 'tp_start' and first timepoint of DVP
      while (it + 1 < data().cend() && it->first <= tp_start)
        ++it;
      auto next_value = it->second;
      current_base = (it->first - tp_start).count();
      if (it->first >= tp_end) { // subtraction from it OK, since it gets incremented at least once in while loop just before
        return 0.5 * (interpolate_linear(next_value, (it - 1)->second, (it->first - tp_end).count()
                                         / static_cast<double>((it->first - (it - 1)->first).count()))
            + interpolate_linear(next_value, (it - 1)->second, current_base
                                 / static_cast<double>((it->first - (it - 1)->first).count())));
      } else {
        rval += 0.5 * (next_value + interpolate_linear(next_value, (it - 1)->second, current_base
                                                       / static_cast<double>((it->first - (it - 1)->first).count())));
      }
    }
    double sum_of_prev_bases = current_base;
    while (it + 1 < data().cend() && (it + 1)->first < tp_end) {
      auto current_tp = it->first;
      auto current_value = it->second;
      current_base = ((++it)->first - current_tp).count();
      sum_of_prev_bases += current_base;
      rval += (current_base / sum_of_prev_bases) * (0.5 * (current_value + it->second) - rval);
    }
    current_base = (tp_end - it->first).count();
    sum_of_prev_bases += current_base;
    if (it + 1 < data().cend()) {
      rval += (current_base / sum_of_prev_bases) * (0.5 * (it->second
          + interpolate_linear(it->second, (it + 1)->second, current_base
                               / static_cast<double>(((it + 1)->first - it->first).count()))) - rval);
    } else {
      rval += (current_base / sum_of_prev_bases) * (it->second - rval);
    }
  }
  return rval;
}

} /* namespace aux */
