/*
 * function.cc
 *
 *  Created on: Jul 21, 2016
 *      Author: pst
 */

#include <auxiliaries/functions.h>
#include <program_settings.h>

#include <cmath>
#include <exception>
#include <iostream>
#include <iomanip> //std::setw
#include <sstream>

namespace aux{

double DiscountFuture2(const aux::TimeBasedData& tbd,
                      aux::SimulationClock::time_point start_discount_horizon,
                      aux::SimulationClock::time_point end_discount_horizon,
                      aux::SimulationClock::duration period,
                      double interest_rate,
                      aux::SimulationClock::time_point present_day) {
  //   std::cout << "FUNC-ID: DiscountFuture2\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  double discounted_sum = 0.0;
  int initial_steps = (start_discount_horizon - present_day) / years(1);

  if (interest_rate != -1.0) {
    double base = 1.0 + interest_rate;
    auto internal_present = start_discount_horizon;
    for (int step = initial_steps; internal_present < end_discount_horizon; ++step) {
      //      std::cout << "\tstart horizon = " << aux::SimulationClock::time_point_to_string(internal_present) << " | step= " << step << std::endl;
      discounted_sum += tbd[internal_present] * std::pow(base, -step);
      internal_present += period;
    }
  } else {
    std::cerr << "ERROR in aux::Function::DiscountFuture DIV by Zero - interest rate i must be != -1.0" << std::endl;
    std::terminate();
  }
  return discounted_sum;
}

double timed_value_annual(double value,
                   aux::SimulationClock::time_point present_day,
                   aux::SimulationClock::time_point other_day,
                   double interest_rate) {
  //   std::cout << "FUNC-ID: timed_value_annual\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  double time_value = 0.;
  if (interest_rate != -1.0) {
    int t_delta_years = ((other_day - present_day ) / years(1))  -1; //positive for future and negative for past time_points // -1 because I_0 at 31.12.yearbeforeFirstANF
    time_value = value*std::pow(1+interest_rate, t_delta_years );
    if (time_value > genesys::ProgramSettings::approx_epsilon() || time_value < -genesys::ProgramSettings::approx_epsilon()){
      return time_value;
    }
  } else {
    std::cerr << "ERROR in aux::Function::timed_value DIV by Zero- interest rate i must be != -1.0, was:" << interest_rate << std::endl;
    std::terminate();
  }
  return time_value;//returns 0.

}

double anf(const double interest_rate, const int T_lifetime) {
  //   std::cout << "FUNC-ID: anf\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  double anf = 0.;
  if (T_lifetime == 0 ) {
	  std::cout << "interest_rate:" << interest_rate << "\n"
			    << "Lifetime T:" << T_lifetime << std::endl;
	  std::cerr << "Error in aux::anf DIV by Zero" << std::endl;
	  std::terminate();
  } else if(interest_rate == 0) {
	  anf =  1./T_lifetime;
  } else {
	double q = 1 + interest_rate;
    double divisor = std::pow(q,T_lifetime)-1;
    if (divisor > genesys::ProgramSettings::approx_epsilon() || divisor < -genesys::ProgramSettings::approx_epsilon()) {//never divByZero
      anf = (std::pow(q, T_lifetime)*(interest_rate) ) / divisor;
    } else {
      std::cout << "interest_rate:" << interest_rate << "\n"
                << "Lifetime T:" << T_lifetime << "\n"
                << "Divisor   :" << divisor<< std::endl;
      std::cerr << "Error in aux::anf: DIV by Zero" << std::endl;
      std::terminate();
    }
    return anf;
  }
  return anf;
}

double sum_means(const aux::TimeBasedData& tbd,
                 aux::SimulationClock::time_point start,
                 aux::SimulationClock::time_point end,
                 aux::SimulationClock::duration interval){
  //   std::cout << "FUNC-ID: sum_means\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  double sum =0.;
  aux::SimulationClock clc(start, interval);
  do {
    double val= tbd.Mean(clc.now(), clc.now()+interval);
    if (val >= genesys::ProgramSettings::approx_epsilon()) {
      sum += val;
    } else {
      //std::cout << "value was small: " << val << std::endl;
    }
  } while (clc.tick() < end);
  return sum;

}

double sum_positiveValues(const aux::TimeBasedData& tbd,
           aux::SimulationClock::time_point start,
           aux::SimulationClock::time_point end,
           aux::SimulationClock::duration interval) {
  //   std::cout << "FUNC-ID: sum_positiveValues\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  double sum =0.;
    aux::SimulationClock clc(start, interval);
    do {
      try {
        double tmp_val = tbd[clc.now()];
        if(tmp_val > genesys::ProgramSettings::approx_epsilon()) {
          sum += tmp_val;
        }
      } catch (const std::exception& e) {
        std::cout << "ERROR in aux::sum_positiveValues() evaluation of empty object?" << std::endl;
        //std::cout << e.what();
      }
    } while (clc.tick() < end);
    return sum;
}

double sum(const aux::TimeBasedData& tbd,
           aux::SimulationClock::time_point start,
           aux::SimulationClock::time_point end,
           aux::SimulationClock::duration interval){
  //   std::cout << "FUNC-ID: sum\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  double sum =0.;
  aux::SimulationClock clc(start, interval);
  do {
    try {
      sum += tbd[clc.now()];
    } catch (const std::exception& e) {
      std::cout << "ERROR in aux::sum() evaluation of empty object?" << std::endl;
      //std::cout << e.what();
    }
  } while (clc.tick() < end);
  return sum;
}

bool cmp_equal(double a,
               double b,
               double precision) {
  //   std::cout << "FUNC-ID: cmp_equal\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  if ((a - b) * (a - b) < precision * precision) {
    return (true);
  }
  return (false);
}


