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
// program_specification.h
//
// This file is part of the genesys-framework v.2

#ifndef PROGRAM_SETTINGS_H_
#define PROGRAM_SETTINGS_H_

#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

#include <auxiliaries/simulation_clock.h>

namespace genesys {

class ProgramSettings {
 public:
  ProgramSettings() = delete;
	ProgramSettings(const std::string& filename);
	~ProgramSettings() = default;
  //bool Get_HSM_Settings_boolean(std::string query);
  //double Get_HSM_Limits_Penalties(std::string qquery);
  static std::string filename_output_dynamic_model() { return filename_output_dynamic_model_;};
  static aux::SimulationClock::time_point simulation_start() {return simulation_start_;}
  static aux::SimulationClock::time_point simulation_end() {return simulation_end_;}
  //static aux::SimulationClock::time_point result_analysis_start() {return result_analysis_start_;}
  static double approx_epsilon() {return approx_const_epsilon_;}
  //const std::unordered_map<std::string, bool>& modules() {return genesys_modules_;}
  static const std::unordered_map<std::string, bool>& modules() {return genesys_modules_;}
  static const std::unordered_map<std::string, double>& penalties() {return penalties_;}



  /** \name Interface to control CMA-ES.*/
  ///@{
  static int cma_lambda() {return cma_lambda_;}
  static int cma_max_iter() {return cma_max_iter_;}
  static int cma_max_fevals() {return cma_max_fevals_;}
  static double cma_init_sigma() {return cma_init_sigma_;}
  static double cma_ftarget() {return cma_ftarget_;}
  static int cma_niter_write_full_result() {return cma_writeFullResult_after_ngenerations_;}
  static int get_cma_weight_target_year() {return cma_weight_target_year_;}
  static int get_cma_weight_target_duration_years() {return cma_weight_target_duration_years_;}
  static bool use_deterministic_cmaes() {return deterministic_cmaes_;}
  ///@}

  /** \name Control variables for operation strategy*/
  ///@{
  static aux::SimulationClock::duration installation_interval() {return installation_interval_;}
  static std::string get_operation_algorithm() {return operation_algorithm_;}
  static aux::SimulationClock::duration get_operation_sequence_duration() {return operation_sequence_duration_;}
  static int operation_get_LookAheadTime() {return future_lookahead_time_;}
  static double interest_rate() {return interest_rate_;}
  static aux::SimulationClock::duration energy2power_ratio() {return energy2power_ratio_;}
  static aux::SimulationClock::duration simulation_step_length() {return simulation_step_length_;}
  //static double penalty_unsupplied_load() {return penalty_unsupplied_load_;}
  //static double penalty_self_supply_quota() {return penalty_self_supply_quota_;}
  //static double SQ_lower_limit_() {return penalty_SQ_lower_limit_;}
  //static double SQ_upper_limit_() {return penalty_SQ_upper_limit_;}
  static int gridbalance_hop_level() { return gridbalance_hop_level_;}
  static bool analysis_hsm_output_detail() { return analysis_hsm_output_detail_;}
  static bool use_global_file() { return use_global_file_;}
  static bool use_randomisation() { return use_randomisation_ ; }
  static double grid_exchange_ratio() {return grid_exchange_ratio_;} //percentage of RL that can be drawn via grid
  ///@}

  void PrintAll();

 private:
	void InsertSetting(std::string setting_name, std::string setting_value);

  static std::string program_mode_; // optimisation, evolution, operation, target_year, resume
  static std::string input_files_folder_;
  static std::string results_output_folder_;
  static std::string filename_output_dynamic_model_;
  static std::string optimisation_algorithm_;
  static aux::SimulationClock::time_point simulation_start_;
  static aux::SimulationClock::time_point simulation_end_;
  //settings to control cma-es algorithm
  static int cma_lambda_;
  static int cma_max_iter_;
  static int cma_max_fevals_;
  static double cma_init_sigma_;
  static double cma_ftarget_;
  static int cma_writeFullResult_after_ngenerations_;
  static aux::SimulationClock::duration installation_interval_;
  static int cma_weight_target_year_;
  static int cma_weight_target_duration_years_;
  //settings relevant for operation simulation
  //deprecated cbu static aux::SimulationClock::time_point result_analysis_start_;
  static std::string operation_algorithm_;
  static aux::SimulationClock::duration operation_sequence_duration_;
  static int future_lookahead_time_;
  static double interest_rate_;
  static aux::SimulationClock::duration energy2power_ratio_;
  static aux::SimulationClock::duration simulation_step_length_;
  static int gridbalance_hop_level_;
  static double approx_const_epsilon_;

  static std::unordered_map<std::string, bool > genesys_modules_;
  //static std::unordered_map<std::string, bool > genesys_modules2_;
  //  static bool consider_electrical_DC_network_;
  //  static bool consider_electrical_AC_network_;
  //  static bool consider_ngas_network_;//natural gas
  //  static bool consider_hgas_network_;//hydrogen gas
  //  static bool consider_dsm_;//demand side management
  static bool consider_transmission_loss_;

  static bool analysis_hsm_output_detail_;
  static bool use_global_file_;
  static bool use_randomisation_;
  static bool deterministic_cmaes_;

  static double grid_exchange_ratio_; //percentage of RL that can be drawn via grid
  static double max_co2_emission_annual_; /*mio t/a*/
  static double penalty_unsupplied_load_ ;
  static double penalty_self_supply_quota_ ;
  static double penalty_SQ_lower_limit_;
  static double penalty_SQ_upper_limit_;
  static double penalty_co2_exceed_emission_ ;
  static double penalty_energy_cut_ ;
  static std::unordered_map<std::string, double> penalties_;

};

} /* namespace genesys */

#endif /* PROGRAM_SETTINGS_H_ */
