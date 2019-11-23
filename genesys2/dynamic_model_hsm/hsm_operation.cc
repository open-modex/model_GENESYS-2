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
// hsm_operation.cc
//
// This file is part of the genesys-framework v.2

#include <dynamic_model_hsm/hsm_operation.h>

#include <chrono>
#include <ratio>
#include <string>
#include <iomanip>
#include <unordered_map>

#include <program_settings.h>

namespace dm_hsm {

HSMOperation::HSMOperation(const sm::StaticModel& model)
    : model_(model),
      tp_start_operation_(genesys::ProgramSettings::simulation_start()),
      tp_end_operation_(genesys::ProgramSettings::simulation_end()),
      duration_operation_sequence_(genesys::ProgramSettings::get_operation_sequence_duration()),
      num_operation_sequence_iterations_((tp_end_operation_- tp_start_operation_) / duration_operation_sequence_),
      duration_last_operation_sequence_(tp_end_operation_- tp_start_operation_ -
                                        num_operation_sequence_iterations_ * duration_operation_sequence_),
      future_lookahead_time_(genesys::ProgramSettings::operation_get_LookAheadTime()),
      accumulated_penalties_unsupplied_load_(0.),
      accumulated_penalties_selfsupply_quota_(0.),
      fitness_(0.0) {
}

std::unordered_map<std::string, double > HSMOperation::CalculateFitnessMinCost(bool analyse){//, aux::SimulationClock::duration max_operation_duration){
  //  std::cout << "FUNC-ID: HSMOperation::CalculateFitnessMinCost()" << std::endl;
  auto wall_time_timer = std::chrono::system_clock::now();
  startSequencer();
  std::unordered_map<std::string, double >result_map;
  if (analyse) {
    //calculate annual values for for output
    model_.calculate_annual_electricity_prices(tp_start_operation_, tp_end_operation_);
    model_.calculate_annual_disc_capex(tp_start_operation_, tp_end_operation_);
  }
  double capex = model_.getSumValue("CAPEX", tp_start_operation_, tp_end_operation_);
  double fopex = model_.getSumValue("FOPEX", tp_start_operation_, tp_end_operation_);
  double vopex = model_.getSumValue("VOPEX", tp_start_operation_, tp_end_operation_);
  double real_cost = capex + vopex + fopex;

  //Application of PENALTIES===========================================================================================
  //  auto sum_usload =   model_.getSumValue("unsupplied_load",  tp_start_operation_, tp_end_operation_);
  auto sum_energy =  model_.getSumValue("ENERGY", tp_start_operation_, tp_end_operation_);
  //  auto penalties = calculate_penalties(total_cost, sum_usload,sum_energy);//return std::make_tuple(total_cost,sum_usload, penalties_SQ);
  //  total_cost += std::get<0>(penalties);
  //  sum_usload += std::get<1>(penalties);
  //  double penalties_SQ =  std::get<2>(penalties);


  auto penalties = calculate_penalties(real_cost, sum_energy);//return std::make_tuple(total_cost,sum_usload, penalties_SQ);
  auto pen0 = std::get<0>(penalties); //unsupplied load pen/energy
  auto pen1 = std::get<1>(penalties); //self supply pen/energy
  //  auto pen2 = std::get<2>(penalties2); //tbd

  //debug std::cout  << "Unsupplied:\t" << pen0.first << " EUR | \t" << pen0.second << " GWh" << std::endl;

  // END PENALTIES=====================================================================================================

  //CALCULATE LCOE=====================================================================================================
  double LCOE = 0.0;
  //calculate discounted values for lcoe calculation
  auto disc_energy = model_.getDiscountedValue("ENERGY", tp_start_operation_, tp_end_operation_);
  auto disc_capex  = model_.getDiscountedValue("CAPEX", tp_start_operation_, tp_end_operation_);
  auto disc_fopex  = model_.getDiscountedValue("FOPEX", tp_start_operation_, tp_end_operation_);
  auto disc_vopex  = model_.getDiscountedValue("VOPEX", tp_start_operation_, tp_end_operation_);


  double all_realCost = disc_capex + disc_vopex + disc_fopex;
    if ( disc_energy != 0) {
      LCOE =  all_realCost  / disc_energy;
      //fitness_ = total_cost / energy;
    } else {
      std::cerr << "ERROR in HSMOperation::CalculateFitness(): calculated Energy resulted in DIVbyZERO Error!" << std::endl;
    }

  //FITNESS DEFINITION: all real_cost + penalties======================================================================
  //fitness_ = real_cost + accumulated_penalties_unsupplied_load_ + penalties_SQ;
  fitness_ = real_cost + pen0.first + pen1.first;// + penalties_SQ;

  //GENERATE OUTPUT====================================================================================================
  #pragma omp critical
  {
      std::cout <<  "capx="    <<std::setw(11)<< capex
                << " fopx="    <<std::setw(11)<< fopex
                << " vopx="    <<std::setw(11)<< vopex
                << " cost="<<std::setw(11)<< real_cost
                << " penUL[%cost]=" <<std::setw(11)<< pen0.first/real_cost  //accumulated_penalties_unsupplied_load_
                << " engyUL[%dem]=" <<std::setw(11)<<  pen0.second/sum_energy// sum_usload
                << " penSQ[%cost]=" <<std::setw(11)<< pen1.first/real_cost       //penalties_SQ
                << " dem="   <<std::setw(11)<< disc_energy
                << " LCOE[ct/kWh]="    <<std::setw(11)<< LCOE/1e4
                << " fitness=" << fitness_ << std::endl;
  }
  //create return values-map
  result_map.emplace("fitness", fitness_);
  //results.push_back(fitness_);
  if (analyse){
  result_map.emplace("energy", disc_energy);
  result_map.emplace("capex", capex);
  result_map.emplace("fopex", fopex);
  result_map.emplace("vopex", vopex);
  result_map.emplace("pen_unsupplied_load", pen0.first );//accumulated_penalties_unsupplied_load_);
  result_map.emplace("pen_self_supply", pen1.first);//penalties_SQ);
  result_map.emplace("lcoe_ct/kWh", LCOE/1e4);
  std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wall_time_timer);
  std::cout << "Analysis finished in " << wctduration.count() << " seconds [Wall Clock]" << std::endl;
  result_map.emplace("analysis_timer_sec", wctduration.count());
  }
  return (result_map); //fitness_);

}


