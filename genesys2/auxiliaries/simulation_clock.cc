/*
 * simulation_clock.cpp
 *
 *  Created on: Aug 21, 2015
 *      Author: pst
 */

#include <auxiliaries/simulation_clock.h>

#include <exception>
#include <iostream>

#include <program_settings.h>

namespace aux {

SimulationClock::time_point SimulationClock::time_point_from_string(const std::string& date_string) {
  std::string string_in = date_string;
  std::array<int, 5> date = epoch_;
  auto numbers = std::string("1234567890");
  std::string::size_type pos = string_in.find_first_of(numbers);
  if (pos == std::string::npos) {
    std::cerr << "Error in aux::SimulationClock::time_point_from_string :" << std::endl <<
        "No numeric characters in input string '" << date_string << "'" << std::endl;
    std::terminate();
  }
  for (auto&& i : date) {
    string_in.erase(0, pos); // erase leading non-numeric characters from date_string
    i = std::stoi(string_in, &pos);
    string_in.erase(0, pos); // erase leading numeric characters processed in previous line
    pos = string_in.find_first_of(numbers);
    if (pos == std::string::npos)
      break; // date_string contains no more numeric characters
  }
  if (1 <= date[1] && date[1] <= 12) { // check range of month field
    std::array<char, 12> days_of_month = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    // check range of remaining fields
    if (1 <= date[2] && date[2] <= days_of_month[date[1] - 1] && 0 <= date[3] && date[3] <= 23
        && 0 <= date[4] && date[4] <= 59) {
      auto monthdelta = date[1] - epoch_[1];
      auto hour_of_year = ((((153 * (monthdelta + (monthdelta > 1 ? -2 : 10)) + 2) / 5) + 59) % 365
          + date[2] - epoch_[2]) * 24 + date[3] - epoch_[3];
      return (time_point(years(date[0] - epoch_[0]) + hours(hour_of_year) + minutes(date[4] - epoch_[4])));
    }
  }
  std::cerr << "Error in aux::SimulationClock::time_point_from_string :" << std::endl <<
      "Given date in input string '" << date_string << "' is out of range for one or more fields" << std::endl;
  std::terminate();
} // credits to Howard Hinnant for the idea to this brilliant algorithm

std::string SimulationClock::time_point_to_string(time_point time_point) {
  std::array<int, 5> date;
  auto time_from_epoch = time_point.time_since_epoch();
  auto year_from_epoch = std::chrono::duration_cast<years>(time_from_epoch);
  auto hour_from_epoch = std::chrono::duration_cast<hours>(time_from_epoch);
  date[0] = year_from_epoch.count() + epoch_[0];
  auto hour_of_year_int = static_cast<int>((hour_from_epoch - year_from_epoch).count());
  auto day_of_year = hour_of_year_int / 24;
  auto shifted_day_of_year = day_of_year + (day_of_year > 59 ? -59 : 306);
  date[3] = hour_of_year_int % 24 + epoch_[3];
  auto temp = (5 * shifted_day_of_year + 2) / 153;
  date[2] = shifted_day_of_year - (153 * temp + 2) / 5 + epoch_[2];
  date[1] = temp + (temp < 10 ? 2 : -10) + epoch_[1];
  date[4] = (time_from_epoch - hour_from_epoch).count() + epoch_[4];
  return std::to_string(date[0]) + "-" + std::to_string(date[1]) + "-" + std::to_string(date[2]) + "_"
      + std::to_string(date[3]) + ":" + std::to_string(date[4]);
} // credits to Howard Hinnant for the idea to this brilliant algorithm

SimulationClock::duration SimulationClock::duration_from_string(const std::string& duration_string) {
  if (duration_string.find("a") != std::string::npos) {
    return years(std::stoi(duration_string));
  } else if (duration_string.find("d") != std::string::npos) {
    return days(std::stoi(duration_string));
  } else if (duration_string.find("h") != std::string::npos) {
    return hours(std::stoi(duration_string));
  } else if (duration_string.find("min") != std::string::npos) {
    return minutes(std::stoi(duration_string));
  } else {
    std::cerr << "ERROR in aux::SimulationClock::duration_from_string :" << std::endl
        << "unknown interval specifier in '" << duration_string << "'" << std::endl;
    std::terminate();
  }
  return duration(); // dummy return
}

double SimulationClock::e2p(double energy,
                            duration e2p_duration) {
  return (energy * (std::chrono::duration_cast<std::chrono::duration<double, period> >(
      genesys::ProgramSettings::energy2power_ratio())
      / std::chrono::duration_cast<std::chrono::duration<double, period> >(e2p_duration)));
}

double SimulationClock::p2e(double power,
                            duration p2e_duration) {
  return (power * (std::chrono::duration_cast<std::chrono::duration<double, period> >(p2e_duration)
      / std::chrono::duration_cast<std::chrono::duration<double, period> >(
          genesys::ProgramSettings::energy2power_ratio())));
}

SimulationClock::SimulationClock(const duration duration_since_epoch,
                                 rep tick_length)
    : tick_length_(tick_length),
      current_simulation_time_(duration_since_epoch) {
  if (tick_length_ == duration(0)) {
      std::cerr << "Error in aux::SimulationClock::SimulationClock :" << std::endl
          << "Tick length must be non-zero." << std::endl;
      std::terminate();
  }
}

const std::array<int, 5> SimulationClock::epoch_ = {1970, 1, 1, 0, 0};

} /* namespace aux */
