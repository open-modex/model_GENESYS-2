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
// program_settings.cc
//
// This file is part of the genesys-framework v.2

#include <program_settings.h>
#include <auxiliaries/functions.h>
#include <version.h>
#include <thread>
#include <chrono>

namespace genesys {

//default values
std::string ProgramSettings::program_mode_ = "evolution";
std::string ProgramSettings::input_files_folder_ = "Input";
std::string ProgramSettings::results_output_folder_ = "Results";
std::string ProgramSettings::filename_output_dynamic_model_ = "DynamicModelOutput.csv" ;
std::string ProgramSettings::optimisation_algorithm_ = "cma-es";
aux::SimulationClock::time_point
ProgramSettings::simulation_start_ = aux::SimulationClock::time_point_from_string("2015-01-01_00:00");
aux::SimulationClock::time_point
ProgramSettings::simulation_end_ = aux::SimulationClock::time_point_from_string("2050-01-01_00:00");
double ProgramSettings::approx_const_epsilon_ = 1e-9;
int ProgramSettings::cma_lambda_ = -1;
int ProgramSettings::cma_max_iter_ = 10000;
int ProgramSettings::cma_max_fevals_ = 50000;
double ProgramSettings::cma_init_sigma_  = 0.8;
double ProgramSettings::cma_ftarget_ = 1e4;
int ProgramSettings::cma_writeFullResult_after_ngenerations_ = 500;
int ProgramSettings::cma_weight_target_year_ = 3;
int ProgramSettings::cma_weight_target_duration_years_ = 1;
aux::SimulationClock::duration
ProgramSettings::installation_interval_ = aux::SimulationClock::duration_from_string("1a");
std::string ProgramSettings::operation_algorithm_ = "old_hierarchy_hsm";
aux::SimulationClock::duration
ProgramSettings::operation_sequence_duration_ = aux::SimulationClock::duration_from_string("1a");
int ProgramSettings::future_lookahead_time_ = 0;
double ProgramSettings::interest_rate_ = 0.06;
aux::SimulationClock::duration ProgramSettings::energy2power_ratio_ = aux::SimulationClock::duration_from_string("1h");
aux::SimulationClock::duration ProgramSettings::simulation_step_length_ = aux::SimulationClock::duration_from_string("1h");
//aux::SimulationClock::time_point ProgramSettings::result_analysis_start_ = aux::SimulationClock::time_point_from_string("2016-01-01_00:00");
int ProgramSettings::gridbalance_hop_level_ = 1;
bool ProgramSettings::consider_transmission_loss_ = false;
bool ProgramSettings::analysis_hsm_output_detail_ = false;
bool ProgramSettings::use_global_file_ = false;
bool ProgramSettings::use_randomisation_ = false;
bool ProgramSettings::deterministic_cmaes_ = false;
double ProgramSettings::max_co2_emission_annual_ = 0; /*mio t/a*/
double ProgramSettings::penalty_unsupplied_load_ = 1e7;
double ProgramSettings::penalty_self_supply_quota_  = 2e10;
double ProgramSettings::penalty_SQ_lower_limit_ = 0.7;
double ProgramSettings::penalty_SQ_upper_limit_ = 1.3;
double ProgramSettings::penalty_co2_exceed_emission_  = 0.;
double ProgramSettings::penalty_energy_cut_  = 0.;
double ProgramSettings::grid_exchange_ratio_= 0.3; //percentage of RL that can be drawn via grid
std::unordered_map<std::string, bool > ProgramSettings::genesys_modules_ = {};
std::unordered_map<std::string, double > ProgramSettings::penalties_ = {};

//std::unordered_map<std::string, bool > genesys_modules2_ = {std::make_pair<std::string, bool>("test", false)};

ProgramSettings::ProgramSettings(const std::string& filename) {
  //std::cout << "FUNC-ID: C'tor ProgramSettings\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<< "\n"<<std::endl;
  //initialisation of modules
  genesys_modules_["heat"]           =  false;
  genesys_modules_["dsm"]            =  false;    //demand side management
  genesys_modules_["ac_grid"]        =  false;    //not implemented
  genesys_modules_["dc_grid"]        =  false;
  genesys_modules_["ngas_grid"]      =  false;    //natural gas grid
  genesys_modules_["hgas_grid"]      =  false;    //hydrogen gas grid
  genesys_modules_["grid_expansion"] =  false;    //implementation iaew
  genesys_modules_["foresight"]      =  false;    //not implemented

  //initialisation of penalties
  penalties_["unsupplied_load"] = 0.;
  penalties_["unsupplied_heat"] = 0.;
  penalties_["self_supply_quota"] = 0.;
  penalties_["self_supply_quota_upper_limit"] = 1.;
  penalties_["self_supply_quota_lower_limit"] = 1.;

  std::ifstream filestream(filename.c_str(), std::ifstream::in);
  if (!filestream.is_open()) {
    std::cerr << "ERROR: ProgramSettings::ProgramSettings() - cannot read from file " << filename << std::endl;
    std::terminate();
  } else {
    std::string line;
    std::string setting_name;
    std::string setting_value;
    std::string data;
    // read the data
    while (getline(filestream, line)) {
      if (!line.empty()) {
        std::istringstream linestream(line);
        std::getline(linestream, data, '=');
        std::stringstream ss;
        ss << data;
        setting_name = ss.str();
        ss.str("");
        ss.clear();
        if (setting_name.substr(0, 2) != "/*") {
          getline(linestream, data);
          ss << data;
          setting_value = ss.str();
          ss.str("");
          ss.clear();
          InsertSetting(setting_name, setting_value);
        }
      }
    }
    filestream.close();
  }
  PrintAll();
}

//bool ProgramSettings::Get_HSM_Settings_boolean(std::string query) {
//  //DEBUG std::cout <<"FUNC-ID: ProgramSettings::Get_HSM_Settings_boolean() - Requested query string = " << query <<  std::endl;
//  bool return_value;
//  if (query.compare("consider_electrical_DC_network") == 0) {
//    return_value = consider_electrical_DC_network_;
//  } else if (query.compare("consider_electrical_AC_network") == 0) {
//    return_value = consider_electrical_AC_network_;
//  } else if (query.compare("consider_ngas_network") == 0) {
//    return_value = consider_ngas_network_;
//  } else if (query.compare("consider_hgas_network") == 0) {
//    return_value = consider_hgas_network_;
//  } else if (query.compare("consider_dsm") == 0) {
//    return_value = consider_dsm_;
//  } else if (query.compare("consider_transmission_loss") == 0) {
//    return_value = consider_transmission_loss_;
//  } else {
//    std::cerr << "ERROR: ProgramSettings::Get_HSM_Settings_boolean() : Queried string not allowed variable : "<< query <<  std::endl;
//    std::terminate();
//  }
//  return (return_value);
//}// END ProgramSettings::Get_Operation_Algorithm()

//double ProgramSettings::Get_HSM_Limits_Penalties(std::string query){
//  //DEBUG std::cout <<"FUNC-ID: ProgramSettings::Get_HSM_Limits_Penalties() - Requested query string = " << query <<  std::endl;
//  double return_value;
//  if (query.compare("max_co2_emission_annual") == 0) {
//    return_value = max_co2_emission_annual_;
//  } else if (query.compare("penalty_self_supply_quota") == 0) {
//    return_value = penalty_self_supply_quota_;
//  } else if (query.compare("penalty_co2_exceed_emission") == 0) {
//    return_value = penalty_co2_exceed_emission_;
//  } else if (query.compare("penalty_energy_cut") == 0) {
//    return_value = penalty_energy_cut_;
//  } else {
//    std::cerr << "ERROR: ProgramSettings::Get_HSM_Settings_boolean() : Queried string not allowed variable : "<< query <<  std::endl;
//    std::terminate();
//  }
//  return (return_value);
//}// ProgramSettings::Get_HSM_Limits_Penalties

void ProgramSettings::PrintAll() {
  //std::cout << "FUNC-ID: C'tor PrintAll\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<< "\n"<<std::endl;
  std::cout << "======ProgramSettings::PrintAll()===============" << "\n"
        << "Current setup: " << "\n"
        << "program_mode_ = " <<  program_mode_ << "\n"
        << "input_files_folder_ = " << input_files_folder_ << "\n"
        << "results_output_folder_ = " << results_output_folder_ << "\n"
        << "filename_output_dynamic_model_ = " << filename_output_dynamic_model_ << "\n"
        << "use_randomisation_ in optim and operation = "<< use_randomisation_ << "\n"
        << "----------global optimisation settings--------------" << "\n"
        << "simulation_start_ = " << aux::SimulationClock::time_point_to_string(simulation_start_) << "\n"
        << "simulation_end_ = " << aux::SimulationClock::time_point_to_string(simulation_end_) << "\n"
        << "optimisation_algorithm_ = " << optimisation_algorithm_ << "\n"
            << "\tcma_deterministic_seed =" << deterministic_cmaes_ << "\n"
            << "\tcma_lambda_ = " << cma_lambda_ << "\n"
            << "\tcma_max_iter_ = " << cma_max_iter_ << "\n"
            << "\tcma_max_fevals_ = " << cma_max_fevals_ << "\n"
            << "\tcma_init_sigma_ = " << cma_init_sigma_ << "\n"
			<< "\tcma_ftarget_ = " << cma_ftarget_ << "\n"
			<< "\tcma_writeFullResult_after_ngenerations_ = " << cma_writeFullResult_after_ngenerations_ << "\n"
            << "\tinstallation_interval_ = " << aux::SimulationClock::duration_to_string(installation_interval_) << "\n"
            << "\tcma_weight_target_year_ = " << cma_weight_target_year_ << "x\n"
        //<< "result_analysis_start_ = " << aux::SimulationClock::time_point_to_string(result_analysis_start_) << "\n"
		<< "use_global_file_ = " << use_global_file_ << "\n"

        << "----------operation algorithm settings--------------" << "\n"
        << "operation_algorithm_ = " << operation_algorithm_ << "\n"
        << "operation_sequence_duration_ = " << aux::SimulationClock::duration_to_string(operation_sequence_duration_) << "\n"
        << "future_lookahead_time_ = " << future_lookahead_time_ << "\n"
        << "interest_rate_ = " << interest_rate_ *100<< " %\n"
        << "energy2power_ratio_ = " << aux::SimulationClock::duration_to_string(energy2power_ratio_) << "\n"
        << "simulation_step_length_ = " << aux::SimulationClock::duration_to_string(simulation_step_length_) << "\n"
        << "gridbalance_hop_level_ = " << gridbalance_hop_level_ << " \n"
            << "\tgrid_exchange_ratio = " << grid_exchange_ratio_ << " * residual-load\n"
            << "\tconsider_transmission_loss_ = " << consider_transmission_loss_ << "\n"
            << "\tmax_co2_emission_annual_ = " << max_co2_emission_annual_ << "\n"

        << "----------operation algorithm penalties--------------" << "\n"
        << "penalty_unsupplied_load_ = " << penalty_unsupplied_load_ << "\n"
        << "penalty_self_supply_quota_ = " << penalty_self_supply_quota_ << "\n"
          << "\t lower limit self_supply_quota_ = " << penalty_SQ_lower_limit_ << "\n"
          << "\t upper limit self_supply_quota_ = " << penalty_SQ_upper_limit_ << "\n"
        //<< "penalty_co2_exceed_emission_ = " << penalty_co2_exceed_emission_ << "\n"
        //<< "penalty_energy_cut_ = " << penalty_energy_cut_ << "\n"
        << "----------analysis_hsm settings--------------" << "\n"
        << "analysis_hsm_output_detail_ = " << analysis_hsm_output_detail_ << "\n"
        << "----------PRINTING MODULES----------" << std::endl;
        aux::print_map_elements(genesys_modules_);
        std::cout << "======ProgramSettings::PrintAll()===============" << "\n"<< std::endl;
}

//Inserting the Values from DAT-File to the correct Setting-Name in STD::STRING format!
void ProgramSettings::InsertSetting (std::string setting_name, std::string setting_value){
  //std::cout << "FUNC-ID: C'tor InsertSetting\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<< "\n"<<std::endl;
  //DEBUG std::cout << "FUNC-ID: ProgramSettings::InsertSetting() working on name " << setting_name <<  " with value " << setting_value << std::endl;

  const size_t name_begin = setting_name.find_first_not_of(" ");
  const size_t name_end = setting_name.find_last_not_of(" ");
  const size_t name_range = name_end - name_begin + 1;

  const size_t value_begin = setting_value.find_first_not_of(" ");
  const size_t value_end = setting_value.find_last_not_of(" ");
  const size_t value_range = value_end - value_begin + 1;

  setting_name = setting_name.substr(name_begin, name_range);
  setting_value = setting_value.substr(value_begin, value_range);
    //begin general settings =========================================================================================================
  if (setting_name == "program_mode") {
    program_mode_ = setting_value;
  } else if (setting_name == "input_files_folder") {
    input_files_folder_ = setting_value;
  } else if (setting_name == "results_output_folder") {
    results_output_folder_ = setting_value;
  } else if (setting_name == "filename_output_dynamic_model") {
    filename_output_dynamic_model_ = setting_value;
  } else if (setting_name == "approx_const_epsilon") {
    approx_const_epsilon_ = std::stod(setting_value);
  } else if ( setting_name == "use_randomisation") {
    if (setting_value == "yes") {
      use_randomisation_ = true;
    } else if (setting_value == "no") {
      use_randomisation_ = false;
    } else {
      std::cerr << "ERROR in Input file, expected value for variable use_randomisation is yes/no, got " << setting_value << std::endl;
      std::terminate();
    }
  } else if ( setting_name == "cma_deterministic_seed") {
     if (setting_value == "yes") {
       deterministic_cmaes_ = true;
     } else if (setting_value == "no") {
       deterministic_cmaes_ = false;
     } else {
       std::cerr << "ERROR in Input file, expected value for variable use_randomisation is yes/no, got " << setting_value << std::endl;
       std::terminate();
     }
  //end general settings ===========================================================================================================
  //begin optimisation related settings ============================================================================================
  } else if (setting_name == "optimisation_algorithm") {
    optimisation_algorithm_ = setting_value;
  } else if (setting_name == "simulation_start") {
    simulation_start_ = aux::SimulationClock::time_point_from_string(setting_value);
  } else if (setting_name == "simulation_end") {
    simulation_end_ = aux::SimulationClock::time_point_from_string(setting_value);
  } else if (setting_name == "cma_lambda") {
    cma_lambda_ = std::stoi(setting_value);
  } else if (setting_name == "cma_max_iter") {
	  cma_max_iter_ = std::stoi(setting_value);
  } else if (setting_name == "cma_max_fevals") {
    cma_max_fevals_ = std::stoi(setting_value);
  } else if (setting_name == "cma_init_sigma") {
    cma_init_sigma_ = std::stod(setting_value);
  } else if (setting_name == "cma_ftarget") {
	  cma_ftarget_ = std::stod(setting_value);
  } else if (setting_name == "cma_writeResult_after_niter") {
	  cma_writeFullResult_after_ngenerations_ = std::stoi(setting_value);
  } else if (setting_name == "cma_weight_target_year") {
    cma_weight_target_year_ = std::stoi(setting_value);
  } else if (setting_name == "cma_weight_target_duration_years") {
	cma_weight_target_duration_years_ = std::stoi(setting_value);
  } else if (setting_name == "installation_interval") {
    installation_interval_ = aux::SimulationClock::duration_from_string(setting_value);
  //end optimisation related settings ==============================================================================================
  //begin operation related settings ===============================================================================================
	// deprecated cbu
    //  } else if (setting_name == "result_analysis_start") {
	//    result_analysis_start_ = aux::SimulationClock::time_point_from_string(setting_value);
  } else if (setting_name == "operation_algorithm") {
    operation_algorithm_ = setting_value;
  } else if (setting_name == "operation_sequence_duration") {
    operation_sequence_duration_ = aux::SimulationClock::duration_from_string(setting_value);
  } else if (setting_name == "interest_rate") {
    interest_rate_ = std::stod(setting_value);
  } else if (setting_name == "energy2power_ratio") {
    energy2power_ratio_ = aux::SimulationClock::duration_from_string(setting_value);
  } else if (setting_name == "simulation_step_length") {
    simulation_step_length_ = aux::SimulationClock::duration_from_string(setting_value);
  } else if (setting_name == "future_lookahead_time") {
    future_lookahead_time_ = std::stoi(setting_value);
  } else if (setting_name == "gridbalance_hop_level") {
    gridbalance_hop_level_ = std::stoi(setting_value);
  }  else if (setting_name == "consider_dsm") {
    if (setting_value == "yes") {
      genesys_modules_["dsm"] = true;
    } else if (setting_value == "no") {
      genesys_modules_["dsm"] = false;
    } else {
      std::cerr << "ERROR in Input file, expected value for variable consider_demand_side_management_ is yes/no, got " << setting_value << std::endl;
      std::terminate();
    }
  } else if (setting_name == "consider_electrical_DC_network") {
    if (setting_value == "yes") {
      genesys_modules_["dc_grid"] = true;
    } else if (setting_value == "no") {
      genesys_modules_["dc_grid"] = false;
    } else {
      std::cerr << "ERROR in Input file, expected value for variable consider_electrical_DC_network_ is yes/no, got " << setting_value << std::endl;
      std::terminate();
    }
  } else if (setting_name == "consider_electrical_AC_network") {
    if (setting_value == "yes") {
      genesys_modules_["ac_grid"] = true;
    } else if (setting_value == "no") {
      genesys_modules_["ac_grid"] = false;
    } else {
      std::cerr << "ERROR in Input file, expected value for variable consider_electrical_AC_network_ is yes/no, got " << setting_value << std::endl;
      std::terminate();
    }
  } else if (setting_name == "consider_ngas_network") {
    if (setting_value == "yes") {
      genesys_modules_["ngas_grid"] = true;
    } else if (setting_value == "no") {
      genesys_modules_["ngas_grid"] = false;
    } else {
      std::cerr << "ERROR in Input file, expected value for variable consider_ngas_network is yes/no, got " << setting_value << std::endl;
      std::terminate();
    }
  } else if (setting_name == "consider_hgas_network") {
    if (setting_value == "yes") {
      genesys_modules_["hgas_grid"] = true;
    } else if (setting_value == "no") {
      genesys_modules_["hgas_grid"] = false;
    } else {
         std::cerr << "ERROR in Input file, expected value for variable consider_hgas_network is yes/no, got " << setting_value << std::endl;
         std::terminate();
    }
  } else if (setting_name == "consider_transmission_loss") {
    if (setting_value == "yes") {
      consider_transmission_loss_ = true;
    } else if (setting_value == "no") {
      consider_transmission_loss_ = false;
    } else {
         std::cerr << "ERROR in Input file, expected value for variable consider_transmission_loss is yes/no, got " << setting_value << std::endl;
         std::terminate();
    }
  } else if (setting_name == "module_grid_expansion_costs") {
      if (setting_value == "yes") {
        genesys_modules_["grid_expansion"] = true;
      } else if (setting_value == "no") {
        genesys_modules_["grid_expansion"] = false;
      } else {
           std::cerr << "ERROR in Input file, expected value for variable module_grid_expansion_costs is yes/no, got " << setting_value << std::endl;
           std::terminate();
      }
  } else if (setting_name == "module_heat") {
      if (setting_value == "yes") {
        genesys_modules_["heat"] = true;
      } else if (setting_value == "no") {
        genesys_modules_["heat"] = false;
      } else {
           std::cerr << "ERROR in Input file, expected value for variable module_grid_expansion_costs is yes/no, got " << setting_value << std::endl;
           std::terminate();
      }
  } else if (setting_name == "analysis_hsm_output_detail") {
     if (setting_value == "yes") {
       analysis_hsm_output_detail_ = true;
     } else if (setting_value == "no") {
       analysis_hsm_output_detail_ = false;
     } else {
          std::cerr << "ERROR in Input file, expected value for variable 'analysis_hsm_output_detail' is yes/no, got " << setting_value << std::endl;
          std::terminate();
     }
  } else if (setting_name == "use_global_file") {
	  if(setting_value == "yes") {
		  use_global_file_ = true;
	  } else if (setting_value == "no") {
		  use_global_file_ = false;
	  } else {
		  std::cerr << "ERROR in Input file, expected value for variable 'use_global_file' is yes/no, got " << setting_value << std::endl;
		  std::terminate();
	  }
  //  } else if (setting_name == "max_co2_emission_annual") {
  //       max_co2_emission_annual_ = std::stod(setting_value);
  } else if (setting_name == "grid_exchange_ratio") { //percentage of RL that can be drawn via grid
    grid_exchange_ratio_ = std::stod(setting_value);
  ///penaltiy cost for operation simulation
  } else if (setting_name == "penalty_unsupplied_load") {
       double value = std::stod(setting_value);
       penalty_unsupplied_load_ = value;
       penalties_["unsupplied_load"] = value;
       if( value < 1e3) {
         std::cerr << "****WARNING: penalty_unsupplied_load value is " << value << ", consider increase if optimising!" << std::endl;
         std::this_thread::sleep_for(std::chrono::seconds(5));
       }
  } else if (setting_name == "penalty_unsupplied_heat") {
       double value = std::stod(setting_value);
       penalty_unsupplied_load_ = value;
       penalties_["unsupplied_heat"] = value;
       if( value < 1e3) {
         std::cerr << "****WARNING: penalty_unsupplied_heat value is " << value << ", consider increase if optimising!" << std::endl;
         std::this_thread::sleep_for(std::chrono::seconds(5));
       }
  } else if (setting_name == "penalty_self_supply_quota") {
       penalty_self_supply_quota_ = std::stod(setting_value);
       penalties_["self_supply_quota"] = std::stod(setting_value);
      } else if (setting_name == "penalty_SQ_upper_limit") {
          penalty_SQ_upper_limit_ = std::stod(setting_value);
          penalties_["self_supply_quota_upper_limit"] = std::stod(setting_value);
      } else if (setting_name == "penalty_SQ_lower_limit") {
          penalty_SQ_lower_limit_ = std::stod(setting_value);
          penalties_["self_supply_quota_lower_limit"] = std::stod(setting_value);
  //  } else if (setting_name == "penalty_co2_exceed_emission") {
  //       penalty_co2_exceed_emission_ = std::stod(setting_value);
  //  } else if (setting_name == "penalty_energy_cut") {
  //       penalty_energy_cut_ = std::stod(setting_value);
  //end operation related settings =================================================================================================
  } else {
    std::cerr << "ERROR: ProgramSettings::InsertSetting  - unknown setting identified: " << setting_name << " with value " << setting_value << std::endl;
    std::cerr <<  "Using GENESYS-2 version " << VERSION_STRING<< " compiled on " << __DATE__ << std::endl;
    std::terminate();
  }
}

} /* namespace genesys */