std::unordered_map<std::string, double > HSMOperation::CalculateFitnessMinLCOE(bool analyse){//, aux::SimulationClock::duration max_operation_duration){
  //  std::cout << "FUNC-ID: HSMOperation::CalculateFitnessMinCost()" << std::endl;
  auto wall_time_timer = std::chrono::system_clock::now();
  startSequencer();
  std::unordered_map<std::string, double >result_map;
  if (analyse) {
    //calculate annual values for for output
    model_.calculate_annual_electricity_prices(tp_start_operation_, tp_end_operation_);
    model_.calculate_annual_disc_capex(tp_start_operation_, tp_end_operation_);
  }
  double capex = model_.getSumValue("CAPEX", tp_start_operation_, tp_end_operation_);
  double vopex = model_.getSumValue("FOPEX", tp_start_operation_, tp_end_operation_);
  double fopex = model_.getSumValue("VOPEX", tp_start_operation_, tp_end_operation_);
  double real_cost = capex + vopex + fopex;

  //Application of PENALTIES===========================================================================================
  auto sum_energy =  model_.getSumValue("ENERGY", tp_start_operation_, tp_end_operation_);

  auto penalties = calculate_penalties(real_cost, sum_energy);//return std::make_tuple(total_cost,sum_usload, penalties_SQ);
  auto pen0 = std::get<0>(penalties); //unsupplied load pen/energy
  auto pen1 = std::get<1>(penalties); //self supply pen/energy
  //  auto pen2 = std::get<2>(penalties2); //tbd
  // END PENALTIES=====================================================================================================

  //CALCULATE LCOE=====================================================================================================
  double LCOE = 0.0;
  //calculate discounted values for lcoe calculation
  auto disc_energy = model_.getDiscountedValue("ENERGY", tp_start_operation_, tp_end_operation_);
  auto disc_capex  = model_.getDiscountedValue("CAPEX", tp_start_operation_, tp_end_operation_);
  auto disc_fopex  = model_.getDiscountedValue("FOPEX", tp_start_operation_, tp_end_operation_);
  auto disc_vopex  = model_.getDiscountedValue("VOPEX", tp_start_operation_, tp_end_operation_);


  double all_realCost = disc_capex + disc_vopex + disc_fopex;
    if ( disc_energy != 0) {
      LCOE =  all_realCost  / disc_energy;
      //fitness_ = total_cost / energy;
    } else {
      std::cerr << "ERROR in HSMOperation::CalculateFitness(): calculated Energy resulted in DIVbyZERO Error!" << std::endl;
    }

  //FITNESS DEFINITION: all real_cost + penalties divided by discounted energy = LCOE==================================
  fitness_ = (real_cost + pen0.first + pen1.first)/disc_energy;

  //GENERATE OUTPUT====================================================================================================
  #pragma omp critical
  {
      std::cout <<  "capx="    <<std::setw(11)<< capex
                << " fopx="    <<std::setw(11)<< fopex
                << " vopx="    <<std::setw(11)<< vopex
                << " cost="<<std::setw(11)<< real_cost
                << " penUL[%cost]=" <<std::setw(11)<< pen0.first/real_cost  //accumulated_penalties_unsupplied_load_
                << " engyUL[%dem]=" <<std::setw(11)<<  pen0.second/sum_energy// sum_usload
                << " penSQ[%cost]=" <<std::setw(11)<< pen1.first/real_cost       //penalties_SQ
                << " dem="   <<std::setw(11)<< disc_energy
                << " LCOE[ct/kWh]="    <<std::setw(11)<< LCOE/1e4
                << " fitness=" << fitness_ << std::endl;
  }
  //create return values-map
  result_map.emplace("fitness", fitness_);
  //results.push_back(fitness_);
  if (analyse){
  result_map.emplace("energy", disc_energy);
  result_map.emplace("capex", capex);
  result_map.emplace("fopex", fopex);
  result_map.emplace("vopex", vopex);
  result_map.emplace("pen_unsupplied_load", pen0.first );//accumulated_penalties_unsupplied_load_);
  result_map.emplace("pen_self_supply", pen1.first);//penalties_SQ);
  result_map.emplace("lcoe_ct/kWh", LCOE/1e4);
  std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wall_time_timer);
  std::cout << "Analysis finished in " << wctduration.count() << " seconds [Wall Clock]" << std::endl;
  result_map.emplace("analysis_timer_sec", wctduration.count());
  }
  return (result_map); //fitness_);

}

