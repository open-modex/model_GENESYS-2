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
// dynamic_model.h
//
// This file is part of the genesys-framework v.2/*

#include <dynamic_model_hsm/dynamic_model.h>

#include <cmath> //pow for penalties
//random iterator for region map in balance
#include <random>
#include <algorithm>
#include <iterator>

#include <unordered_map>
#include <utility>

namespace dm_hsm {

DynamicModel::DynamicModel(const DynamicModel& other)
     : StaticModel(other),
       region_codes_{},
       link_codes_{},
       annual_electricity_price_(other.annual_electricity_price_){
  if (!other.regions().empty()) {
      for (const auto& it : other.regions()){
        regions_.emplace(it.first, std::shared_ptr<Region>(new Region(*(it.second))));
        region_codes_.push_back(it.first);
      }
  }
  if (!other.links().empty()) {
      for (const auto& it : other.links()){
        links_.emplace(it.first, std::shared_ptr<Link>(new Link(*(it.second))));
        link_codes_.push_back(it.first);
      }
  }
  if (!other.global().empty()) {
	  //DEBUG std::cout << "\t***DEBUG DynamicModel::DynamicModel CopyC'Tor: found global in other model! " << std::endl;
	  for (const auto& it : other.global())
         global_.emplace(it.first, std::shared_ptr<Global>(new Global(*(it.second))));
  }
}

DynamicModel::DynamicModel(const sm::StaticModel& origin)
    : sm::StaticModel(origin),
      annual_electricity_price_(),
      annual_unsupplied_total_(){
  if (!sm::StaticModel::regions().empty()) {
      for (const auto& it : StaticModel::regions()) {
        regions_.emplace(it.first, std::shared_ptr<Region>(new Region(*(it.second))));
        region_codes_.push_back(it.first);
      }
  }
  if (!sm::StaticModel::links().empty()) {
      for (const auto& it : StaticModel::links()) {
        auto new_link_ptr = std::shared_ptr<Link>(new Link(*(it.second), regions()));
        new_link_ptr->RegisterWithRegions(new_link_ptr);
        links_.emplace(it.first, new_link_ptr);
        link_codes_.push_back(it.first);
      }
  }
  if (!sm::StaticModel::global().empty()) {
	  //DEBUGstd::cout << "\t***DEBUG: DynamicModel::DynamicModel C'Tor: found global in StaticModel! " << std::endl;
	  for (const auto& it : StaticModel::global())
	    global_.emplace(it.first, std::shared_ptr<Global>(new Global(*(it.second))));
  }
  setHSMCategoriesFromCode();//TODO This should be updated to internal representation input/output
  if (!regions_.empty()){
    for (const auto& it : regions_){
      it.second->connectReservoirs();
    }
  }
  if (!global_.empty()) { //Switch pointers to global reservoirs
	  for (const auto& it : global_) {
	    for(const auto& pe : it.second->primary_energy_ptrs_dm()) {
	      //std::cout << "DM c'tor: found global prim engy: " << pe.first << std::endl;
	      if (pe.first == "CO2") {
	        //std::cout << "\t***one of them is co2" << std::endl;
	        //it regions
	        //connectGlobalCO2

	      } else {
	        std::cout << "\tDM c'tor: found not co2 - not defined yet!" << std::endl;
	      }

	    }


		  if (auto co2_ptr = it.second->getCO2ptr()) {
		    //std::cout << "global co2 found: " << co2_ptr->code() << " trying to switch ptrs...." << std::endl;
		    if (!regions_.empty()){
		      for (const auto& r : regions_){
		        r.second->connectGlobalReservoirs("CO2", co2_ptr);
//		        if (!it.second->storage_ptrs_sm().empty()) {
//		          std::cout << "found global storages in "<< r.second->code() << std::endl;
//		        }
		      }
		    }

		  }
	  }
  }
}

double DynamicModel::getDiscountedValue(std::string query,
                                        aux::SimulationClock::time_point start,
                                        aux::SimulationClock::time_point end,
                                        aux::SimulationClock::time_point discount_present) const {
  //	std::cout << "FUND-ID: DynamicModel::getDiscountedValue with query: " << query << std::endl;
  double discounted_value = 0.0;
  //collect from all region
  if (!regions_.empty()) {
    for (auto&& it : regions_) {
      discounted_value += it.second->getDiscountedValue(query, start, end, discount_present);
    }
  }
  //collect from all links
  if (!(query == "ENERGY")) {
    if (!links_.empty()) {
      for (auto&& it : links_) {
        discounted_value += it.second->getDiscountedValue(query, start, end, discount_present);
      }
    }
  }
  //any global values to add?
  return (discounted_value);
}

double DynamicModel::getSumValue(std::string query,
                                 aux::SimulationClock::time_point start,
                                 aux::SimulationClock::time_point end) const{
  //  std::cout << "FUND-ID: DynamicModel::getSumValue with query: " << query << std::endl;
  double sum_value = 0.0;
  //collect from all region
  if (!regions_.empty()) {
    for (auto&& it : regions_) {
      sum_value += it.second->get_sum(query, start, end);
    }
  }
  //collect from all links
  if (!(query == "ENERGY")  && !(query == "unsupplied_load")){
    if (!links_.empty()) {
      for (auto&& it : links_) {
        sum_value += it.second->getSumValue(query, start, end);
      }
    }
  }
  //  if(query=="uweight_usload"){
  //    //std::cout <<"unweighted usload"<< std::endl;
  //    //    annual_electricity_price_.PrintToString();
  //    std::cout <<"DynamicModel::getSumValue - uweight_usload as query argument not valid anymore!"<< std::endl;
  //    std::cerr <<"DynamicModel::getSumValue - uweight_usload as query argument not valid anymore!"<< std::endl;
  //    std::terminate();
  //   return (aux::sum(annual_unsupplied_total_, start, end, aux::years(1)));
  //  }
  //any global values to add?
  return (sum_value);
}

void DynamicModel::calculate_annual_disc_capex(aux::SimulationClock::time_point start,
                                          aux::SimulationClock::time_point end) {
  auto mystart = start;
  do {
    if (!regions_.empty()) {
      for (auto&& it : regions_) {
        it.second->calculated_annual_disc_capex(mystart);
      }
    }
    if (!links_.empty()) {
      for (auto&& it : links_) {
        it.second->calculated_annual_disc_capex(mystart);
      }
    }
    mystart += aux::years(1);

  } while (mystart < end);
}

void DynamicModel::calculate_annual_electricity_prices(aux::SimulationClock::time_point start,
                                                       aux::SimulationClock::time_point end) {
  //std::cout << "DynamicModel::calculate_annual_electricity_prices" << std::endl;
  auto mystart = start;
  do {
    double cost_current_year = 0.;
    double discounted_energy_current_year = 0.;
    double lcoe_current_year = 0.;
    if (!regions_.empty()) {
        for (auto&& it : regions_) {
          discounted_energy_current_year += it.second->getDiscountedValue("ENERGY", mystart, (mystart+aux::years(1)), start);
          cost_current_year += it.second->getDiscountedValue("CAPEX", mystart, (mystart+aux::years(1)), start);
          cost_current_year += it.second->getDiscountedValue("FOPEX", mystart, (mystart+aux::years(1)), start);
          cost_current_year += it.second->getDiscountedValue("VOPEX", mystart, (mystart+aux::years(1)), start);
        }
    }
    if (!links_.empty()) {
      for (auto&& it : links_) {
        cost_current_year += it.second->getDiscountedValue("CAPEX", mystart, (mystart+aux::years(1)), start);
        cost_current_year += it.second->getDiscountedValue("FOPEX", mystart, (mystart+aux::years(1)), start);
        cost_current_year += it.second->getDiscountedValue("VOPEX", mystart, (mystart+aux::years(1)), start);
      }
    }
    if (discounted_energy_current_year > 0.) {
      lcoe_current_year = cost_current_year / discounted_energy_current_year;
    }
    //std::cout << " | LCOE = " << lcoe_current_year << " | DISC_ENERGY = " << discounted_energy_current_year << std::endl;
    annual_electricity_price_ += aux::TimeSeriesConst(std::vector<double>{lcoe_current_year, 0.}, mystart, aux::years(1));
    mystart += aux::years(1);
  } while (mystart < end);
}

void DynamicModel::setTransferStoredEnergy(const aux::SimulationClock& sclock){
  for (auto&& it : regions_){
    //save soc to persitend_SOC_ for each storage in the region
    it.second->setTransferStoredEnergy(sclock);
  }
}

void DynamicModel::CalculateResidualLoad(dm_hsm::HSMCategory hsm_cat,
                                         aux::SimulationClock::time_point tp_start,
                                         aux::SimulationClock::time_point tp_end_seq,
                                         aux::SimulationClock::duration tick_length) {
  //DEBUG   std::cout << "FUNC-ID: DynamicModel::hsmCalculateResidualLoad for TS" << std::endl;
  for (auto& it : regions_) {
    //calucalte RL of each region
    it.second->calculateResidualLoad(hsm_cat, tp_start, tp_end_seq, tick_length);
    if(genesys::ProgramSettings::modules().find("heat")->second)
      it.second->init_heat_load(tp_start, tp_end_seq, tick_length);
  }
}

//void DynamicModel::transfer_persisting_data(const aux::SimulationClock& clock){
//  //annual_unsupplied_total_ =
//}

void DynamicModel::resetSequencedModel(const aux::SimulationClock& clock){
  //std::cout << "FUNC-ID: DynamicModel::resetSequencedModel()" << std::endl;
  //Reset all values to standard
  for (auto &it : regions_){
    it.second->resetSequencedRegion(clock);
  }
  for (auto &it : links_){
	  it.second->resetSequencedLink(clock);
  }
  for (auto &it : global_) {
    it.second->resetSequencedGlobal(clock);
  }
}

void DynamicModel::resetCurrentTP(const aux::SimulationClock& clock){
  //DEBUG std::cout << "FUNC-ID: DynamicModel::resetCurrentTP()"<<std::endl;
    //reset transported energy for regions
  if(!regions_.empty()){
    for (auto& it : regions_){
       it.second->resetCurrentTP(clock);
    }
  } else {
    std::cerr <<"ERROR: DynamicModel::resetCurrentTP -> regions_ is empty!" << std::endl;
    std::terminate();
  }
  if(!links_.empty()){
    for (auto& it : links_){
       it.second->resetCurrentTP(clock);
    }
  } else {
    //std::cerr <<"***WARNING: DynamicModel::resetCurrentTP -> links_ is empty!" << std::endl;
  }
  if(!global_.empty()) {
	  for (auto& it : global_)
		  it.second->resetCurrentTP(clock);
  }
}

void DynamicModel::decommission_plants(){
  if (!regions_.empty()){
    for (const auto& it : regions_){
      it.second->decommission_plants();
    }
  }
}

void DynamicModel::uncheck_active_current_year(){
	if (!regions_.empty()){
	    for (const auto& it : regions_){
	      it.second->uncheck_active_current_year();
	    }
	  }
	if (!links_.empty()){
	    for (const auto& it : links_){
	      it.second->uncheck_active_current_year();
	    }
	  }
}

void DynamicModel::add_OaM_cost(aux::SimulationClock::time_point tp) {
	//	std::cout << "FUNC-ID: DynamicModel::add_OaM_cost at tp_now " << aux::SimulationClock::time_point_to_string(tp)  << " for precceeding year. " << std::endl;
  if (!regions_.empty()) {
	  for (const auto& it : regions_) {
		  it.second->add_OaM_cost(tp);
	  }
  }
  if (!links_.empty()) {
    for (const auto& it : links_) {
      it.second->add_OaM_cost(tp);
    }
  }
}

void DynamicModel::activate_mustrun(const aux::SimulationClock& sim_clock){
  std::cerr << "***WARNING: DynamicModel::activate_mustrun not implemented!" << std::endl;
  for (auto& it : regions_){
    //std::cout << "activate_mustrun in region " << it.first << std::endl;
    it.second->activate_mustrun(sim_clock);
  }
}

void DynamicModel::generalised_balance(bool local,
                                       const dm_hsm::HSMCategory& hsm_cat,
                                       const aux::SimulationClock& clock){

  /* Heat Integrationsversuch: kja
  //check that heat components are only applied locally
	if (!local && (hsm_cat == dm_hsm::HSMCategory::HEAT_GENERATOR || hsm_cat == dm_hsm::HSMCategory::HEAT_RESIDUAL || hsm_cat == dm_hsm::HSMCategory::HEAT_STORAGE)){
	std::cerr << "ERROR in HSM: HEAT Balancing only available locally!" << std::endl;
	std::terminate();
	}
  */

  //DEBUG std::cout << "FUNC-ID: DynamicModel::generalised_balance() with HSMCategory= "<< static_cast<int>(hsm_cat) << " | local = " << local<< std::endl;
  if (hsm_cat == dm_hsm::HSMCategory::CONV_MUSTRUN) {
    for (auto& it : regions_){
        //std::cout << "activate_mustrun in region " << it.first << std::endl;
        it.second->activate_mustrun(clock);
    }
  } else if (local) {
    for (auto& it : regions_){
      it.second->generalised_balance_local(hsm_cat, clock);
    }
  } else if (!local){
    int max_hops = genesys::ProgramSettings::gridbalance_hop_level();
    if ( 0  <= max_hops ) {//otherwise skip grid balanc if negative!
      //std::cout << "DEBUG: Execution of grid-balance: max_hops: " << max_hops << std::endl;
      int current_hops = 0;//0 = direct neighbours to start with
      do {

        if (genesys::ProgramSettings::use_randomisation()){
          //random balance one region first!
          std::random_device rd;
          std::mt19937_64 g(rd());
          std::shuffle(region_codes_.begin(), region_codes_.end(), g); //lookup vector
        }
        for (auto& it : region_codes_){
          auto search = regions_.find(it);
          if(search != regions_.end()) {
              //std::cout << "\t found " << search->first << std::endl;
              search->second->balance_start(current_hops, hsm_cat, clock);
          }
          else {
              std::cerr << "ERROR in DynamicModel::generalised_balance --> Region code : " << it << " not found" << std::endl;
              std::terminate();
          }
        }
        //DEPRECATED: loop through map has always same order
        //        for (auto& it : regions_){//iterate all regions for the current hop level
        //          std::cout << it.first << std::endl;
        //          it.second->balance_start(current_hops, hsm_cat, clock);
        //        }
        ++current_hops;//try to balance deeper hop level next
      } while (current_hops <= max_hops);
    }
	//	else {
	//	  std::cout << "DEBUG: No execution of grid-balance: max_hops < 0: " << max_hops << std::endl;
	//
	//	}
  } else{
    std::cerr << "ERROR in DynamicModel::generalised_balance() - could not identify subfunction" << std::endl;
    std::terminate();
  }
}

void DynamicModel::save_unsupplied_load (const aux::SimulationClock& clock) {
  for (auto& it : regions_){
    it.second->save_unsupplied_load(clock);
  }
  //unsupplied +=
  // double unsupplied = 0.0;
  //return unsupplied;
}

double DynamicModel::calculate_unsupplied_load(){
  //  std::cout<<"DEBUG: FUNC-ID = DynamicModel::calculate_unsupplied_load()"<<std::endl;
  double unsupplied = 0.0;
  for (auto& it : regions_){
    auto usLoad = it.second->get_remaining_residual_load_();

    //Build clock
    aux::SimulationClock clock(genesys::ProgramSettings::simulation_start(), genesys::ProgramSettings::simulation_step_length());
    aux::SimulationClock::time_point tp_end = genesys::ProgramSettings::simulation_end();
    int current_year = clock.year();
    int start_year = current_year;
    aux::SimulationClock::time_point tp_start_tmp = genesys::ProgramSettings::simulation_start();
    do {
      if (clock.year() - current_year != 0 ||  //or last year end of simulation
          clock.now() + aux::minutes(genesys::ProgramSettings::simulation_step_length()) > tp_end) { // tp_end is not exactly reached therefore check if clock exceeds
        auto step = aux::minutes(genesys::ProgramSettings::simulation_step_length());
        auto tp_justbefore_now = clock.now()- step;
        double current_year_usLoad = aux::sum_positiveValues(it.second->get_remaining_residual_load_(),
                                                             tp_start_tmp, tp_justbefore_now,
                                                             genesys::ProgramSettings::simulation_step_length());
        annual_unsupplied_total_ += aux::TimeSeriesConst(std::vector<double>{current_year_usLoad,0.0},
                                                         tp_start_tmp,
                                                         aux::years(1));
        it.second->set_annual_unsupplied(current_year_usLoad, tp_start_tmp);
        double factor = std::pow((1+genesys::ProgramSettings::interest_rate()), (current_year - start_year));
        //unsupplied of later years is expanded by factor to accound for lower worth of money in far future when penalty is applied.
        unsupplied += current_year_usLoad*factor;
        //DEBUG:
        //        std::cout << it.first  << " " << aux::SimulationClock::time_point_to_string(tp_start_tmp)
        //                  << " usLoad = " << current_year_usLoad << " GWh" << std::endl;
        tp_start_tmp = clock.now() ;
        current_year = clock.year();
      }
    } while (clock.tick() <= tp_end);
    //    double total_usLoad = aux::sum_positiveValues(it.second->get_remaining_residual_load_(), genesys::ProgramSettings::simulation_start(), genesys::ProgramSettings::simulation_end(), genesys::ProgramSettings::simulation_step_length());
    //    std::cout << "TOTOAL\n -----------\n usLoad = " << total_usLoad << " GWh \t\tin "<< it.first << " between " << aux::SimulationClock::time_point_to_string(genesys::ProgramSettings::simulation_start())<< " and " << aux::SimulationClock::time_point_to_string(genesys::ProgramSettings::simulation_end()) << std::endl;
    //    double usLoad_regio = aux::sum_positiveValues(usLoad,
    //                                                  genesys::ProgramSettings::simulation_start(),
    //                                                  genesys::ProgramSettings::simulation_end(),
    //                                                  genesys::ProgramSettings::simulation_step_length());
    //    std::cout << "\t region "<< it.first << " | " << usLoad_regio << " GWh" << std::endl;
    //    std::cout << usLoad.PrintToString().length() << std::endl;
  }
//  if(query=="uweight_usload"){
//    std::cout <<"unweighted usload"<< std::endl;
//   annual_electricity_price_.PrintToString();
//   return (aux::sum(annual_unsupplied_total_, start, end, aux::years(1)));
  //std::cout << "\tunsupplied load ts:" << annual_unsupplied_total_.PrintToString() << std::endl;
  return unsupplied;
}

void DynamicModel::print_current_model_capacities(const aux::SimulationClock& sclock){
  std::cout << "Timepoint: " << aux::SimulationClock::time_point_to_string(sclock.now()) << std::endl;
  std::cout << "Current state of all " << regions_.size() << " regions:" << std::endl;
  for (auto& it : regions_){
    it.second->print_current_capacities(sclock);
  }

  std::cout << "Current state of all " << links_.size() << " lines:" << std::endl;

  for (auto& it : links_) {
      it.second->print_current_capacities();
      std::cerr << "WARNING not implemented: DynamicModel::print_current_model_capacities call to link->PrintCurrentCapacities()" << std::endl;
      std::terminate();
  }
  std::cout << "====================END print_current_model_capacities========================================" << std::endl;
}

void DynamicModel::set_annual_lookups(const aux::SimulationClock& clock){
  //std::cout << "FUNC-ID: DynamicModel::set_annual_lookups" << std::endl;
	if (genesys::ProgramSettings::use_global_file()) {
		if(!global_.empty()){
			for (auto& it : global_) {
				//std::cout << "global: set_annual_lookups" << it.first<< std::endl;
				it.second->set_annual_lookups(clock);
			}
		}
	}
	if(!regions_.empty()){
		for (auto& it : regions_){
			it.second->set_annual_lookups(clock);
		}
	}
	if(!links_.empty()){
		for (auto& it : links_){
			it.second->set_annual_lookups(clock);
		}
	}
}

void DynamicModel::setHSMCategoriesFromCode() {
  //DEBUG std::cout << "FUNC-ID: DynamicModel::setHSMCategoriesFromCode" << std::endl;
  //map should be generated externally in the future
  std::unordered_map<std::string, dm_hsm::HSMCategory> keyMap;
  //RE_GENERATOR, CONV_MUSTRUN, DISPATCHABLE_GENERATOR, STORAGE_CHARGER, STORAGE_DISCHARGER, STORAGE_BICHARGER, LINE_CONVERTER, UNAVAILABLE
  //insert (key-code, HSM-Category)
  keyMap.emplace("WIND1", dm_hsm::HSMCategory::RE_GENERATOR);
  keyMap.emplace("WIND2", dm_hsm::HSMCategory::RE_GENERATOR);
  keyMap.emplace("WIND_OFF", dm_hsm::HSMCategory::RE_GENERATOR);
  keyMap.emplace("PV1", dm_hsm::HSMCategory::RE_GENERATOR);
  keyMap.emplace("PV2", dm_hsm::HSMCategory::RE_GENERATOR);
  keyMap.emplace("ROR", dm_hsm::HSMCategory::RE_GENERATOR);
  keyMap.emplace("OIL_TURBINE", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);
  keyMap.emplace("MOTOR_POWER", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);
  keyMap.emplace("GASN_TURBINE", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);
  keyMap.emplace("GASH2_TURBINE", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);
  keyMap.emplace("CCCONV_TURBINE", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);
  keyMap.emplace("CH4_OCTURBINE", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);//MultiConverter
  keyMap.emplace("CH4_CCTURBINE", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);//MultiConverter
  keyMap.emplace("NUCLEAR_TURBINE", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);//MultiConverter
  keyMap.emplace("NUCLEAR_TURBINE_DEU", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);//MultiConverter
  keyMap.emplace("NUCLEAR_TURBINE_GER", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);//MultiConverter
  keyMap.emplace("LIGNITE_OCTURBINE", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);//MultiConverter
  keyMap.emplace("HARDCOAL_OCTURBINE", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);//MultiConverter
  keyMap.emplace("BIOMASS_TURBINE", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);//MultiConverter
  keyMap.emplace("BIO_CH4_TURBINE", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);//MultiConverter
  keyMap.emplace("BIOMASS_FURNANCE", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);//MultiConverter
  keyMap.emplace("H2_ELECTROLYSER", dm_hsm::HSMCategory::LT_STORAGE);
  keyMap.emplace("CH4_GENERATOR", dm_hsm::HSMCategory::LT_STORAGE);
  keyMap.emplace("CCH2_TURBINE", dm_hsm::HSMCategory::LT_STORAGE);
  keyMap.emplace("CH4_TURBINE", dm_hsm::HSMCategory::LT_STORAGE);
  keyMap.emplace("PH_TURBINE", dm_hsm::HSMCategory::ST_STORAGE);
  keyMap.emplace("BATPOWER", dm_hsm::HSMCategory::ST_STORAGE);//generic
  keyMap.emplace("BAT1POWER", dm_hsm::HSMCategory::ST_STORAGE);//NaS
  keyMap.emplace("BAT2POWER", dm_hsm::HSMCategory::ST_STORAGE);//LiIon
  keyMap.emplace("BAT3POWER", dm_hsm::HSMCategory::ST_STORAGE);//PbAcid
  keyMap.emplace("BAT4POWER", dm_hsm::HSMCategory::ST_STORAGE);//VaRedox
  keyMap.emplace("EL_HVDC_OH", dm_hsm::HSMCategory::LINE_CONVERTER);
  keyMap.emplace("EL_HVDC_SEA", dm_hsm::HSMCategory::LINE_CONVERTER);
  keyMap.emplace("EL_AC", dm_hsm::HSMCategory::LINE_CONVERTER);
  keyMap.emplace("GAS_N", dm_hsm::HSMCategory::LINE_CONVERTER);
  keyMap.emplace("GAS_H2", dm_hsm::HSMCategory::LINE_CONVERTER);
  //openMODEX
  keyMap.emplace("Photovoltaics_Mid", dm_hsm::HSMCategory::RE_GENERATOR);
  keyMap.emplace("Photovoltaics_North", dm_hsm::HSMCategory::RE_GENERATOR);
  keyMap.emplace("Photovoltaics_South", dm_hsm::HSMCategory::RE_GENERATOR);
  keyMap.emplace("hvac_Mid_North", dm_hsm::HSMCategory::LINE_CONVERTER);
  keyMap.emplace("hvac_South_Mid", dm_hsm::HSMCategory::LINE_CONVERTER);
  keyMap.emplace("hvac_South_North", dm_hsm::HSMCategory::LINE_CONVERTER);
  keyMap.emplace("Gas_plant_Mid", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);
  keyMap.emplace("Gas_plant_North", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);
  keyMap.emplace("Gas_plant_South", dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR);
  keyMap.emplace("converter_Pump_storage_Mid", dm_hsm::HSMCategory::LT_STORAGE);
  keyMap.emplace("converter_Pump_storage_North", dm_hsm::HSMCategory::LT_STORAGE);
  keyMap.emplace("converter_Pump_storage_South", dm_hsm::HSMCategory::LT_STORAGE);

  /* Heat Integrationsversuch: kja
  if(genesys::ProgramSettings::modules().find("heat")->second) {
    std::cout << "activated heat module: adding additional converters to hierarchy" << std::endl;
    //available categories HEAT_GENERATOR, HEAT_STORAGE, HEAT_RESIDUAL
    keyMap.emplace("HEAT_STORE", dm_hsm::HSMCategory::HEAT_STORAGE);
    keyMap.emplace("HEAT2HEAT_STORAGE_CONVERTER", dm_hsm::HSMCategory::HEAT_STORAGE);//Kevin ???
    keyMap.emplace("HEATPUMP1", dm_hsm::HSMCategory::EL2HEAT);
    keyMap.emplace("GAS_HEATER", dm_hsm::HSMCategory::HEAT_GENERATOR); //Kevin warum Heat_RESIDUAL
    keyMap.emplace("BHKW", dm_hsm::HSMCategory::HEAT_AND_EL); //Kevin warum Heat_RESIDUAL
  }
  */

  //keymap for HSMSubCategory i.e. storages
  std::unordered_map<std::string, dm_hsm::HSMSubCategory> subKeyMap;
  subKeyMap.emplace("H2_ELECTROLYSER", dm_hsm::HSMSubCategory::CHARGER);
  subKeyMap.emplace("CH4_GENERATOR", dm_hsm::HSMSubCategory::CHARGER);
  subKeyMap.emplace("CCH2_TURBINE", dm_hsm::HSMSubCategory::DISCHARGER);
  subKeyMap.emplace("CH4_TURBINE", dm_hsm::HSMSubCategory::DISCHARGER);
  subKeyMap.emplace("PH_TURBINE", dm_hsm::HSMSubCategory::BICHARGER);
  subKeyMap.emplace("BATPOWER", dm_hsm::HSMSubCategory::BICHARGER);//generic
  subKeyMap.emplace("BAT1POWER", dm_hsm::HSMSubCategory::BICHARGER);//NaS
  subKeyMap.emplace("BAT2POWER", dm_hsm::HSMSubCategory::BICHARGER);//LiIon
  subKeyMap.emplace("BAT3POWER", dm_hsm::HSMSubCategory::BICHARGER);//PbAcid
  subKeyMap.emplace("BAT4POWER", dm_hsm::HSMSubCategory::BICHARGER);//VaRedox
  //openMODEX
  subKeyMap.emplace("converter_Pump_storage_Mid", dm_hsm::HSMSubCategory::BICHARGER);
  subKeyMap.emplace("converter_Pump_storage_North", dm_hsm::HSMSubCategory::BICHARGER);
  subKeyMap.emplace("converter_Pump_storage_South", dm_hsm::HSMSubCategory::BICHARGER);

  /* Heat Integrationsversuch: kja
  if(genesys::ProgramSettings::modules().find("heat")->second){
    subKeyMap.emplace("HEAT_STORE", dm_hsm::HSMSubCategory::BICHARGER);//generic
    subKeyMap.emplace("HEAT2HEAT_STORAGE_CONVERTER", dm_hsm::HSMSubCategory::BICHARGER);
  }
  */


  //update line converter
  for (auto& it : links_) {
    it.second->updateConverterHSMCategory(keyMap);
  }
  //update region converter

  for (auto& it : regions_) {
    // not implemented: it.second->autoSetHSMCategory();
    it.second->updateConverterHSMCategory(keyMap);
    it.second->updateConverterHSMSubCategory(subKeyMap);
  }

}

double DynamicModel::calc_annual_selfsupply(const aux::SimulationClock& clock){
  double selfsupply_quota =0.;
  int counter = 0.;
  for (auto& it : regions_) {
    double tmp_quota = it.second->calc_SelfSupplyQuota(clock);
    if (tmp_quota > 1. && tmp_quota < 3.) {
    	selfsupply_quota += std::pow(tmp_quota,3);//x^3 scaling of the regions quota>1
    } else if (tmp_quota > 3.) {
    	selfsupply_quota += std::pow(10*tmp_quota,3);//x^3 scaling of the regions quota>1
    } else {
    	selfsupply_quota += std::pow(tmp_quota,2);//x^3 scaling of the regions quota>1
    }
    if( tmp_quota > genesys::ProgramSettings::approx_epsilon()){
      counter ++;
    }
  }
  try {
    selfsupply_quota = selfsupply_quota/counter;
  } catch (const std::exception& e) {
    std::cout << "ERROR in DynamicModel::calc_annual_selfsupply : div by zero?" << std::endl;
    std::cout << e.what(); // information from error printed
  }
  //std::cout << "count" << counter << " | total SQ-Quota= " << selfsupply_quota << std::endl;
  return selfsupply_quota;
}

} /* namespace dm_hsm */
