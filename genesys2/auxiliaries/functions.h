/*
 * functions.h
 *
 *  Created on: Jul 21, 2016
 *      Author: pst
 */

#ifndef AUXILIARIES_FUNCTIONS_H_
#define AUXILIARIES_FUNCTIONS_H_

#include <algorithm>
#include <numeric>
#include <string>
#include <unordered_map>

#include <auxiliaries/simulation_clock.h>
#include <auxiliaries/time_based_data.h>

namespace aux {

// COLLECTION OF FUNCTIONS THAT CURRENTLY ARE NEEDED, BUT NOT LOCATEABLE TO A CERTAIN CLASS
// TRY TO _NOT_ PLACE ANYTHING HERE AND IF YOU DO SO, PLEASE KEEP CHECKING, IF YOU CAN MOVE IT INTO SOME CLASS LATER

/**
 * @brief Discounting the time based data by a given rate
 *
 * @param[in] tbd Time based data
 * @param[in] present Start time point
 * @param[in] horizon End time point
 * @param[in] period Observation period
 * @param[in] rate Interest rate
 * @param[in] present_day: if start date is not equal to the date which should be discounted to
 *
 * @return Discounted time based data
 */

double DiscountFuture2(const aux::TimeBasedData& tbd,
                      aux::SimulationClock::time_point start_discount_horizon,
                      aux::SimulationClock::time_point end_discount_horizon,
                      aux::SimulationClock::duration period,
                      double interest_rate,
                      aux::SimulationClock::time_point present_day);

double timed_value_annual(double value,
                   aux::SimulationClock::time_point present_day,
                   aux::SimulationClock::time_point other_day,
                   double interest_rate);

double anf(const double interest_rate,
           const int lifetime);

double sum_means(const aux::TimeBasedData& tbd,
                 aux::SimulationClock::time_point start,
                 aux::SimulationClock::time_point end,
                 aux::SimulationClock::duration interval);

double sum(const aux::TimeBasedData& tbd,
           aux::SimulationClock::time_point start,
           aux::SimulationClock::time_point end,
           aux::SimulationClock::duration interval);

double sum(const aux::TimeBasedData& tbd,
           aux::SimulationClock::time_point start,
           aux::SimulationClock::time_point end,
           aux::SimulationClock::duration interval);

double sum_positiveValues(const aux::TimeBasedData& tbd,
           aux::SimulationClock::time_point start,
           aux::SimulationClock::time_point end,
           aux::SimulationClock::duration interval);

/**
 * @brief Controls if the difference of two values satisfies the given precision interval
 *
 * @param[in] a First value
 * @param[in] b Second value
 * @param[in] precision Precision interval to fulfill
 */
bool cmp_equal(double a,
               double b,
               double precision);

/*
template <typename T>
std::vector<size_t> indices_sorted(const std::vector<T>& v) {
  // initialize original index locations
  std::vector<size_t> idx(v.size());
  std::iota(idx.begin(), idx.end(), 0);
  // sort indexes based on comparing values in v
  std::sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});
  return idx;
}
*/

std::string pretty_time_string(int time_msec);

//could also be integrated in templated form
void print_map_elements (const std::unordered_map<std::string, bool>&);

} /* namespace aux */

#endif /* AUXILIARIES_FUNCTIONS_H_ */