//DEPRECATED-cbu
//std::unordered_map<std::string, double > HSMOperation::CalculateFitness(bool analyse){//, aux::SimulationClock::duration max_operation_duration){
//  //  std::cout << "FUNC-ID: HSMOperation::CalculateFitness()" << std::endl;
//  auto wall_time_timer = std::chrono::system_clock::now();
//  //solve system for each hour independently
//  startSequencer();
//  //std::vector<double> results;
//  std::unordered_map<std::string, double >result_map;
//
//  //calculate fitness from time -step-simulation values
//    //discount annual values to present day = start simulation
//    auto energy = model_.getDiscountedValue("ENERGY", tp_start_operation_, tp_end_operation_);
//    auto capex  = model_.getDiscountedValue("CAPEX", tp_start_operation_, tp_end_operation_);
//    auto fopex  = model_.getDiscountedValue("FOPEX", tp_start_operation_, tp_end_operation_);
//    auto vopex  = model_.getDiscountedValue("VOPEX", tp_start_operation_, tp_end_operation_);
//
//    //WEIGHTING MECHANISMS FOR LAST OPTIMISATION YEAR==================================================================
//    //WEIGHTING of the target year
//      double weight_target = genesys::ProgramSettings::get_cma_weight_target_year();
//
//    //Cost of the target year
//      auto start_target_year = aux::SimulationClock::year(tp_end_operation_)- genesys::ProgramSettings::get_cma_weight_target_duration_years();
//      if (start_target_year < aux::SimulationClock::year(tp_start_operation_)) {//catch if start target year is valid!
//    	  std::cerr << "Error in HSMOperation::CalculateFitness() year calculated for weight target year was before start" << std::endl;
//    	  std::cerr << "\t Start year = " << aux::SimulationClock::time_point_to_string(tp_start_operation_) << std::endl;
//
//    	  std::cerr << "\t Start weighting year = " << start_target_year<< "-01-01_00:00" << std::endl;
//    	  std::terminate();
//      }
//      auto tp_start_target_year = aux::SimulationClock::time_point_from_string(
//                                                                    std::to_string(start_target_year)+"-01-01_00:00");
//      //init vars
//      auto capex_target_year  =  0.;
//      auto fopex_target_year =  0;
//      auto vopex_target_year =  0;
//      auto cost_target_year = 0.;
//      if (tp_start_target_year < tp_end_operation_){
//        capex_target_year +=  model_.getDiscountedValue("CAPEX", tp_start_target_year, tp_end_operation_, tp_start_operation_);
//        fopex_target_year +=  model_.getDiscountedValue("FOPEX", tp_start_target_year, tp_end_operation_, tp_start_operation_);
//        vopex_target_year +=  model_.getDiscountedValue("VOPEX", tp_start_target_year, tp_end_operation_, tp_start_operation_);
//      } else {
//        std::cerr << "ERROR in HSMOperation::CalculateFitness() weighting of last year not possible: tp_start_target year > | >= tp_end_operation!" << std::endl;
//        std::terminate();
//      }
//      //add discounted cost of target year
//        cost_target_year += capex_target_year;
//        cost_target_year += fopex_target_year;
//        cost_target_year += vopex_target_year;
//        //mean if the weighting is for more than one year
//        if (genesys::ProgramSettings::get_cma_weight_target_duration_years() > 1)
//        	cost_target_year = cost_target_year / (aux::SimulationClock::year(tp_end_operation_)-start_target_year);
//
//  if (analyse) {
//    //calculate annual values for for output
//    model_.calculate_annual_electricity_prices(tp_start_operation_, tp_end_operation_);
//    model_.calculate_annual_disc_capex(tp_start_operation_, tp_end_operation_);
//  }
//  //END WEIGHTING MECHANISMS FOR LAST OPTIMISATION YEAR==================================================================
//
//  double all_realCost = capex + vopex + fopex;
//  //Total cost relevant for the operation fitness:
//  double total_cost = all_realCost + weight_target*cost_target_year;
//
//  //Application of PENALTIES===========================================================================================
//  auto sum_usload =   model_.getSumValue("unsupplied_load",  tp_start_operation_, tp_end_operation_);
//  auto sum_energy =  model_.getSumValue("ENERGY", tp_start_operation_, tp_end_operation_);
//  //calculate
//  auto penalties = calculate_penalties(total_cost, sum_usload ,sum_energy);//return std::make_tuple(total_cost,sum_usload, penalties_SQ);
//  total_cost += std::get<0>(penalties);
//  sum_usload += std::get<1>(penalties);
//  double penalties_SQ =  std::get<2>(penalties);
//  // END PENALTIES=====================================================================================================
//
//  double LCOE = 0.0;
//  if ( energy != 0) {
//    LCOE =  all_realCost  / energy;
//    fitness_ = total_cost / energy;
//  } else {
//    std::cerr << "ERROR in HSMOperation::CalculateFitness(): calculated Energy resulted in DIVbyZERO Error!" << std::endl;
//  }
//  //GENERATE OUTPUT====================================================================================================
//  #pragma omp critical
//  {
//      std::cout <<  "capx="    <<std::setw(11)<< capex
//                << " fopx="    <<std::setw(11)<< fopex
//                << " vopx="    <<std::setw(11)<< vopex
//                << " penUL="   <<std::setw(11)<< accumulated_penalties_unsupplied_load_
//			        	<< " UL[GWh]=" <<std::setw(11)<< sum_usload
//                << " penSQ="   <<std::setw(11)<< penalties_SQ
//                << " enrgy="   <<std::setw(11)<< energy
//                << " LCOE="    <<std::setw(11)<< LCOE/1e4 << " ct/kWh"
//                << " fitness=" << fitness_ << std::endl;
//  }
//  //create return values-map
//  result_map.emplace("fitness", fitness_);
//  //results.push_back(fitness_);
//  if (analyse){
//	result_map.emplace("energy", energy);
//	result_map.emplace("capex", capex);
//	result_map.emplace("fopex", fopex);
//	result_map.emplace("vopex", vopex);
//	result_map.emplace("pen_unsupplied_load", accumulated_penalties_unsupplied_load_);
//	result_map.emplace("pen_self_supply", penalties_SQ);
//	result_map.emplace("lcoe_ct/kWh", LCOE/1e4);
//	std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wall_time_timer);
//	std::cout << "Analysis finished in " << wctduration.count() << " seconds [Wall Clock]" << std::endl;
//  result_map.emplace("analysis_timer_sec", wctduration.count());
//  }
//  return (result_map); //fitness_);
//}

