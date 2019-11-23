/*
 * time_series_linear.cc
 *
 *  Created on: Nov 16, 2015
 *      Author: pst
 */

#include <auxiliaries/time_series_linear.h>

namespace aux {

double TimeSeriesLinear::operator[](const SimulationClock::time_point& time_point) const {
  if (time_point >= start()) {
    if (time_point < end()) {
      double fraction;
      auto index = locate_time(time_point, fraction);
      return (interpolate_linear(value(index), value(index + 1), fraction));
    } else {
      return (value(size() - 1));
    }
  } else {
    return (0.0);
  }
}

double TimeSeriesLinear::Mean(SimulationClock::time_point tp_start,
                              SimulationClock::time_point tp_end) const {
  double rval = 0.0;
  if (tp_end > start()) { // Mean != 0 only for tp_end past start of TS
    if (tp_start > end()) // tp_start is past end of TS, Mean is equal to last value of TS
      return value(size() - 1);
    auto start_index = locate_time(tp_start);
    auto end_index = locate_time(tp_end);
    if (start_index == end_index) {
      double current_base = interval().count();
      auto tp_start_index = start() + start_index * interval();
      return 0.5 * (interpolate_linear(value(start_index), value(start_index + 1),
                                       static_cast<double>((tp_start - tp_start_index).count()) / current_base)
          + interpolate_linear(value(start_index), value(start_index + 1),
                               static_cast<double>((tp_end - tp_start_index).count()) / current_base));
    }
    double sum_of_prev_bases = 0.0;
    double current_base = (start() + ++start_index * interval() - tp_start).count();
    if (tp_start >= start()) { // otherwise, value = 0 between tp_start and start of TS
      rval += 0.5 * (value(start_index) + interpolate_linear(value(start_index), value(start_index - 1), current_base
                                                             / static_cast<double>(interval().count())));
      sum_of_prev_bases += current_base;
    } else {
      start_index = 0;
      sum_of_prev_bases += (start() - tp_start).count();
    }
    if (tp_end > end())
      end_index = size() - 1; // make sure we are not running past the end of the TS in the following for-loop
    current_base = 0.5 * interval().count();
    if (start_index != end_index) {
      // shift evaluation grid by 0.5*interval to simplify computation for many intervals between tp_start and tp_end
      sum_of_prev_bases += current_base;
      rval += (current_base / sum_of_prev_bases) * (value(start_index) - rval);
      unsigned int i = 1;
      double tmp_sum = 0.0;
      for (; start_index + i < end_index; ++i)
        tmp_sum += value(start_index + i);
      // shift back to original evaluation grid
      sum_of_prev_bases += current_base;
      rval += (current_base / sum_of_prev_bases) * (value(end_index) - rval);
      // add loop block to rval
      current_base = interval().count() * --i;
      sum_of_prev_bases += current_base;
      rval += (current_base / sum_of_prev_bases) * ((i != 0 ? tmp_sum / i : tmp_sum) - rval);
    }
    current_base = (tp_end - start() - end_index * interval()).count();
    if (tp_end < end()) {
      rval += (current_base / (sum_of_prev_bases + current_base)) * (0.5 * (value(end_index)
          + interpolate_linear(value(end_index), value(end_index + 1), current_base
                               / static_cast<double>(interval().count()))) - rval);
    } else {
      rval += (current_base / (sum_of_prev_bases + current_base)) * (value(end_index) - rval);
    }
  }
  return rval;
}

} /* namespace aux */