std::string pretty_time_string(int time_msec){
  //   std::cout << "FUNC-ID: pretty_time_string\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  std::stringstream tmp_stream;
  int optimization_time_printed = 0;
  if (time_msec > 1000) {
    auto optimisation_time_sec = time_msec / 1000;
    if (optimisation_time_sec > 60) {
      auto optimisation_time_min = optimisation_time_sec / 60;
      if (optimisation_time_min > 60) {
        auto optimisation_time_h = optimisation_time_min / 60;
        if (optimisation_time_h > 24) {
          auto optimisation_time_d = optimisation_time_h / 24;
          tmp_stream << optimisation_time_d << " days, ";
          optimisation_time_h -= (optimization_time_printed += optimisation_time_d * 24);
        }
        tmp_stream << optimisation_time_h << " hours, ";
        optimization_time_printed += optimisation_time_h;
        optimization_time_printed *= 60;
        optimisation_time_min -= optimization_time_printed;
      }
      tmp_stream << optimisation_time_min << " minutes, ";
      optimization_time_printed += optimisation_time_min;
      optimization_time_printed *= 60;
      optimisation_time_sec -= optimization_time_printed;
    }
    tmp_stream << optimisation_time_sec << " seconds and ";
    optimization_time_printed += optimisation_time_sec;
    optimization_time_printed *= 1000;
    time_msec -= optimization_time_printed;
  }
  tmp_stream << time_msec << " milliseconds";
  return tmp_stream.str();
}

void print_map_elements(const std::unordered_map<std::string, bool>& in_map){
  //  std::cout << "FUNC-ID: print_map_elements\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<< "\n"<<std::endl;
  std::cout << in_map.size() << " Elements in map" << std::endl;
  if(!in_map.empty())
    for(auto &it : in_map){
      std::cout << "KEY\t" << std::setw(25)<< it.first << "\tBOOL\t"<< it.second << std::endl;
    }
  else
    std::cout << "--MAP EMPTY--" << std::endl;
}

}//end namespace aux