void HSMOperation::set_annual_lookups(const aux::SimulationClock& clock){
	model_.set_annual_lookups(clock);
}

void HSMOperation::save_unsupplied_load(const aux::SimulationClock& clock) {
  //std::cout<<"DEBUG FUNC-ID = HSMOperation::add_penalties_unsupplied_load_()" <<std::endl;
  model_.save_unsupplied_load(clock);
}

void HSMOperation::add_energy_unsupplied_(const aux::SimulationClock& clock) {
  std::cout<<"FUNC-ID: HSMOperation::add_energy_unsupplied_" << clock.year() << std::endl;
  if(model_.add_annual_unsupplied_total_(clock)){
    return;
  } else {
    std::cerr << "ERROR in HSMOperation::add_energy_unsupplied_ - function failed in model_" << std::endl;
    std::terminate();
  }
}

void HSMOperation::startSequencer() {
  //DEBUG std::cout << "FUNC-ID: HSMOperation::startSequencer() " << std::endl;

  ///Sequencer to avoid memory problems
  for (int current_seq = 0; current_seq < num_operation_sequence_iterations_; current_seq++) {
    //DEBUG std::cout << "HSM sequencer running seq-no: " << current_seq  << " of " << num_operation_sequence_iterations_ << " iterations"<< std::endl;
    auto tp_start = tp_start_operation_ + current_seq*duration_operation_sequence_; //valid also for last seq.

    solveSequence(tp_start, duration_operation_sequence_);
    //transfer_persisting_data(tp_start, duration_operation_sequence_);
    //add_OaM_cost(tp_start+duration_operation_sequence_);//OaM for last year of sequence
  }
  //end sequencer

  //calculate last sequence if  duration_last_operation_sequence_  > 0
  const auto my_duration = duration_last_operation_sequence_;

  if (aux::SimulationClock::duration(0) < my_duration) {
    auto tp_start = tp_start_operation_ + num_operation_sequence_iterations_*duration_operation_sequence_;
    solveSequence(tp_start, my_duration);
    //add_OaM_cost(tp_start+my_duration);//OaM for last year of sequence
  } else {
    //duration of last sequence was zero - do nothing
  }
}

