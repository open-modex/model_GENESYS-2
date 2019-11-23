// ==================================================================
//
//  GENESYS2 is an optimisation tool and model of the European electricity supply system.
//
//  Copyright (C) 2015, 2016, 2017.  Robin Beer, Christian Bussar, Zhuang Cai, Kevin
//  Jacque, Luiz Moraes Jr., Philipp Stöcker
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public License
//  as published by the Free Software Foundation; either version 3 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
//  02110-1301 USA.
//
//  Project host: RWTH Aachen University, Aachen, Germany
//  Website: http://www.genesys.rwth-aachen.de
//
// ==================================================================
//
// simulation_clock.h
//
// This file is part of the genesys-framework v.2

#ifndef AUXILIARIES_SIMULATION_CLOCK_H_
#define AUXILIARIES_SIMULATION_CLOCK_H_

#include <array>
#include <chrono>
#include <ratio>
#include <string>
#include <typeinfo>

namespace aux {

typedef std::chrono::duration<int, std::ratio<8760 * 3600> >  years;
typedef std::chrono::duration<int, std::ratio<24 * 3600> >    days;
typedef std::chrono::hours                                    hours;
typedef std::chrono::minutes                                  minutes;

class SimulationClock {
 public:
  typedef minutes                                   duration;
  typedef duration::rep                             rep;
  typedef duration::period                          period;
  typedef std::chrono::time_point<SimulationClock>  time_point;

  /**
  * @brief Default Constructor (deleted)
  */
  SimulationClock() = delete;
  /**
  * @brief Constructor
  *
  * @param[in] tp The desired start date of the clock
  * @param[in] tick_length The desired tick length of the clock
  */
  template <class TickType = duration>
  SimulationClock(const time_point tp,
                  TickType tick_length)
      : SimulationClock(tp.time_since_epoch(), std::chrono::duration_cast<duration>(tick_length).count()) {}

  /**
  * @brief Required to comply with the chrono clock concept
  */
  static constexpr bool is_steady = false;

  /**
  * @brief Get current time of the clock
  *
  * @return time_point
  */
  time_point now() const {return time_point(current_simulation_time_);}

  /**
  * @brief Return the year of current time-point of clock
  *
  * @return int
  */
  int year() const {return (1970 + current_simulation_time_.count()/(8760*60));}

  static int year(time_point tp) {return 1970 + tp.time_since_epoch().count()/(8760*60);}


  /**
  * @brief Get tick length of the clock
  *
  * @return duration of the tick of the clock
  */
  duration tick_length() const {return tick_length_;}

  /**
  * @brief Gets the number of ticks per hour
  *
  * @return Number of ticks per hour
  */
  double ticks_per_hour() const {
    return static_cast<double>(std::ratio_divide<hours::period, period>::num)
          / (std::ratio_divide<hours::period, period>::den * tick_length_.count());
  }

  /**
  * @brief Gets the number of ticks per year
  *
  * @return Number of ticks per year
  */
  double ticks_per_year() const {
    return static_cast<double>(std::ratio_divide<years::period, period>::num)
          / (std::ratio_divide<years::period, period>::den * tick_length_.count());
  }

  /**
  * @brief Increments the simulation time by one tick
  *
  * @return time_point with time of clock after the tick was performed
  */
  time_point tick() {return time_point(current_simulation_time_ += tick_length_);}

  /**
  * @brief Increments the simulation time by the meta variable duration
  *
  * @param[in] leap_duration Duration of tick in current simulation
  */
  template <typename Rep2, typename Period2, typename = typename
      std::enable_if<std::chrono::treat_as_floating_point<rep>::value
             || (std::ratio_divide<Period2, period>::den == 1
                 && !std::chrono::treat_as_floating_point<Rep2>::value)>::type >
  time_point leap(const std::chrono::duration<Rep2, Period2>& leap_duration) {
    auto l_duration = std::chrono::duration_cast<duration>(leap_duration);
    return time_point(current_simulation_time_ += l_duration);
  }

  /**
  * @brief Converts string to time_point
  *
  * @param[in] date_string containing date information
  * (format: aaaa*m*d*h*min with * = wildcard except any numbers)
  * @return time_point constructed from the input string
  */
  static time_point time_point_from_string(const std::string& date_string);

  /**
  * @brief Converts time_point to string
  *
  * @param[in] time_point to be converted
  * @return string containing the converted date
  */
  static std::string time_point_to_string(time_point time_point);

  /**
  * @brief Converts string to duration
  *
  * @param[in] duration_string containing duration information
  * (format: number + specifier with specifier = {a,d,h,min,s})
  * @return duration constructed from the input string
  */
  static duration duration_from_string(const std::string& duration_string);

  /**
  * @brief Converts duration to string
  *
  * @param[in] duration to be converted
  * @return string containing the converted time interval
  */
  template <typename Duration>
  static std::string duration_to_string(Duration dur) {
    std::string duration_unit;
    if (typeid(dur).hash_code() == typeid(years).hash_code()) {
      duration_unit = "y";
    } else if (typeid(dur).hash_code() == typeid(days).hash_code()) {
      duration_unit = "d";
    } else if (typeid(dur).hash_code() == typeid(hours).hash_code()) {
      duration_unit = "h";
    } else if (typeid(dur).hash_code() == typeid(minutes).hash_code()) {
      duration_unit = "min";
    }
    return (std::to_string(dur.count()) + duration_unit);
  }

  /**
  * @brief Converts energy to power
  *
  * @param[in] energy
  * @param[in] e2p_duration
  * @return Mean power resulting from distributing the amount of energy over the duration
  */
  static double e2p(const double energy,
                    duration e2p_duration);

  /**
  * @brief Converts power to energy
  *
  * @param[in] power
  * @param[in] p2e_duration
  * @return energy resulting from integrating the power over the duration
  */
  static double p2e(double power,
                    duration p2e_duration);

 private:
  SimulationClock(const duration duration_since_epoch,
                  rep tick_length);

  static const std::array<int, 5> epoch_;

  duration tick_length_;
  duration current_simulation_time_;
};

} /* namespace aux */

#endif /* AUXILIARIES_SIMULATION_CLOCK_H_ */

