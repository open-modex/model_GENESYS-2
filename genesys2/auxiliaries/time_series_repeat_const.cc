/*
 * time_series_repeat_const.cc
 *
 *  Created on: Nov 16, 2015
 *      Author: pst
 */

#include <auxiliaries/time_series_repeat_const.h>

namespace aux {

double TimeSeriesRepeatConst::operator[](const SimulationClock::time_point& time_point) const {
  if (time_point >= start()) {
    return (value(locate_time(time_point) % size()));
  } else {
    return (0.0);
  }
}

double TimeSeriesRepeatConst::Mean(SimulationClock::time_point tp_start,
                                   SimulationClock::time_point tp_end) const {
  double rval = 0.0;
  if (tp_end > start()) { // Mean != 0 only for tp_end past start of TS
    auto start_index = locate_time(tp_start);
    auto end_index = locate_time(tp_end);
    if (start_index == end_index)
      return value(start_index % size());
    double sum_of_prev_bases = 0.0;
    if (tp_start >= start()) { // otherwise, value = 0 between tp_start and start of TS
      rval += value(start_index % size());
      sum_of_prev_bases += (start() + ++start_index * interval() - tp_start).count();
    } else {
      start_index = 0;
      sum_of_prev_bases += (start()  - tp_start).count();
    }
    unsigned int i = 0;
    double tmp_sum = 0.0;
    for (; start_index + i < end_index; ++i)
      tmp_sum += value((start_index + i) % size());
    double current_base = interval().count() * i;
    sum_of_prev_bases += current_base;
    rval += (current_base / sum_of_prev_bases) * (tmp_sum / i - rval);
    current_base = (tp_end - start() - end_index * interval()).count();
    rval += (current_base / (sum_of_prev_bases + current_base)) * (value(end_index % size()) - rval);
  }
  return rval;
}

}/* namespace aux */