void HSMOperation::solveSequence(aux::SimulationClock::time_point tp_start_sequence,
                                 aux::SimulationClock::duration duration) {
  //DEBUG  std::cout << "FUNC-ID: HSMOperation::solveSequence()"<< std::endl;
   //Set up the Clock
  aux::SimulationClock main_clock(tp_start_sequence, genesys::ProgramSettings::simulation_step_length());
  aux::SimulationClock::time_point tp_end_seq = tp_start_sequence + duration;
  //Reset the operation variables from former sequence
  resetSequencedModel(main_clock);//reset TSCsumable of hourly-basis, preserve annual values, set SOC from prior sequence
  calculateResidualLoadTS(dm_hsm::HSMCategory::RE_GENERATOR, main_clock.now(), tp_end_seq, main_clock.tick_length());

	int current_year = main_clock.year();
	//	int counts = 0;
	add_OaM_cost(main_clock.now()); //first year
	set_annual_lookups(main_clock);
  //Hourly Calculation
  do { //std::cout << "TIME:" << aux::SimulationClock::time_point_to_string(main_clock.now()) << std::endl;
    resetCurrentTP(main_clock);//update RL etc.
    ///Things that should be updated annually
    if (main_clock.year() - current_year != 0 ||  //or last year end of simulation
        main_clock.now() + aux::minutes(genesys::ProgramSettings::simulation_step_length()) > tp_end_seq) {
      //DEBUG std::cout << "current year = " << current_year << std::endl;
      add_OaM_cost(main_clock.now());
      calc_self_supply_quota_and_apply_penalties(main_clock);
      set_annual_lookups(main_clock);
      //print_current_model_capacities(main_clock);
      current_year = main_clock.year();
      //std::terminate();

    }
    //every year but not beginning first year
    if ((main_clock.year() - aux::SimulationClock::year(genesys::ProgramSettings::simulation_start()))!= 0 ){
      //decommission_plants(); TODO: check if a strategy with memory parameter can be implemented!
      uncheck_active_current_year();
    } else {
      //only first year
    }
    static bool local = true;
//    std::cout << "==============NEXT HSM STEP: add must-run capacities to residual load================="<< std::endl;
//    generalised_balance(local, dm_hsm::HSMCategory::CONV_MUSTRUN, main_sim_clock);
    //std::cout << "==============NEXT HSM STEP: balance via grid: residual load from RE+must-run with other regions"<< std::endl;
    generalised_balance(!local, dm_hsm::HSMCategory::RE_GENERATOR, main_clock);
    // std::cout << "==============NEXT HSM STEP: discharge heat storage" << std::endl;

    /* Heat Integrationsversuch: kja
    if(genesys::ProgramSettings::modules().find("heat")->second){
      generalised_balance(local, dm_hsm::HSMCategory::HEAT_STORAGE, main_clock);}
    if(genesys::ProgramSettings::modules().find("heat")->second){
	     generalised_balance(local, dm_hsm::HSMCategory::EL2HEAT, main_clock);}
    if(genesys::ProgramSettings::modules().find("heat")->second){
      generalised_balance(local, dm_hsm::HSMCategory::HEAT_AND_EL, main_clock);}
    */

    // std::cout << "==============NEXT HSM STEP: balance locally: with short term storage=================="<< std::endl;
    generalised_balance(local, dm_hsm::HSMCategory::ST_STORAGE, main_clock);
    //std::cout << "==============NEXT HSM STEP: balance locally: with long term storage and power-to-X===="<< std::endl;
    generalised_balance(local, dm_hsm::HSMCategory::LT_STORAGE, main_clock);
    //std::cout << "==============NEXT HSM STEP: balance via grid: with long term storage and power-to-X===="<< std::endl;
    generalised_balance(!local, dm_hsm::HSMCategory::LT_STORAGE, main_clock);
    //std::cout << "==============NEXT HSM STEP/ Dispachable Generators=================================="<< std::endl;

    /* Heat Integrationsversuch: kja
    if(genesys::ProgramSettings::modules().find("heat")->second){
              generalised_balance(local, dm_hsm::HSMCategory::EL2HEAT_STORAGE, main_clock);}
    */

    generalised_balance(local, dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR, main_clock);
    //std::cout << "==============NEXT HSM STEP/ balance via grid: global dispatch of remaining generators=================================="<< std::endl;
    //TODO
    generalised_balance(!local, dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR, main_clock);

    /* Heat Integrationsversuch: kja
    // and remaining heat is balanced by gas/oil burner and generates operation cost in the respective region
    if(genesys::ProgramSettings::modules().find("heat")->second){
      generalised_balance(local, dm_hsm::HSMCategory::HEAT_GENERATOR, main_clock);}
    */

    save_unsupplied_load(main_clock);
    //    ++counts;
  } while (main_clock.tick() <= tp_end_seq);
  //std::cout << "counts= " << counts<< std::endl;
  setTransferStoredEnergy(main_clock); //store longterm SOC in vector for transfer to next sequence
  //std::cout << "DEBUG: END HSMOperation::solveSequence()" << std::endl;
}

