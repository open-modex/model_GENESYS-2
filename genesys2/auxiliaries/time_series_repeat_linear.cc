/*
 * time_series_repeat_linear.cc
 *
 *  Created on: Nov 16, 2015
 *      Author: pst
 */

#include <auxiliaries/time_series_repeat_linear.h>

namespace aux {

double TimeSeriesRepeatLinear::operator[](const SimulationClock::time_point& time_point) const {
  if (time_point >= start()) {
    double fraction;
    auto index = locate_time(time_point, fraction);
    return (interpolate_linear(value(index % size()),
                              value((index + 1) % size()),
                              fraction));
  } else {
    return (0.0);
  }
}

double TimeSeriesRepeatLinear::Mean(SimulationClock::time_point tp_start,
                                    SimulationClock::time_point tp_end) const {
  double rval = 0.0;
  if (tp_end > start()) { // Mean != 0 only for tp_end past start of TS
    auto start_index = locate_time(tp_start);
    auto end_index = locate_time(tp_end);
    if (start_index == end_index) {
      double current_base = interval().count();
      auto tp_start_index = start() + start_index * interval();
      return 0.5 * (interpolate_linear(value(start_index % size()), value((start_index + 1) % size()),
                                       static_cast<double>((tp_start - tp_start_index).count()) / current_base)
          + interpolate_linear(value(start_index % size()), value((start_index + 1) % size()),
                               static_cast<double>((tp_end - tp_start_index).count()) / current_base));
    }
    double sum_of_prev_bases = 0.0;
    double current_base = (start() + ++start_index * interval() - tp_start).count();
    if (tp_start >= start()) { // otherwise, value = 0 between tp_start and start of TS
      rval += 0.5 * (value(start_index % size()) + interpolate_linear(value(start_index % size()),
                                                                      value((start_index - 1) % size()), current_base
                                                                      / static_cast<double>(interval().count())));
      sum_of_prev_bases += current_base;
    } else {
      start_index = 0;
      sum_of_prev_bases += (start() - tp_start).count();
    }
    current_base = 0.5 * interval().count();
    if (start_index != end_index) {
      // shift evaluation grid by 0.5*interval to simplify computation for many intervals between tp_start and tp_end
      sum_of_prev_bases += current_base;
      rval += (current_base / sum_of_prev_bases) * (value(start_index % size()) - rval);
      unsigned int i = 1;
      double tmp_sum = 0.0;
      for (; start_index + i < end_index; ++i)
        tmp_sum += value((start_index + i) % size());
      // shift back to original evaluation grid
      sum_of_prev_bases += current_base;
      rval += (current_base / sum_of_prev_bases) * (value(end_index % size()) - rval);
      // add loop block to rval
      current_base = interval().count() * --i;
      sum_of_prev_bases += current_base;
      rval += (current_base / sum_of_prev_bases) * ((i != 0 ? tmp_sum / i : tmp_sum) - rval);
    }
    current_base = (tp_end - start() - end_index * interval()).count();
    rval += (current_base / (sum_of_prev_bases + current_base)) * (0.5 * (value(end_index % size())
        + interpolate_linear(value(end_index % size()), value((end_index + 1) % size()), current_base
                             / static_cast<double>(interval().count()))) - rval);
  }
  return rval;
}

}/* namespace aux */
