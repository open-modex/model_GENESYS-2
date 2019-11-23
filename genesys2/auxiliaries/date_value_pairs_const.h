/*
 * date_value_pairs_const.h
 *
 *  Created on: Oct 13, 2015
 *      Author: pst
 */

#ifndef AUXILIARIES_DATE_VALUE_PAIRS_CONST_H_
#define AUXILIARIES_DATE_VALUE_PAIRS_CONST_H_

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <auxiliaries/date_value_pairs.h>
#include <auxiliaries/simulation_clock.h>

namespace aux {

class DateValuePairsConst : public DateValuePairs {
 public:
  /**
   * @brief Default constructor
   */
  DateValuePairsConst() = delete;
  virtual ~DateValuePairsConst() override = default;

  /**
  * @brief Constructor
  *
  * @param[in] data Data to store in local variable
  */
  DateValuePairsConst(const std::vector<std::pair<SimulationClock::time_point, double> >& data)
      : DateValuePairs(data) {}

  /**
   * @brief Overloaded operator defining the selection of an element from the data, saved as pair of date and value -> use of constant interpolation
   * @details Constant Interpolation is used for the determination of the unknown parameters
   *
   * @param[in] time_point Local time point defining the position of a value in the time based data
   * @return The value of the local time defined by the input variable
   */
  using TimeBasedData::operator [];
  virtual double operator [](const SimulationClock::time_point& time_point) const override;

  /**
   * @brief Copies and stores the time based data in a new unique pointer
   *
   * @return Unique pointer containing the copied time based data, saved as pair of date and value
   */
  virtual std::unique_ptr<TimeBasedData> clone() const override {
    return std::unique_ptr<TimeBasedData>(new DateValuePairsConst(*this));
  }

  virtual double Mean(SimulationClock::time_point tp_start,
                      SimulationClock::time_point tp_end) const override;

  virtual std::string PrintToString() const override {return PrintToStringDVP("DVP_const");}
};

} /* namespace aux */

#endif /* AUXILIARIES_DATE_VALUE_PAIRS_H_ */