//void HSMOperation::transfer_persisting_data(aux::SimulationClock::time_point tp_start_sequence,
//                                            aux::SimulationClock::duration duration) {
//  model_.transfer_persisting_data(clock);
//}

void HSMOperation::resetSequencedModel(const aux::SimulationClock& clock) {
  //std::cout << "FUNC-ID: HSMOperation::resetSequencedModel()" << std::endl;
  model_.resetSequencedModel(clock);
}

void HSMOperation::resetCurrentTP(const aux::SimulationClock& clock) {
  //std::cout << "FUNC-ID: HSMOperation::resetCurrentTP()" << std::endl;
  model_.resetCurrentTP(clock);
  //std::cout << "FUNC-END: HSMOperation::resetCurrentTP()" << std::endl;
}

void HSMOperation::decommission_plants(){
  //Powerplants which were not used in a past year are decommissioned / not generating anymore capacity and no more annual revision cost
  model_.decommission_plants();
}


void HSMOperation::uncheck_active_current_year(){
	//DEBUG std::cout << "FUNC-ID: HSMOperation::uncheck_active_current_year()" << std::endl;
  model_.uncheck_active_current_year();
}

void HSMOperation::setTransferStoredEnergy(const aux::SimulationClock& clock) {
  //DEBUG std::cout << "FUNC-ID: HSMOperation::setTransferStoredEnergy" << std::endl;
  model_.setTransferStoredEnergy(clock);
}

