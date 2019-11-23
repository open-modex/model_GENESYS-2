/*
 * time_based_data.h
 *
 *  Created on: Aug 5, 2015
 *      Author: pst
 */

#ifndef AUXILIARIES_TIME_BASED_DATA_H_
#define AUXILIARIES_TIME_BASED_DATA_H_

#include <memory>
#include <string>
#include <vector>

#include <auxiliaries/simulation_clock.h>

namespace aux {

class TimeBasedData {
 public:
  /**
  * @brief Default deconstructor
  */
  virtual ~TimeBasedData() = default;

  /**
   * @brief Copies and stores the time based data in a new unique pointer
   *
   * @return Unique pointer containing the copied time based data
   */
  virtual std::unique_ptr<TimeBasedData> clone() const = 0;

  /**
   * @brief Virtual function defining the operator to overload in the subclasses
   * @details virtual function
   *
   * @param[in] time_point Local time point defining the position of a value in the time based data
   * @return The value of the local time defined by the input variable
   */
  virtual double operator [](const SimulationClock::time_point&) const {return 0.0;} // dummy return

  /**
  * @brief Overloaded operator defining the selection of an element from the data
  * @details virtual function
  *
  * @param[in] clock Simulation clock defining the position of a value in the time based data
  * @return The value of the actual local time of the simulation clock defined by the input variable
  */
  double operator [](const SimulationClock& clock) const {
    return (operator [](clock.now()));
  }
  virtual SimulationClock::time_point start() const {
    return aux::SimulationClock::time_point_from_string("1900-01-01_00:00");
  }
  virtual double Mean(SimulationClock::time_point,
                      SimulationClock::time_point) const {return 0.0;} // dummy return

  /**
   * @brief Virtual function defining the conversion of time based data to string
   *
   * @return String containing the time based data
   */
  virtual std::string PrintToString() const {return "dummy return - this is pure virtual";}
  virtual bool empty() const {return true;}

 protected:
  /**
   * @brief Function determining the unknown value between two data points by linear interpolation
   *
   * @param[in] a Data defining the left boundary of the interpolation
   * @param[in] b Data defining the right boundary of the interpolation
   * @param[in] fraction Value expressing the ratio of the interpolation
   */
  inline double interpolate_linear(const double a,
                                   const double b,
                                   const double fraction) const noexcept {
      return ((1.0-fraction)*a + fraction*b);
  } // needs to be defined here since it is used by various subclasses
};

} /* namespace aux */

#endif /* AUXILIARIES_TIME_BASED_DATA_H_ */