void HSMOperation::calculateResidualLoadTS(dm_hsm::HSMCategory hsm_cat,
                                           aux::SimulationClock::time_point tp_start_seq,
                                           aux::SimulationClock::time_point tp_end_seq,
                                           aux::SimulationClock::duration  tick_length) {
  //DEBUG std::cout << "FUNC-ID: HSMOperation::calculateResidualLoad for TS" << std::endl;
  model_.CalculateResidualLoad(hsm_cat, tp_start_seq, tp_end_seq, tick_length);
  //DEBUG std::cout << "FUNC-END: HSMOperation::calculateResidualLoad for TS" << std::endl;
}

void HSMOperation::calculate_sum_values(const aux::SimulationClock::time_point,
                                        const aux::SimulationClock::time_point) {
  //calculate the annual sums for sum_.. variables in model
  //TODO  model_->CalculateAnnualSums(tp_start_seq, tp_end_seq);
  std::cerr << "WARNING not implemented: HSMOperation::calculate_sum_values" << std::endl;
  std::terminate();
}

void HSMOperation::generalised_balance(bool local,
                                       const dm_hsm::HSMCategory& cat,
                                       const aux::SimulationClock& clock) {
  model_.generalised_balance(local, cat, clock);
}

void HSMOperation::add_OaM_cost(aux::SimulationClock::time_point tp_now) {
  //std::cout << "CALLED HSMOperation::add_OaM_cost - tp: " << aux::SimulationClock::time_point_to_string(tp_now) << std::endl;
	//TODO if sys_component was not active before in the current year add OaM_cost
	model_.add_OaM_cost(tp_now);
}

void HSMOperation::print_current_model_capacities(const aux::SimulationClock& clock){
  //std::cout << "DEBUG: HSMOperation:::print_current_model_capacities(const aux::SimulationClock& clock)" << std::endl;
  model_.print_current_model_capacities(clock);
}

void HSMOperation::calc_self_supply_quota_and_apply_penalties(const aux::SimulationClock& clock){
  //Calculate the annual self-supply quota
  double selfsupply_ratio = 0.;
  selfsupply_ratio += model_.calc_annual_selfsupply(clock);
  //Apply penalties to tracking variable
  if (selfsupply_ratio > genesys::ProgramSettings::approx_epsilon()) {
  	  accumulated_penalties_selfsupply_quota_ += selfsupply_ratio;
  }
}

std::tuple<std::pair<double,double> , std::pair<double, double> , std::pair<double, double>> HSMOperation::calculate_penalties(double sum_real_cost, double sum_energy){
  //std::cout << "DEBUG: FUNC-ID = calculate_penaltiesV2()" << std::endl;
  double pen_unsupplied = 0.0;
  double engy_unsupplied = 0.0;
  double pen_sq = 0.0;
  double engy_sq = 0.0;
  double pen_third = 0.0;
  double engy_third = 0.0;

  engy_unsupplied += model_.calculate_unsupplied_load();
  //pen_unsupplied +=engy_unsupplied*genesys::ProgramSettings::penalty_unsupplied_load();
  //scaling of unsupplied penalty
    //quota of total energy
  try{
      double quota_us = engy_unsupplied/sum_energy;
      if (quota_us < 1e-4 ){ //smaller penalty applied if very small deviation *promille of energy
        pen_unsupplied +=engy_unsupplied*genesys::ProgramSettings::penalties().find("unsupplied_load")->second;
        //pen_unsupplied +=engy_unsupplied*genesys::ProgramSettings::penalty_unsupplied_load();
        //std::cout << "DEBUG: small quota energy=" << quota_us <<  std::endl;
      } else {
        pen_unsupplied = std::pow((1+ engy_unsupplied),2)* genesys::ProgramSettings::penalties().find("unsupplied_load")->second;
      }
    } catch (const std::exception& e) {
      std::cerr << "Error in HSMOperation::calculate_penaltiesV2 - could not calculate quota_us, sum_energy zero?" << sum_energy << std::endl;
      std::cerr << e.what();
      std::terminate();
    }
  //std::cout << engy_unsupplied << " GWh unsupplied in total | " << engy_unsupplied*genesys::ProgramSettings::penalty_unsupplied_load() << " EUR total pen" << std::endl;
  //  auto unweighted_usload = model_.getSumValue("uweight_usload",  tp_start_operation_, tp_end_operation_);
  //  std::cout << "uweig_usload[GWh]" << unweighted_usload << std::endl;
  ////  calc_pen_sq();

    ////////// PENALTIES for Self-Supply Quota//////////
    if (accumulated_penalties_selfsupply_quota_ > genesys::ProgramSettings::approx_epsilon()) {
      pen_sq = accumulated_penalties_selfsupply_quota_ *genesys::ProgramSettings::penalties().find("self_supply_quota")->second;
      engy_sq = accumulated_penalties_selfsupply_quota_;
    }

  auto unsupplied = std::make_pair(pen_unsupplied, engy_unsupplied);
  auto selfsupply = std::make_pair(pen_sq, engy_sq);
  auto third = std::make_pair(pen_third, engy_third);
  return std::make_tuple(unsupplied ,selfsupply, third);
}

//DEPRECATED
//std::tuple<double , double , double > HSMOperation::calculate_penalties(double total_cost, double sum_usload, double sum_energy){
//  //std::cout<<"DEBUG: FUNC-ID=HSMOperation::calculate_penalties()" << std::endl;
//  //std::tuple<double,double > HSMOperation::calculate_penalties(double sum_usload, double sum_energy){
//  //Penalties Self supply quota
//  double pen_selfs_supply_quota =0.;
//  double pen_unsupplied =0.;
//
//  if (accumulated_penalties_selfsupply_quota_ > genesys::ProgramSettings::approx_epsilon()) {
//    pen_selfs_supply_quota = accumulated_penalties_selfsupply_quota_ *genesys::ProgramSettings::penalties().find("self_supply_quota")->second;
//  }
//  //total_cost += pen_selfs_supply_quota;
//  //Penalties unsupplied load quota
//  try{
//    double usload_quota = sum_usload/sum_energy;
//    if (usload_quota > 0.01){
//      //total_cost += accumulated_penalties_unsupplied_load_;
//      pen_unsupplied  +=  accumulated_penalties_unsupplied_load_;
//      //std::cout << "\tCASE A: UL_quota was greater 1 %: "<< usload_quota << std::endl;
//    } else if (usload_quota > 0.001){
//        accumulated_penalties_unsupplied_load_ = sum_usload*usload_quota*genesys::ProgramSettings::penalty_unsupplied_load();
//        //std::cout << "\tCASE B: UL_quota was greater 0.1 %: "<< usload_quota << std::endl;
//        accumulated_penalties_unsupplied_load_ = sum_usload;
//        //std::cout << "\tCASE C: UL_quota was very small: "<< usload_quota << std::endl;
//        }
//  } catch (const std::exception& e) {
//    std::cerr << "could not calculate usload_quota, sum_energy zero?" << sum_energy << std::endl;
//    std::cerr << e.what();
//    std::terminate();
//  }
//  return std::make_tuple(pen_unsupplied ,sum_usload, pen_selfs_supply_quota);
//
//}

} /* namespace dm_hsm */
