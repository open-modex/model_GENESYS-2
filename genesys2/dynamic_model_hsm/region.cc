// ==================================================================
//
//  GENESYS2 is an optimisation tool and model of the European electricity supply system.
//
//  Copyright (C) 2015, 2016, 2017.  Robin Beer, Christian Bussar, Zhuang Cai, Kevin
//  Jacque, Alexander Fehler, Luiz Moraes Jr., Philipp Stoecker
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
// region.cc
//
// This file is part of the genesys-framework v.2

#include <dynamic_model_hsm/region.h>

#include <algorithm>
#include <chrono>
#include <exception>
#include <iostream>
#include <iomanip>
#include <utility>
#include <tuple>
#include <thread>

#include <auxiliaries/functions.h>
#include <io_routines/csv_all.h>
#include <program_settings.h>

namespace dm_hsm {

Region::Region(const sm::Region& origin)
    : sm::Region(origin),
      map_reserved_exports_tp_(),
      annual_co2_emissions_(),
      annual_consumed_heat_GWh_(),
      annual_generated_heat_GWh_(),
      annual_imported_electricity_GWh_(),
      annual_imported_gas_GWh_(),
      annual_exported_electricity_GWh_(),
      annual_exported_gas_GWh_(),
      annual_selfsupply_quota_(),
      annual_unsupplied_electricity_(),
	    //annual_max_demand_(),
      residual_load_(),
      residual_heat_(),
	    remaining_residual_load_(),
      remaining_residual_heat_load_(),
	    remaining_excess_heat_(),
      co2_emissions_(),
      exported_energy_(),
      exported_electricity_(),
      exported_gas_(),
      imported_energy_(),
      imported_electricity_(),
      imported_gas_(),
      import_for_local_balance_(),
      RL_init_(false),
      HEAT_init_(false),
      connected_to_grid_(false),
      supply_region_(false),
      balanced_(false),
      residual_load_TP_(0.),
      residual_heat_TP_(0.),
      reserved_residual_load_tp_(0.),
      available_power_for_export_(0.),
      max_pwr_exchange_grid_tp_(0.){
  //std::cout << "DEBUG: dm::region::region ctor" << std::endl;
  for (const auto& it : primary_energy_ptrs())
    primary_energy_ptrs_.emplace(it.first, std::shared_ptr<PrimaryEnergy>(new PrimaryEnergy(*it.second)));
  for (const auto& it : converter_ptrs())
    converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>(
        new Converter(static_cast<sm::Converter>(*it.second)))); // static_cast is necessary because of multiple inheritance
  if(!multi_converter_ptrs().empty()) {
    for (const auto& it : multi_converter_ptrs()){
      //converter as parent class pointer-> create all converters in one container
      converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>(
          new dm_hsm::MultiConverter(static_cast<sm::MultiConverter>(*it.second))));
    }
  }
  if(!storage_ptrs_sm().empty()) {
    for (const auto& it : storage_ptrs_sm()){
      storage_ptrs_.emplace(it.first, std::shared_ptr<Storage>(new Storage(*it.second)));
    }
  }
}

Region::Region(const Region& other)
    : sm::Region(other),
      map_reserved_exports_tp_(),
      annual_co2_emissions_(other.annual_co2_emissions_),
      annual_consumed_heat_GWh_(other.annual_consumed_heat_GWh_),
      annual_generated_heat_GWh_(other.annual_generated_heat_GWh_),
      annual_imported_electricity_GWh_(other.annual_imported_electricity_GWh_),
      annual_imported_gas_GWh_(other.annual_imported_gas_GWh_),
      annual_exported_electricity_GWh_(other.annual_exported_electricity_GWh_),
      annual_exported_gas_GWh_(other.annual_exported_gas_GWh_),
      annual_selfsupply_quota_(other.annual_selfsupply_quota_),
      annual_unsupplied_electricity_(other.annual_unsupplied_electricity_),
      residual_load_(other.residual_load_),
      residual_heat_(other.residual_heat_),
	    remaining_residual_load_(other.remaining_residual_load_),
      remaining_residual_heat_load_(other.remaining_residual_heat_load_),
      co2_emissions_(other.co2_emissions_),
      exported_energy_(other.exported_energy_),
      exported_electricity_(other.exported_electricity_),
      exported_gas_(other.exported_gas_),
      imported_energy_(other.imported_energy_),
      imported_electricity_(other.imported_electricity_),
      imported_gas_(other.imported_gas_),
      import_for_local_balance_(other.import_for_local_balance_),
      RL_init_(other.RL_init_),
      HEAT_init_(other.HEAT_init_),
      connected_to_grid_(other.connected_to_grid_),
      supply_region_(other.supply_region_),
      balanced_(other.balanced_),
      residual_load_TP_(other.residual_load_TP_),
      residual_heat_TP_(other.residual_heat_TP_),
      reserved_residual_load_tp_(other.reserved_residual_load_tp_),
      available_power_for_export_(other.available_power_for_export_),
      max_pwr_exchange_grid_tp_(other.max_pwr_exchange_grid_tp_){
  //std::cout << "DEBUG: CopyC-Tor dm::Region::Region" << std::endl;
  if (!other.converter_ptrs_.empty()) {
    for (const auto &it : other.converter_ptrs_) {
      if (auto multi_conv = std::dynamic_pointer_cast<dm_hsm::MultiConverter>(it.second)) {
            converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>(new MultiConverter(*multi_conv)));
      } else {
            converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>(new Converter(*it.second)));
      }
    }
  }
  if (!other.storage_ptrs_.empty()) {
    for (const auto &it : other.storage_ptrs_)
        storage_ptrs_.emplace(it.first, std::shared_ptr<Storage>(new Storage(*it.second)));
  }
  if (!other.primary_energy_ptrs_.empty()) {
      for (const auto &it : other.primary_energy_ptrs_)
        primary_energy_ptrs_.emplace(it.first, std::shared_ptr<PrimaryEnergy>(
            new PrimaryEnergy(*it.second)));
  }
}

void Region::updateConverterHSMCategory(std::unordered_map<std::string, dm_hsm::HSMCategory>& keymap) const {
  //DEBUG   std::cout << "FUNC-ID:  Region::updateConverterHSMCategory() " << std::endl;
  for (auto& it : converter_ptrs_) {
    std::string code = it.second->code();
    //std::string region = this->code();
    //DEBUG std::cout << "Doing updateConverterHSMCategory of " << code << std::endl;
    it.second->setHSMcategory(keymap.find(code)->second);
  }
}

void Region::updateConverterHSMSubCategory(std::unordered_map<std::string, dm_hsm::HSMSubCategory>& subKeyMap) const {
  //DEBUG   std::cout << "FUNC-ID:  Region::updateConverterHSMSubCategory() " << std::endl;
  for (auto& it : converter_ptrs_) {
    if ((it.second->get_HSMCategory() == dm_hsm::HSMCategory::LT_STORAGE) ||
        (it.second->get_HSMCategory() == dm_hsm::HSMCategory::ST_STORAGE)) {
      std::string code = it.second->code();
      std::string region = this->code();
      //DEBUG std::cout << "\t Region: " << region << " doing updateConverterHSMSubCategory of " << code << std::endl;
      it.second->setHSMSubCategory(subKeyMap.find(code)->second);
    }

    /* Heat Integrationsversuch: kja
    if ((it.second->get_HSMCategory() == dm_hsm::HSMCategory::LT_STORAGE) ||
        (it.second->get_HSMCategory() == dm_hsm::HSMCategory::ST_STORAGE) ||
        (it.second->get_HSMCategory() == dm_hsm::HSMCategory::HEAT_STORAGE)) {
      std::string code = it.second->code();
      std::string region = this->code();
      //DEBUG std::cout << "\t Region: " << region << " doing updateConverterHSMSubCategory of " << code << std::endl;
      it.second->setHSMSubCategory(subKeyMap.find(code)->second);
    }
    */
  }
}

void Region::autoSetHSMCategory(){
// std::cout << "DEBUG: Region::autoSetHSMCategory() not implemented yet" << std::endl;
//  for (auto& it: converter_ptrs_){
//    auto in = it.second->input_type();
//    auto out = it.second->output_type();
//    std::cout << "in = " << in << "\nout = " << out << std::endl;
//
//    std::vector<std::string> RE_GENERATOR ={"WIND", "PV", "ROR"};
//    std::vector<std::string> DISPATCHABLE_GENERATOR ={"COAL", "LIGNITE", "GAS"};
//
//
//    for (auto& str : RE_GENERATOR){
//        if (it.second->code().find(str) != std::string::npos){
//        std::cout << "found RE:"<< it.second->code() << '\n';
//        it.second->setHSMcategory(dm_hsm::HSMCategory::RE_GENERATOR);
//        }
//    }
//
//    if (in == "HEAT" && out == "HEAT") {
//      std::cout << it.first << std::endl;
//    }
//    keyMap.emplace("WIND1", dm_hsm::HSMCategory::RE_GENERATOR);
//    keyMap.emplace("WIND2", dm_hsm::HSMCategory::RE_GENERATOR);
//    keyMap.emplace("WIND_OFF", dm_hsm::HSMCategory::RE_GENERATOR);
//    keyMap.emplace("PV1", dm_hsm::HSMCategory::RE_GENERATOR);
//    keyMap.emplace("PV2", dm_hsm::HSMCategory::RE_GENERATOR);
//    keyMap.emplace("ROR", dm_hsm::HSMCategory::RE_GENERATOR);
}

void Region::connectGlobalReservoirs(std::string output_type,std::shared_ptr<PrimaryEnergy> primaryEnergy_ptr) {
	//	std::cout << "FUNC-ID:  Region::connectGlobalReservoirs() of type " << output_type << " in " << code() << std::endl;
	for (auto& it : converter_ptrs_) {
		if (it.second->is_multi_converter()){
			//std::cout << "\t***changing co2ptr for " << it.first << std::endl;
			for (auto& out : it.second->output_types()) {
			        //if (primary_energy_ptrs_.find(out) != primary_energy_ptrs_.end()) {
					if (output_type == out ){
						if (out == "CO2") {
							if (it.second->connectCo2Reservoir(primaryEnergy_ptr)) {
								//std::cout <<"\t"<< code() << ": global co2 ... connected to "<< it.first <<  std::endl;
                //TODO Problems in analysis
								//drop existing co2 reservoir ptr
                //								primary_energy_ptrs_.erase("CO2");
							} else {
								std::cerr << "ERROR in Region::connectGlobalReservoirs() - could not connect CO2-reservoir(): "
										  << it.second->input_type() << " in region " << code() << std::endl;
								std::terminate();
							}
						} else if (out == "RECO2"){
						  if (it.second->connectCo2Reservoir(primaryEnergy_ptr)) {
						    std::cout <<"\t"<< code() << ": global renewable co2 ... connected to "<< it.first <<  std::endl;
                //TODO Problems in analysis
                //drop existing co2 reservoir ptr
                //                primary_energy_ptrs_.erase("CO2");
						  } else {
                std::cerr << "ERROR in Region::connectGlobalReservoirs() - could not connect RECO2-reservoir(): "
                      << it.second->input_type() << " in region " << code() << std::endl;
                std::terminate();
            }


						} else {
							std::cerr << "Region::connectGlobalReservoirs not defined for other than co2 yet!" << std::endl;
							std::terminate();
						}

					}
			}
		}
	}

}

void Region::connectReservoirs() {
  //  std::cout << "FUNC-ID:  Region::connectReservoirs() in " << code() << std::endl;
  if (!converter_ptrs_.empty()){
	  for (auto&conv : converter_ptrs_){
		//std::cout << conv.first << " connecting reservoirs..."<< std::endl;
		auto cat = conv.second->get_HSMCategory();
		if (conv.second->is_multi_converter()){
		  connect_multi_converter(conv);
		  //std::cout << "//////////m-u-l-t-i-c-o-n-v-e-r-t-e-r////////////" <<std::endl;
		} else {
		  //not multi-conv
		  if((cat == dm_hsm::HSMCategory::CONV_MUSTRUN) ||
			 (cat == dm_hsm::HSMCategory::LINE_CONVERTER) || (cat == dm_hsm::HSMCategory::UNAVAILABLE)) {
				  //do nothing   std::cout << "\t no connection necessary for this converter type" << std::endl;
	//        std::cout << "do nothing for " << conv.first << std::endl;
	//        std::cout << "//////////t-r-c-o-n-v-e-r-t-e-r////////////" <<std::endl;
		  } else {
			connect_converter(conv);
			//DEPRECATED manual_connect_conv(conv);
	//        std::cout << "//////////c-o-n-v-e-r-t-e-r////////////" <<std::endl;
		  }
		}
	   }
  }
  // check all storage and primary energy if connected
  if (!storage_ptrs_.empty())
	  for (auto& it : storage_ptrs_) {
		if (!it.second-> connected()){
		  std::cout << "at this point all converters should be connected to PrimaryEnergy and Storage" << std::endl;
		  std::cerr << "Parametrisation ERROR: " << it.second->code()<< " in " << code() << " missing at least one converter!" << std::endl;
		  std::terminate();
		}
	  }
  if(!primary_energy_ptrs_.empty())
	  for (auto& it : primary_energy_ptrs_) {
		if (!it.second-> connected()){
		  std::cout << "at this point all converters should be connected to PrimaryEnergy and Storage" << std::endl;
		  std::cout << "Parametrisation ERROR: " << it.second->code()<< " in " << code() << " missing at least one converter!" << std::endl;
		  std::cerr << "Parametrisation ERROR: " << it.second->code()<< " in " << code() << " missing at least one converter!" << std::endl;
		  std::terminate();
		} else {
			//DEBUG std::cout << it.second->code() << " is connected = " << it.second->connected() << std::endl;
		}
		//std::cout << "connected primary energy:" << it.second->code() << std::endl;
		//std::cout << "\t status " << it.second->connected() << std::endl;
	  }
}

void Region::connectLink(std::weak_ptr<Link> link,
                         const std::unordered_map<std::string,
                                                  std::shared_ptr<TransmissionConverter> >& converter_ptrs) {
  //DEBUG std::cout << "FUNC: Region::connectLink" << std::endl;
  for (const auto& it : converter_ptrs) {
    if (it.second->get_input() == "electric_energy") {
      line_map_el_.emplace(link.lock()->code() + "." + it.first, std::make_pair(std::move(link.lock()), it.second));
    } else if (it.second->get_input() == "methane") {
      //std::cout << "connecting methane - link: "<< link.lock()->code() << std::endl;
      line_map_ch4_.emplace(link.lock()->code() + "." + it.first, std::make_pair(std::move(link.lock()), it.second));
    }
  }
}

void Region::calculateResidualLoad(dm_hsm::HSMCategory category,
                                   aux::SimulationClock::time_point tp_start_seq,
                                   aux::SimulationClock::time_point tp_end_seq,
                                   aux::SimulationClock::duration tick_length) {
  //calculate RL based on generation TS (only dm_hsm::HSMCategory::RE_GENERATION at the moment)
  //DEBUG  std::cout << "FUNC-ID: Region::calculateResidualLoad for renewable timeseries "<< std::endl;
  if (category != dm_hsm::HSMCategory::RE_GENERATOR) {
    std::cerr << "***WARNING: using calculateResidualLoad for not time - series based calculation"
                 " - not defined case yet!" << std::endl;
    std::cerr << "ERROR in Region::calculateResidualLoad - undefined case!" << std::endl;
    std::terminate();
  }
  if (!RL_init_) {
    //simulationclock to iterate along timeseries
    aux::SimulationClock sub_sim_clock(tp_start_seq, tick_length);//tp_start_seq
    std::vector<double> residual_load;
    do {
      ///Calculate residualLoad(RL) from the load and subtract the converter power of the activated category.
      double generation_tp = 0;
      if (!converter_ptrs_.empty()){
		  for (auto& it : converter_ptrs_) {
			if (it.second->get_HSMCategory() == category) {
			  generation_tp += it.second->generation_fromPotential(sub_sim_clock);//generation = capcity * ts_generation_potential
			}
		  }
		  // std::cout << "Demand: " << demand_electric(sub_sim_clock.now())  << " GW generation " << generation_tp << " GW " << std::endl;
		  residual_load.push_back(demand_electric(sub_sim_clock.now()) - generation_tp);
      }
    // DEBUG
	//       std::cout << "tick: " << aux::SimulationClock::time_point_to_string(sub_sim_clock.now())
	//       	   	   	 << "generation = " << generation_tp << " and RL = "
	//				 << residual_load.back() << std::endl;
    //*/
    } while (sub_sim_clock.tick() < tp_end_seq);
    residual_load.push_back(0.0);//end of the timeSeries=const=0
    residual_load_ += aux::TimeSeriesConst(residual_load, tp_start_seq, tick_length);
    RL_init_ = true;
  } else {
    std::cerr << "ERROR: ResidualLoad was already calculated initially." << std::endl;
    std::terminate();
  }
}

void Region::init_heat_load(aux::SimulationClock::time_point tp_start_seq,
                            aux::SimulationClock::time_point tp_end_seq,
                            aux::SimulationClock::duration tick_length) {
  //std::cout << "FUNC-ID: Region::init_heat_load\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  if(module_heat_active()){
    //simulationclock to iterate along timeseries
    aux::SimulationClock sub_sim_clock(tp_start_seq, tick_length);//tp_start_seq
    std::vector<double> residual_heat_load;
    do {
         residual_heat_load.push_back(demand_heat(sub_sim_clock.now()));
        //         std::cout << "tick: " << aux::SimulationClock::time_point_to_string(sub_sim_clock.now())
        //                   << " and heat_load = " << residual_heat_load.back() << " GW_th in region " << code() << std::endl;

   } while (sub_sim_clock.tick() < tp_end_seq);
       residual_heat_load.push_back(0.0);//end of the timeSeries=const=0
       residual_heat_ += aux::TimeSeriesConst(residual_heat_load, tp_start_seq, tick_length);
  //std::cout << "end Region::init_heat_load in region " << code() << std::endl;
  }
  //  std::cout << "region " << code() << " heat= ? " << module_heat_active() << std::endl;
  //  std::cout << "REACHED END OF FUNC-ID: Region::init_heat_load\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
}

void Region::resetSequencedRegion(const aux::SimulationClock& clock) {
  //std::cout << "FUNC-ID: Region::resetSequencedRegion"<< std::endl;
  //do not reset all variables!

  //reset all converter
  if (!converter_ptrs_.empty()) {
    for (auto &it : converter_ptrs_) {
      it.second->ResetSequencedConverter();
    }
  }
  if (!storage_ptrs_.empty()) {
    //reset all storages
    for (auto &it : storage_ptrs_) {
       it.second->ResetSequencedStorage(clock);
    }
  }
  if(!primary_energy_ptrs_.empty()) {
    for (auto &it : primary_energy_ptrs_) {
      it.second->resetSequencedPrimaryEnergy(clock);
    }
  }
   //reset variables in region with hourly values
   RL_init_ = false;
   supply_region_ = false;
   residual_load_TP_ = std::numeric_limits<double>::infinity();
   if(genesys::ProgramSettings::modules().find("heat")->second){
     residual_heat_TP_ = std::numeric_limits<double>::infinity();
   }else{
     residual_heat_TP_ = 0;
   }
   //RESET TSCA Members to dummies
   residual_load_ = aux::TimeSeriesConstAddable();
   residual_heat_ = aux::TimeSeriesConstAddable();
   remaining_residual_load_ = aux::TimeSeriesConstAddable();
   remaining_residual_heat_load_ = aux::TimeSeriesConstAddable();
   co2_emissions_ = aux::TimeSeriesConstAddable();
   imported_energy_ = aux::TimeSeriesConstAddable();
   exported_electricity_ = aux::TimeSeriesConstAddable();
   exported_gas_ = aux::TimeSeriesConstAddable();
   imported_electricity_ = aux::TimeSeriesConstAddable();
   imported_gas_ = aux::TimeSeriesConstAddable();
}

void Region::resetCurrentTP(const aux::SimulationClock& clock){
  if(!converter_ptrs_.empty()){
    for( auto&&it : converter_ptrs_){
      it.second->resetCurrentTP(clock);
    }
  }
  if(!storage_ptrs_.empty()){
    for( auto&&it : storage_ptrs_){
      it.second->ResetStorageCurrentTP(clock);
    }
  }
  if(!primary_energy_ptrs_.empty()) {
    for (auto&& it : primary_energy_ptrs_) {
      it.second->resetCurrentTP(clock);
    }
  }
  //copy RL to local double
  residual_load_TP_ = residual_load_[clock];
  //std::cout << "residual load original: " << residual_load_TP_ << std::endl;
  if(module_heat_active())
     residual_heat_TP_ = residual_heat_[clock];
  //  if (residual_load_TP_ > max_demand_current_year_)
  //	  max_demand_current_year_ = residual_load_TP_;
  //  std::cout << aux::SimulationClock::time_point_to_string(clock.now())<< " | RL =" << residual_load_[clock] << " in Region " << code()<< std::endl;
  //std::this_thread::sleep_for(std::chrono::milliseconds(500));
  reserved_residual_load_tp_ = 0.;
  map_reserved_exports_tp_.clear();
  max_pwr_exchange_grid_tp_ = 0.;
}

void Region::set_annual_lookups(const aux::SimulationClock& clock){
	if(!primary_energy_ptrs_.empty()) {
	    for (auto& it : primary_energy_ptrs_) {
	      it.second->set_annual_lookups(clock);
	    }
    }
	if(!converter_ptrs_.empty()) {
		for(const auto& it : converter_ptrs_){
			it.second->set_annaul_lookups(clock);

		}
	}

	if(!storage_ptrs_.empty()) {
		for(const auto& it : storage_ptrs_){
			it.second->set_annaul_lookups(clock);

		}
	}
	//	if(max_demand_current_year_ > genesys::ProgramSettings::approx_epsilon()){// do not execute in first year!
	//		annual_max_demand_ += aux::TimeSeriesConst(std::vector<double>{max_demand_current_year_, 0.0},
	//		                                                                      clock.now()-aux::years(1),
	//																			  aux::years(1));
	//		std::cout << aux::SimulationClock::time_point_to_string(clock.now())<< "Region " << code() << " | max demand last year = " << max_demand_current_year_ << " GW" << std::endl;
	//	}
	//	max_demand_current_year_ = 0.;

}

void Region::set_annual_unsupplied(double unsupplied_el_gwh, aux::SimulationClock::time_point tp){
  annual_unsupplied_electricity_ += aux::TimeSeriesConst(std::vector<double>{unsupplied_el_gwh,0.0},
                                                   tp,
                                                   aux::years(1));
}

void Region::decommission_plants(){
	std::cout << "***WARNING Decommissioning not tested!" << std::endl;
  if(!converter_ptrs_.empty()){
    for(const auto& it : converter_ptrs_){
      if (it.second->get_HSMCategory() == HSMCategory::DISPATCHABLE_GENERATOR )
      it.second->check_for_decommission();
    }
  }

}

void Region::uncheck_active_current_year(){
	if(!converter_ptrs_.empty()){
	    for(const auto&it : converter_ptrs_){
	      if (it.second->get_HSMCategory() != HSMCategory::UNAVAILABLE ){
	    	  it.second->uncheck_active_current_year();
	      }
	    }
	}
	if(!storage_ptrs_.empty()){
	    for(const auto&it : storage_ptrs_){
	     it.second->uncheck_active_current_year();
	    }
	}
}

void Region::add_OaM_cost(aux::SimulationClock::time_point tp_now) {
  //DEBUG std::cout << "add OaM for active component " << it.first << " tp: " << aux::SimulationClock::time_point_to_string((clock.now()-aux::years(1))) << std::endl;
  //std::cout << "add OaM for active component " << it.first << " tp: " << aux::SimulationClock::time_point_to_string((clock.now()-aux::years(1))) << std::endl;
	if (!converter_ptrs_.empty()) {
		for (const auto&it : converter_ptrs_) {
		  it.second->add_OaM_cost(tp_now);
		  //TODO DECOMMISSIONING - use flag active_current_year to destinguish! ***CODE NOT FINISHED!
		  //		  if (it.second->active_current_year()) {
      //		    std::cout << aux::SimulationClock::time_point_to_string(tp_now) << " = active_current_year : "<< it.second->code()  << " | in " <<code()<< std::endl;
      //		    it.second->add_OaM_cost(tp_now);
      //		  } else if (it.second->get_HSMCategory() == HSMCategory::RE_GENERATOR) {
      //		    std::cout << aux::SimulationClock::time_point_to_string(tp_now) << " = RE -active_current_year : "<< it.second->code() << std::endl;
      //		    it.second->add_OaM_cost(tp_now);//in any cases because operation flag is not relevant
      //		  } else if (it.second->get_HSMCategory() != HSMCategory::UNAVAILABLE ) {
      //			  std::cout << aux::SimulationClock::time_point_to_string(tp_now) << " = !active copde : "<< it.second->code()  << " active=" << it.second->active_current_year()<< std::endl;
      //			  it.second->add_OaM_cost(tp_now);
      //			} else {
      //			  std::cerr << "ERROR in Region::add_OaM_cost - unidentfied converter found!" << std::endl;
      //			  std::terminate();
      //        // it.second->add_OaM_cost_zero(tp_now);//to avoid segmentation fault because of uninitialised tbd
      //      }
		}
  }
	if (!storage_ptrs_.empty()) {
		for (const auto&it : storage_ptrs_) {
		  it.second->add_OaM_cost(tp_now);
		  //TODO DECOMMISSIONING - use flag active_current_year to destinguish!  ***CODE NOT FINISHED!
      //			if (it.second->active_current_year()) {
      //				it.second->add_OaM_cost(tp_now);
      //			} else {
      //		    	it.second->add_OaM_cost_zero(tp_now);//to avoid segmentation fault because of uninitialised tbd
      //		  }
		}
	}
}

void Region::activate_mustrun(const aux::SimulationClock& clock) {
  for (auto&& it : converter_ptrs_) {
    auto hsm_cat = it.second->get_HSMCategory();
    if (hsm_cat == dm_hsm::HSMCategory::CONV_MUSTRUN) {
        std::cout << "Region::activate_mustrun - not implemented for CONV_MUSTRUN" << std::endl;
        //it.second->useConverterOutputMustrun(clock);
        //      double generation = it.second->getCapacity(sim_clock);
        //      if (generation > 0){
        //    	  if (it.second->useCapacity(generation, sim_clock)){
        //    		  residual_load_TP_ -= generation;
        //    	  }
        //      }

      //DEBUG std::cout << aux::SimulationClock::time_point_to_string(sim_clock.now())<< "debug: activate_mustrun -> residual_load_currentTP_ = " << residual_load_currentTP_ << std::endl;
    } else {
      // do nothing
      //std::cout << "debug: activate_mustrun() - no CONV_MUSTRUN found" << std::endl;
    }
  }
}

// XXX1
void Region::generalised_balance_local(const dm_hsm::HSMCategory& category,
                                       const aux::SimulationClock& clock){
  /* Heat Integrationsversuch: kja
  //check:
    // a) is heat category && b) is heat active in region?
  if (module_heat_active()) {
    // std::cout << "heat active in region" << code() << std::endl;
    if (residual_heat_TP_ > genesys::ProgramSettings::approx_epsilon()){
      // std::cout << " need to balance residual heat of " << residual_heat_TP_ << " GW_th" << std::endl;
      if (category == HSMCategory::HEAT_STORAGE){
          std::cout << "heat active in region" << code()  << " balancing via storage" << std::endl;
          // Kevin => warum nicht if in storage_discharging um auf richtige residual zuzugreifen???
          // discharge_heat_storage(HSMCategory::HEAT_STORAGE, clock);
          converter_balance(category, clock);
        }else if (category == HSMCategory::HEAT_GENERATOR || category == dm_hsm::HSMCategory::HEAT_AND_EL || category == HSMCategory::EL2HEAT){
		  // Kevin => residual heat wird doch eingelesenund ich region_prototype skaliert
		  //residual_heat_balance(clock);
		  converter_balance(category, clock);
        }
    }
  }
  */

  //DEBUG  std::cout << "FUNC-ID: Region::generalised_balance_local() in region "<< code() << std::endl;
  if (residual_load_TP_ > genesys::ProgramSettings::approx_epsilon()){
	  converter_balance(category, clock);
    /* Heat Integrationsversuch: kja
    if (category != HSMCategory::HEAT_STORAGE && category != HSMCategory::EL2HEAT_STORAGE && category != HSMCategory::EL2HEAT && category != HSMCategory::HEAT_GENERATOR && category != HSMCategory::HEAT_AND_EL) {
		  //use a converter to balance std::cout << aux::SimulationClock::time_point_to_string(clock.now()) << " | ++RL = " << residual_load_TP_ << " callconverter_balance()!" << std::endl;
		  converter_balance(category, clock);
	  }
    */
  } else if (residual_load_TP_ < -genesys::ProgramSettings::approx_epsilon()){
    //use a storage to absorbe negative RL std::cout << "--RL = " << residual_load_TP_ << " call storage_charging()!" << std::endl;
    if(category == HSMCategory::LT_STORAGE || category == HSMCategory::ST_STORAGE) {
      storage_charging(category, clock);
    }
    /* Heat Integrationsversuch: kja
    if ((category == HSMCategory::EL2HEAT_STORAGE) && (genesys::ProgramSettings::modules().find("heat")->second)) {
	     storage_charging(HSMCategory::HEAT_STORAGE, clock);
	    }
    */
  } else {
	  // This is the case of exact balance in the order of epsilon!
    residual_load_TP_ = 0.;
    //    std::cout << aux::SimulationClock::time_point_to_string(clock.now()) <<  " RL in " << code_ << " in balance: " << residual_load_currentTP_ << std::endl;
  }
}


void Region::balance_start(int hops,
                     const dm_hsm::HSMCategory& cat,
                     const aux::SimulationClock& clock) {
  if (residual_load_TP_> genesys::ProgramSettings::approx_epsilon()){ //R L> 0 equals unsatisfied demand
    if (max_pwr_exchange_grid_tp_ == 0. && genesys::ProgramSettings::grid_exchange_ratio() > 0.){//first balance via grid in tp (reset per tp)
      max_pwr_exchange_grid_tp_ = genesys::ProgramSettings::grid_exchange_ratio()*residual_load_TP_;
      //      std::cout << max_pwr_exchange_grid_tp_ << " = max_pwr_exchange grid" << std::endl;
      //      std::cout << genesys::ProgramSettings::grid_exchange_ratio() << " = grid_exchange_ratio grid" << std::endl;
      //      std::cout << residual_load_TP_ << " = residual_load_TP_ grid" << std::endl;
    }
      //std::cout << "Region::balance_start===========in region " << code() << "| hops depth = " << hops << "| RL = " << residual_load_TP_  << " GW"<< std::endl;
    if (max_pwr_exchange_grid_tp_ > 0.){
      auto import = balance(hops, std::min(max_pwr_exchange_grid_tp_, residual_load_TP_), cat, clock);
      residual_load_TP_ -= import;
      if(import > 0)
        import_for_local_balance_ += aux::TimeSeriesConst(std::vector<double>{import, 0.0},
                                  clock.now(),
                                  clock.tick_length() );
	  }
  } else {
    //DEBUG    std::cout << code() << " DEBUG: RL < 0 no balance necessary!" << std::endl;
  }
}

void Region::setTransferStoredEnergy(const aux::SimulationClock& clock) {
  for (auto &it : storage_ptrs_) {
    it.second->setTransferStoredEnergy(clock);
  }
}

double Region::getDiscountedValue(std::string query,
                                  aux::SimulationClock::time_point start,
                                  aux::SimulationClock::time_point end,
                                  const aux::SimulationClock::time_point discount_present) const{
  aux::SimulationClock::time_point present;
    if (discount_present == aux::SimulationClock::time_point_from_string("1970-01-01_00:00")){
      present = start;
    } else {
      present = discount_present;
    }
    double discounted_value = 0.0;
    if (query == "ENERGY") {
      auto energy_data = calc_energy_vector(start, end);
      discounted_value += aux::DiscountFuture2(aux::TimeSeriesConst(std::get<0>(energy_data),
                                                                    std::get<1>(energy_data),
                                                                    aux::years(1)),
                                               start,
                                               end,
                                               aux::years(1),
                                               genesys::ProgramSettings::interest_rate(),
                                               present);
    } else if ( query == "CAPEX" || query == "VOPEX" || query == "FOPEX") {
        if (!converter_ptrs_.empty()) {
          for (const auto& it : converter_ptrs_) {
            discounted_value += it.second->getDiscountedValue(query, start, end, present);
          }
        }
        if (!storage_ptrs_.empty()) {
          for (const auto& it : storage_ptrs_) {
            discounted_value += it.second->getDiscountedValue(query, start, end, present);
          }
        }
    } else {
        std::cerr << "ERROR in Region::getDiscountedValue - query " << query << " could not be identified!" << std::endl;
        std::terminate();
    }
      return (discounted_value);


}

double Region::get_sum(std::string query,
                         aux::SimulationClock::time_point start,
                         aux::SimulationClock::time_point end) const {
  double sum_value = 0.0;
  if (query == "ENERGY") {
        auto energy_data = calc_energy_vector(start, end);
        sum_value += aux::sum(aux::TimeSeriesConst(std::get<0>(energy_data),
                                                                      std::get<1>(energy_data),
                                                                      aux::years(1)),
                                    start,
                                    end,
                                    aux::years(1));
  //Deprecated
  //  } else if (query == "unsupplied_load") {
  //	  sum_value += aux::sum_positiveValues(remaining_residual_load_, start, end, genesys::ProgramSettings::simulation_step_length());
  //	    //std::cout << "region: " << code() << " | " << sum_value << " GWh" << std::endl;

  } else  if ( query == "CAPEX" || query == "VOPEX" || query == "FOPEX") {
        if (!converter_ptrs_.empty()) {
          for (const auto& it : converter_ptrs_) {
            sum_value += it.second->get_sum(query, start, end);
          }
        }
        if (!storage_ptrs_.empty()) {
          for (const auto& it : storage_ptrs_) {
            sum_value += it.second->get_sum(query, start, end);
          }
        }
  } else {
        std::cerr << "ERROR in Region::get_sum - query " << query << " could not be identified!" << std::endl;
        std::terminate();
  }
  return (sum_value);
}

void Region::calculated_annual_disc_capex(aux::SimulationClock::time_point start) {
  //std::cout << "FUNC-ID: Region::calculated_annual_capex" << std::endl;
  if (!converter_ptrs_.empty()) {
    for (const auto &it : converter_ptrs_) {
      it.second->calculated_annual_disc_capex(start);
    }
  }

  if (!storage_ptrs_.empty()) {
      for (const auto&it : storage_ptrs_) {
        it.second->calculated_annual_disc_capex(start);
      }
  }
}

double Region::getExportCapacity(const dm_hsm::HSMCategory cat,
                                 const aux::SimulationClock& clock,
                                 const double exportRequest) {
  //  std::cout << "\t\t" << code() << "| Region::getExportCapacity - requestedExport= " << exportRequest << " GW" << std::endl;
  double export_cap = 0.;

  if (cat == dm_hsm::HSMCategory::RE_GENERATOR) {
    if (residual_load_TP_ < 0.){ // RL<0 = generation exceeeds demand, export possible
      reserved_residual_load_tp_ = export_cap = std::min(std::abs(residual_load_TP_), exportRequest);
      //      std::cout << "\t\tDEBUG: Export= "<< export_cap << " | RL= " << residual_load_TP_ << " | TR-Limit= " << exportRequest << std::endl;
      //      std::cout << "\t\tDEBUG: RL-Reserved= " << reserved_residual_load_tp_ << std::endl;
    }
  } else {
    std::multimap<double, std::weak_ptr<dm_hsm::Converter>, std::greater<double> > efficient_converters = collectConverter(cat,
                                                                                                                           clock);
    double remaining_request = exportRequest;
    //    std::cout << "\tDEBUG: remaining_request= " << remaining_request << " | init"<< std::endl;
    for (auto &it : efficient_converters) { //do not use it.first, for sorting purposes only!
      if (remaining_request > genesys::ProgramSettings::approx_epsilon()){
      auto storage_type = it.second.lock()->get_HSMSubCategory();
        if ((storage_type == dm_hsm::HSMSubCategory::BICHARGER) || (storage_type == dm_hsm::HSMSubCategory::DISCHARGER)) {
          //check usable_power != usable capacity to consider the storage/primary energy source
          auto power_output = it.second.lock()->reserveDischarger(clock, exportRequest);
          if (power_output > genesys::ProgramSettings::approx_epsilon()){
            //          std::cout << "\tmap_reserved_export inserting: code: " << it.second.lock()->code() << " with capacity: " << power_output << " | of total " << it.second.lock()->usable_capacity_out_tp() << std::endl;
            map_reserved_exports_tp_.emplace(power_output, it.second.lock());
            //std::cout << "\tmap_reserved_converter contains " << map_reserved_exports_tp_.size() << " elements!" << std::endl;
            remaining_request -= power_output;
          } //DEBUG else if (power_output != 0.){ std::cout << "***\t\tpower out small = " << power_output << std::endl;}
        }// else is invalid converter = charger //TODO export of conventional generation is not supported yet!
      }
    }
    export_cap = (exportRequest - remaining_request);
    //std::cout << "\t\tregion " << code() << " has export cap of " << export_cap << std::endl;
  }
  return export_cap;
}

bool Region::useExportCapacity(const dm_hsm::HSMCategory cat,
                                   const double export_request,
                                   const aux::SimulationClock& clock,
                                   const std::weak_ptr<Link>&) {
  //std::cout << "FUNC-ID: Region::useExportCapacity with request = " << export_request << std::endl;
  bool request_ok = false;
  if (cat == dm_hsm::HSMCategory::RE_GENERATOR) {  //std::cout << "\tRE Generator: RL-Balancing" << std::endl;
    if (!(reserved_residual_load_tp_ < export_request ) || !(export_request > residual_load_TP_)) {
      residual_load_TP_ -= export_request;
      exported_electricity_ += aux::TimeSeriesConst(std::vector<double>{export_request,0.0},
                                                    clock.now(),
                                                    clock.tick_length());
      reserved_residual_load_tp_ = 0.;
      request_ok = true;
    }
    return (request_ok);
  } else { // std::cout << "\tother Generator: Converter-Balancing | "  << map_reserved_exports_tp_.size()
           //<< " elements in map " << std::endl;
    if (!(export_request < 0.)) {
      double remaining_request = export_request;
        for (auto& it : map_reserved_exports_tp_) {
            double current_request = std::min(it.first, remaining_request);
            /* Heat Integrationsversuch: kja
            if (it.second.lock()->useConverterOutput(clock, remaining_request, "electric_energy")) {
            */
            if (it.second.lock()->useConverterOutput(clock, remaining_request)) {
              remaining_request -=current_request;
            } else {
              std::cout <<aux::SimulationClock::time_point_to_string(clock.now())<<
                          " => Region::useExportCapacity: failed in " << code() << " for " << it.second.lock()->code() << std::endl;
            }
            map_reserved_exports_tp_.erase(it.first);//remove used item from map!
        } //all entries of map iterated. map shoudl have size 0 now
        //        std::cout << "\t"<< map_reserved_exports_tp_.size() << " elements in map " << std::endl;
      if (remaining_request < genesys::ProgramSettings::approx_epsilon()) { //std::cout << "Remaining request from " << code( ) << " zero GW" << std::endl;
        request_ok = true;
      }
      return request_ok;
    } else {
      std::cerr << aux::SimulationClock::time_point_to_string(clock.now()) <<
                   " Region::useExportTransmission export_request <0: " << export_request<< std::endl;
      std::terminate();
    }
    return false;
  }
}


void Region::print_current_capacities(const aux::SimulationClock& clock){
  std::cout << code() << "\t\t | \t" << residual_load_TP_ << "\t residual_load_currentTP_"
      << std::endl;
  if(!converter_ptrs_.empty()){
    for (auto &it : converter_ptrs_) {
      it.second->print_current_capacities(clock);
    }
  }
  if(!storage_ptrs_.empty()){
    for (auto &it : storage_ptrs_) {
      it.second->print_current_capacities(clock);
    }
  }
  if(!primary_energy_ptrs_.empty()){
	  for (auto &it : primary_energy_ptrs_) {
		  it.second->print_current_capacities(clock);
	  }
  }
  std::cout << "----------------------------------------------------" << std::endl;
  //primary_energy_ptrs_
}


//========================================Private Functions============================================================
template <typename T>
std::shared_ptr<T> Region::get_pointer_to(const std::unordered_map<std::string, std::shared_ptr<T> >& ptr_map,
                                          const std::string& query) {
  auto pos = ptr_map.find(query);
  if (pos != ptr_map.end()) {
    return pos->second;
  } else {
    //return empty ptr for the calling function to specify which ptr returned empty!
    //std::cerr << "ERROR in dm_hsm::Region::get_pointer_to:  Returning empty pointer for " << query << std::endl;
    //std::terminate();
    return std::shared_ptr<T>();
  }
}


std::multimap<double, std::weak_ptr<dm_hsm::Converter>, std::greater<double> >
                                                        Region::collectConverter(dm_hsm::HSMCategory cat,
                                                                                 const aux::SimulationClock& clock) {
  // std::cout << "\tFUNC-ID: Region::collectConverter with  HSMCategory: " << static_cast<std::underlying_type<dm_hsm::HSMCategory>::type>(cat) << std::endl;
  std::multimap<double, std::weak_ptr<dm_hsm::Converter>, std::greater<double> > tmp_map_converter; //map with descending sorting

  //Working principle in pseudocode:
  //if cat =  ST_STORAGE, LT_STORAGE, LINE_CONVERTER
      //=> sort by std::greater<efficiency>
  //else if cat = DISPATCHABLE_GENERATOR
      //=> sort by std::less<vopex> == std::greater<1/vopex>
  //else if cat = HEAT
    //=> sort by efficiency merit order
  /* Heat Integrationsversuch: kja
  if (cat == HSMCategory::LT_STORAGE || cat == HSMCategory::ST_STORAGE || cat == HSMCategory::LINE_CONVERTER || (cat == HSMCategory::HEAT_STORAGE && (genesys::ProgramSettings::modules().find("heat")->second)) || (cat == HSMCategory::EL2HEAT && (genesys::ProgramSettings::modules().find("heat")->second))) {// sort by std::greater<efficiency>
  */
  if (cat == HSMCategory::LT_STORAGE || cat == HSMCategory::ST_STORAGE || cat == HSMCategory::LINE_CONVERTER) {// sort by std::greater<efficiency>
    for (const auto& it : converter_ptrs_) {
      //select converter according category:
      //std::cout << "==" << aux::SimulationClock::time_point_to_string(clock.now()) << "== storage search: " << it.second->code() << std::endl;
      if (it.second->usable_capacity_el(clock) > genesys::ProgramSettings::approx_epsilon()) { //ignore converter with small capacity!
        if (cat == it.second->get_HSMCategory()) { //map is sorted by conv_efficiency
          double conv_efficiency = it.second->get_efficiency(clock); //get_efficiency>0 implies capacity>0!
          if ((genesys::ProgramSettings::approx_epsilon() <= conv_efficiency) &&
              /* Heat Integrationsversuch: kja
              ((conv_efficiency <= 1 + genesys::ProgramSettings::approx_epsilon()) || (cat == HSMCategory::EL2HEAT)))
              */
              ((conv_efficiency <= 1 + genesys::ProgramSettings::approx_epsilon()))) {
              tmp_map_converter.emplace(conv_efficiency, it.second);
               //} else if (efficiency_max_power == 0) { //do not fill into map
          } else {
                 std::cerr << "ERROR in Region::collectEfficicentConverter: Efficiency_max corrupted for converter "
                           << it.first << std::endl;
                 std::terminate();
          }
        }//get_HSMCat;l
      }//usable_capacity>0
    }//for
  /* Heat Integrationsversuch: kja
  } else if (cat == HSMCategory::DISPATCHABLE_GENERATOR || (cat == HSMCategory::HEAT_GENERATOR && (genesys::ProgramSettings::modules().find("heat")->second)) || (cat == HSMCategory::HEAT_AND_EL && (genesys::ProgramSettings::modules().find("heat")->second))) {//=> sort by std::less<vopex> == std::greater<1/vopex>
  */
  } else if (cat == HSMCategory::DISPATCHABLE_GENERATOR) {//=> sort by std::less<vopex> == std::greater<1/vopex>
    for (const auto& it : converter_ptrs_) {

      //std::cout << "==" << aux::SimulationClock::time_point_to_string(clock.now()) << "== converter search: " << it.second->code() << std::endl;
      //select converter according category:
      if (it.second->usable_capacity_out_tp() > genesys::ProgramSettings::approx_epsilon()) { //ignore converter with small capacity!
        if (cat == it.second->get_HSMCategory()) { //check no CONV_MUSTRUN is selected
        	//CBU: the following lines are obsolete... if usable_capacity > 0
        	//double conv_efficiency = it.second->get_efficiency(clock); //implies capacity>0 Conversion from primary energy to electricity
        	//// This procedure only works with converter efficiency > 0 and < 1
        	//if (genesys::ProgramSettings::approx_epsilon() < conv_efficiency && conv_efficiency < 1+ genesys::ProgramSettings::approx_epsilon()){
            double specf_vopex = it.second->get_spec_vopex(clock);//specific variable==emission dependent opex
            if(specf_vopex > genesys::ProgramSettings::approx_epsilon() ) {
              //std::cout << "\t economic_converter_map: inserting converter of code: " << it.second->code()  << " | specf_vopex = " << specf_vopex << " | eff= " << 1/specf_vopex << " | capacity = " << it.second->usable_capacity_el(clock) << std::endl;
              tmp_map_converter.emplace(1/specf_vopex, it.second);
            }// else skip converter!
          //}// 0 < eff < 1
        }//cat
      }//usable_cap>0
    }//for
  } else {
    std::cerr << "ERROR in Region::collectConverter - not defined for this HSM Category" << std::endl;
    std::terminate();
  }
  return tmp_map_converter;
}

void Region::converter_balance(const dm_hsm::HSMCategory& category,
                               const aux::SimulationClock& clock) {
	//std::cout << "FUNC-ID: Region::converter_balance in region " << code() << " |\t residual_load= " << residual_load_TP_ << " GW"<< std::endl;
  std::multimap<double, std::weak_ptr<dm_hsm::Converter>, std::greater<double> > tmp_map_converter = collectConverter(category, clock);
  //loop all converter in region
  for (auto &it : tmp_map_converter) { //do not use it.first, for sorting purposes only!
	  //DEBUG std::cout << "==" << aux::SimulationClock::time_point_to_string(clock.now()) << "== converter active: " << it.second.lock()->code() << std::endl;

	///////////////////////////// Load
	double remaining_request_load = 0.;
	//check RL
	if (residual_load_TP_ > genesys::ProgramSettings::approx_epsilon()) {
		remaining_request_load = residual_load_TP_;
	//std::cout << "==" << aux::SimulationClock::time_point_to_string(clock.now()) << "== |==" << code()<< "== remaining request load = " << remaining_request_load << std::endl;
	} else {
		residual_load_TP_ = 0.;
	break;
	}

  /* Heat Integrationsversuch: kja
  if(genesys::ProgramSettings::modules().find("heat")->second){
  	//////////////////////////// Heat
  	double remaining_request_heat = 0.;
  	if (residual_heat_TP_ < genesys::ProgramSettings::approx_epsilon()) {
  		residual_heat_TP_ = 0.;
  		break;
  	// std::cout << "==" << aux::SimulationClock::time_point_to_string(clock.now()) << "== |==" << code()<< "== remaining request heat = " << remaining_request << std::endl;
  	} else {
  		remaining_request_heat = residual_heat_TP_;

		if (category == dm_hsm::HSMCategory::HEAT_GENERATOR || category == dm_hsm::HSMCategory::HEAT_STORAGE || category == dm_hsm::HSMCategory::HEAT_AND_EL || category == dm_hsm::HSMCategory::EL2HEAT) {

			//check if !storage charger
			auto hsm_subcat = it.second.lock()->get_HSMSubCategory();

			if (hsm_subcat != dm_hsm::HSMSubCategory::CHARGER) {//=discharger, bicharger, dispatchable generator
			  //DEBUG  std::cout << "==" << aux::SimulationClock::time_point_to_string(clock.now()) << "== converter active: " << it.second.lock()->code() <<  "|power= "<< it.second.lock()->usable_capacity_out_tp() <<  " remaining request = " << remaining_request << std::endl;
				double reserved_pwr_from_converter_heat = 0.;
				double reserved_pwr_from_converter_el = 0.;

			  if(it.second.lock()->get_HSMCategory() != dm_hsm::HSMCategory::EL2HEAT && it.second.lock()->get_HSMCategory() != dm_hsm::HSMCategory::HEAT_AND_EL){
				  //case A: Dispachtable Converter  or conventional converter in must-run mode
				  if(it.second.lock()->get_HSMCategory() == dm_hsm::HSMCategory::HEAT_GENERATOR){
					//TODO check for correctly applied efficiencies
					  reserved_pwr_from_converter_heat = it.second.lock()->usable_power_out_tp(clock, remaining_request_heat, "heat");//usable_power...overloaded for multi-conv!
					//case B: Storage Discharger / BiCharger
				  }
				  if(it.second.lock()->get_HSMCategory() == dm_hsm::HSMCategory::HEAT_STORAGE){
					  // Kevin => nimmt doch alle Speicher und dürfte doch nur Heat nehmen ???
					reserved_pwr_from_converter_heat = it.second.lock()->reserveDischarger(clock, remaining_request_heat);//usable_power...overloaded for multi-conv!
				  }

				  if (reserved_pwr_from_converter_heat > genesys::ProgramSettings::approx_epsilon() ) {
					  double balanceable_pwr_heat = std::min(reserved_pwr_from_converter_heat, residual_heat_TP_);
						if (it.second.lock()->useConverterOutput_el2heat(clock, reserved_pwr_from_converter_heat, "heat")) {
							residual_heat_TP_ += -balanceable_pwr_heat;
						}
					}
			  }

			  ///////////////////////////////////////
			  if(it.second.lock()->get_HSMCategory() == dm_hsm::HSMCategory::EL2HEAT){
				// Collect HEAT_AND_EL
				double request_heat_el2heat = 0;
				double usable_capacity_heat_of_chp = 0;
				double usable_capacity_el_of_chp = 0;
				double lowest_efficiency_of_chp = 1;
				double lowest_conversion_of_chp_heat = 1;
				double lowest_conversion_of_chp_el = 1;
				std::multimap<double, std::weak_ptr<dm_hsm::Converter>, std::greater<double> > tmp_map_converter = collectConverter(dm_hsm::HSMCategory::HEAT_AND_EL, clock);
				for (auto &it2 : tmp_map_converter) {
					usable_capacity_heat_of_chp += it2.second.lock()->usable_power_out_tp(clock, std::numeric_limits<double>::infinity(), "heat");//usable_power...overloaded for multi-conv!
					usable_capacity_el_of_chp += (usable_capacity_heat_of_chp/it2.second.lock()->get_output_conversion("heat")) * it2.second.lock()->get_output_conversion("electric_energy");
					if (lowest_efficiency_of_chp > it2.second.lock()->efficiency(clock.now())){
						lowest_efficiency_of_chp = it2.second.lock()->efficiency(clock.now());
						lowest_conversion_of_chp_heat = it2.second.lock()->get_output_conversion("heat");
						lowest_conversion_of_chp_el = it2.second.lock()->get_output_conversion("electric_energy");
					}
				}

				// separate Hierarchie für Wärmepumpe und KWK
				if (remaining_request_load - usable_capacity_el_of_chp > genesys::ProgramSettings::approx_epsilon()){
					//if (remaining_request_heat - usable_capacity_heat < genesys::ProgramSettings::approx_epsilon() * -1){
					//  remain = usable_capacity_heat - remaining_request_heat;
					//	storage_charging(HSMCategory::HEAT_STORAGE, clock);
					//}
					if (remaining_request_heat - usable_capacity_heat_of_chp > genesys::ProgramSettings::approx_epsilon()){
						request_heat_el2heat = remaining_request_heat - usable_capacity_heat_of_chp;
					}
				}
				if (remaining_request_load - usable_capacity_el_of_chp < genesys::ProgramSettings::approx_epsilon() * -1){
					double remaining_request_load_copy = remaining_request_load;
					double usable_capacity_of_chp_el_temp = 0;
					double usable_capacity_heat_of_chp_to_satisfy_el_exact = 0;
					std::multimap<double, std::weak_ptr<dm_hsm::Converter>, std::greater<double> > tmp_map_converter = collectConverter(dm_hsm::HSMCategory::HEAT_AND_EL, clock);
					for (auto &it2 : tmp_map_converter) {
						usable_capacity_of_chp_el_temp = it2.second.lock()->usable_power_out_tp(clock, remaining_request_load_copy, "heat");//usable_power...overloaded for multi-conv!
						remaining_request_load_copy -= usable_capacity_of_chp_el_temp;
						usable_capacity_heat_of_chp_to_satisfy_el_exact += (usable_capacity_of_chp_el_temp/it2.second.lock()->get_output_conversion("electric_energy")) * it2.second.lock()->get_output_conversion("heat");
					}

					if (remaining_request_heat - usable_capacity_heat_of_chp_to_satisfy_el_exact > genesys::ProgramSettings::approx_epsilon()){
						double remaining_capacity_el_for_el_2_heat = usable_capacity_el_of_chp - remaining_request_load;

						request_heat_el2heat = (remaining_request_heat - usable_capacity_heat_of_chp_to_satisfy_el_exact)/(1 + (lowest_conversion_of_chp_heat)/(lowest_conversion_of_chp_el*lowest_efficiency_of_chp));

						if ((request_heat_el2heat/lowest_conversion_of_chp_heat) * lowest_conversion_of_chp_el > remaining_capacity_el_for_el_2_heat){
							request_heat_el2heat = remaining_request_heat - ((remaining_capacity_el_for_el_2_heat/lowest_conversion_of_chp_el) * lowest_conversion_of_chp_heat);
						}
					}
					//if (remaining_request_heat - usable_capacity_heat_satisfy_el_exact < genesys::ProgramSettings::approx_epsilon() * -1){
					//	storage_charging(HSMCategory::HEAT_STORAGE, clock);
					//}
				}

				//TODO check for correctly applied efficiencies
				reserved_pwr_from_converter_heat = it.second.lock()->usable_power_out_tp_el2heat(clock, request_heat_el2heat);//usable_power...overloaded for multi-conv!
				reserved_pwr_from_converter_el = reserved_pwr_from_converter_heat / it.second.lock()->efficiency(clock.now());
				//case B: Storage Discharger / BiCharger

				//std::multimap<double, std::weak_ptr<dm_hsm::Converter>, std::greater<double> > tmp_map_converter = collectConverter(dm_hsm::HSMCategory::HEAT_AND_EL, clock);
				  //loop all converter in region
				//auto balanceable_pwr_heat =
				//for (auto &it : tmp_map_converter) { //do not use it.first, for sorting purposes only!

				if (reserved_pwr_from_converter_heat > genesys::ProgramSettings::approx_epsilon() ) {
					auto balanceable_pwr_heat = std::min(reserved_pwr_from_converter_heat, residual_heat_TP_);
					auto balanceable_pwr_el = std::max(reserved_pwr_from_converter_el,0.);
					if (it.second.lock()->useConverterOutput_el2heat(clock, reserved_pwr_from_converter_heat, "heat")) {
						residual_heat_TP_ += -balanceable_pwr_heat;
						residual_load_TP_ += balanceable_pwr_el;
					}
				}
		  }

			  if(it.second.lock()->get_HSMCategory() == dm_hsm::HSMCategory::HEAT_AND_EL){
				  reserved_pwr_from_converter_heat = it.second.lock()->usable_power_out_tp(clock, remaining_request_heat, "heat");//usable_power...overloaded for multi-conv!
				  reserved_pwr_from_converter_el = (reserved_pwr_from_converter_heat/it.second.lock()->get_output_conversion("heat")) * it.second.lock()->get_output_conversion("electric_energy");
				  if (reserved_pwr_from_converter_heat > genesys::ProgramSettings::approx_epsilon() ) {
					//std::cout << "===== converter active = " << it.second.lock()->code() << " with output power " << power_from_converter << std::endl;
					auto balanceable_pwr_heat = std::min(reserved_pwr_from_converter_heat, residual_heat_TP_);
					auto balanceable_pwr_el = (balanceable_pwr_heat/it.second.lock()->get_output_conversion("heat")) * it.second.lock()->get_output_conversion("electric_energy");
					if (it.second.lock()->useConverterOutput(clock, reserved_pwr_from_converter_heat, "heat")) {
						residual_heat_TP_ += -balanceable_pwr_heat;
						residual_load_TP_ += -balanceable_pwr_el;
						// residual_heat_TP_ += balanceable_pwr; Kevin für Strom bei anderem
					  //std::cout << "converter_balance() of " << balanceable_pwr << "GW succeeded in "<< code() << " with " << it.second.lock()->code()<< std::endl;
					} else {
					  std::cerr << "Region::converter_balance - balance with reserved power failed to useConverterOutput()" << std::endl;
					  std::terminate();
					}
				  } else {
					//std::cout << "converter output too small" << std::endl;
				  }
			  }
			}
		}
	}
    } else {
  */

		//check if !storage charger

		//check if !storage charger
		auto hsm_subcat = it.second.lock()->get_HSMSubCategory();

		if (hsm_subcat != dm_hsm::HSMSubCategory::CHARGER) {//=discharger, bicharger, dispatchable generator
		  //DEBUG  std::cout << "==" << aux::SimulationClock::time_point_to_string(clock.now()) << "== converter active: " << it.second.lock()->code() <<  "|power= "<< it.second.lock()->usable_capacity_out_tp() <<  " remaining request = " << remaining_request << std::endl;
		  auto reserved_pwr_from_converter = 0.;

		  //case A: Dispachtable Converter  or conventional converter in must-run mode
		  if(it.second.lock()->get_HSMCategory() == dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR
			  ||it.second.lock()->get_HSMCategory() == dm_hsm::HSMCategory::CONV_MUSTRUN ){
			//TODO check for correctly applied efficiencies
			/* Heat Integrationsversuch: kja
      	  	  reserved_pwr_from_converter = it.second.lock()->usable_power_out_tp(clock, remaining_request_load, "electric_energy");//usable_power...overloaded for multi-conv!
			 */
			reserved_pwr_from_converter = it.second.lock()->usable_power_out_tp(clock, remaining_request_load);//usable_power...overloaded for multi-conv!

		  } else {//case B: Storage Discharger / BiCharger
			reserved_pwr_from_converter = it.second.lock()->reserveDischarger(clock, remaining_request_load);//usable_power...overloaded for multi-conv!
		  }
		  //std::cout <<"\t *SUCESS Reserving "<<std::setw(20)<< it.second.lock()->code() << "\t| power_from_converter= "<<std::setw(10) << reserved_pwr_from_converter << " GW" << std::endl;
		  if (reserved_pwr_from_converter > genesys::ProgramSettings::approx_epsilon() ) {
			//std::cout << "===== converter active = " << it.second.lock()->code() << " with output power " << reserved_pwr_from_converter << std::endl;
			auto balanceable_pwr = std::min(reserved_pwr_from_converter, residual_load_TP_);
      /* Heat Integrationsversuch: kja
      if (it.second.lock()->useConverterOutput(clock, balanceable_pwr, "electric_energy")) {
      */
			if (it.second.lock()->useConverterOutput(clock, balanceable_pwr)) {
			  residual_load_TP_ += -balanceable_pwr;
			  //std::cout << "converter_balance() of " << balanceable_pwr << "GW succeeded in "<< code() << " with " << it.second.lock()->code()<< std::endl;
			  //std::cout << "\tremaining residual load: "<< residual_load_TP_ << " GW" << std::endl;
			} else {
			  std::cerr << "\tRegion::converter_balance - balance with reserved power failed to useConverterOutput()" << std::endl;
			  std::terminate();
			}
		  } else {
			//std::cout << "converter output too small" << std::endl;
		  }
		}
	  }
}

void Region::storage_charging(const dm_hsm::HSMCategory& category,
                       const aux::SimulationClock& clock) {
  //  std::cout << "FUNC-ID: Region::storage_charging in region "<<  code() << " "
  //          << aux::SimulationClock::time_point_to_string(clock.now())  << "\t|\t"
  //          << " residual load = " << residual_load_TP_ << std::endl;
  std::multimap<double, std::weak_ptr<dm_hsm::Converter>, std::greater<double> > tmp_map_converter = collectConverter(category, clock);
  double avail_chrg_pwr =0.;
  for (auto &it : tmp_map_converter) {
    if (residual_load_TP_ < -genesys::ProgramSettings::approx_epsilon()) { //check for excess generation after each converter
      avail_chrg_pwr = std::abs(residual_load_TP_);
    } else {
      break;
    }
    auto storage_conv_cat= it.second.lock()->get_HSMSubCategory();
    if (storage_conv_cat == dm_hsm::HSMSubCategory::BICHARGER || storage_conv_cat == dm_hsm::HSMSubCategory::CHARGER) {
      double max_chrg_pwr = it.second.lock()->reserveChargerInput(clock, std::abs(avail_chrg_pwr));
      if (max_chrg_pwr > 0 ) { //reduce RL with request to converter
        if (it.second.lock()->useChargeStorage(max_chrg_pwr, clock)) {
          residual_load_TP_ += max_chrg_pwr;
        }
      }
    }
  }
}//END Region::storage_balance

std::map<double,
           std::pair<std::weak_ptr<Link>, std::weak_ptr<TransmissionConverter>>,
           std::greater<double> > Region::createTempLinkMap(const std::weak_ptr<Link> hotlink,
                                                              const aux::SimulationClock& clock){
  //DEBUG std::cout << "\t\t" <<code()<< "Region::createTemporaryMap"<< std::endl;
  //map sorted by desccending efficiency
  std::map<double, std::pair<std::weak_ptr<Link>, std::weak_ptr<TransmissionConverter>>,
                             std::greater<double> > direct_el_lines_map;
  for (auto &it : line_map_el_){
    if (!hotlink.expired()){
      if (hotlink.lock() != it.second.first.lock()){ //exclusion of hotlink for balance - request
        if(it.second.second.lock()->get_capacity(clock) > genesys::ProgramSettings::approx_epsilon()) //only if any capacity on the link
        direct_el_lines_map.emplace(it.second.second.lock()->get_efficiency(clock),
                                    std::make_pair(it. second.first, it.second.second));
      }
    } else {//no active hotlink = direct neighbour
      if(it.second.second.lock()->get_capacity(clock) > genesys::ProgramSettings::approx_epsilon()) //only if any capacity on the link
        direct_el_lines_map.emplace(it.second.second.lock()->get_efficiency(clock),
                                          std::make_pair(it. second.first, it.second.second));
    }
  }
  return direct_el_lines_map;
}//END Region::createTemporaryMap

double Region::balance(int hops,
                       const double balance_demand,
                       const dm_hsm::HSMCategory& cat,
                       const aux::SimulationClock& clock,
                       const std::weak_ptr<Link>& hotlink) {
  //std::cout << "\t" <<code()<< "Region::balance - trying to balance " << balance_demand << " GW via " << hops << " hop-depth!" << std::endl;
  // This function should not be called with small values for balance_demand. Otherwise, return 0 since no balancing will be conducted
  if (balance_demand < genesys::ProgramSettings::approx_epsilon()) {
    return 0.0; //early return balance_demand
  }

  if (hops == 0) {  //units in direct neighbours
    std::map<double,
                 std::pair<std::weak_ptr<Link>, std::weak_ptr<TransmissionConverter>>,
                 std::greater<double> > lines_to_neighbours = createTempLinkMap(hotlink.lock(), clock); //map sorted by desccending efficiency
    double remaining_demand = balance_demand; //init
    for (auto& it : lines_to_neighbours){
      if (remaining_demand > genesys::ProgramSettings::approx_epsilon()) {
          double import =  it.second.first.lock()->getImportCapacity(cat, clock, code(), it.second.second, remaining_demand);
          if (import > 0){
        	  //std::cout << "\t\t could get potential import capacity to" << code() << " : GW= " << import << std::endl;
          }
          //Valid case
          if (import > genesys::ProgramSettings::approx_epsilon() && import != std::numeric_limits<double>::infinity() ){
            if (it.second.first.lock()->useImportCapacity(cat, import, clock, code(), it.second.second, it.second.first)) {
              remaining_demand -= import;
              //std::cout << "could import " << import << "GW to "<< code() << std::endl;
              //Update tracking variable
              imported_electricity_ += aux::TimeSeriesConst(std::vector<double>{import, 0.0},
                                                                                clock.now(),
                                                                                clock.tick_length() );
            }
			//else {
			//std::cout << "**WARNING: could not useImportCapacity() - transmission converter operating in other direction or reservation failed!" << std::endl;
			//}
          }
          if (remaining_demand < genesys::ProgramSettings::approx_epsilon()) {
            remaining_demand = 0.;
            break;//breaks for-loop
          } // else try next link!
      } else {
        remaining_demand = 0.;
        break;//breaks for-loop
      }
    }
    return (balance_demand-remaining_demand);

  } else {//hops > 0    /
    --hops;
    std::map<double,
             std::pair<std::weak_ptr<Link>, std::weak_ptr<TransmissionConverter>>,
             std::greater<double> > direct_el_lines_map = createTempLinkMap(hotlink.lock(), clock); //map sorted by desccending efficiency
    double remaining_demand = balance_demand; //init
    for (auto& it : direct_el_lines_map){
      if (remaining_demand > genesys::ProgramSettings::approx_epsilon()) {
        remaining_demand -=  balance(hops, remaining_demand, cat, clock, it.second.first.lock());
        if (remaining_demand < genesys::ProgramSettings::approx_epsilon()) {
          remaining_demand = 0.;
          break;//breaks for-loop
        } // else try next link!
      } else {
        remaining_demand = 0.;
        break;//breaks for-loop
      }
    }
    return (balance_demand-remaining_demand);//default return

  }//end hops>0 option
  std::cerr << "Region::balance return without correct result" << std::endl;
  std::terminate();
  return 0.0; //error return
}

std::tuple<std::vector<double>, aux::SimulationClock::time_point> Region::calc_energy_vector(aux::SimulationClock::time_point start,
                                                                                             aux::SimulationClock::time_point end) const {
  std::vector<double> annual_demand_vec;
  // compute first start of a new year at or past tp start
  auto begin_of_start_year = aux::SimulationClock::time_point(
      std::chrono::duration_cast<aux::years>(start.time_since_epoch()));
  auto begin_of_end_year = aux::SimulationClock::time_point(
      std::chrono::duration_cast<aux::years>(end.time_since_epoch()));
  auto next_year = begin_of_start_year + aux::years(1);
  if (start.time_since_epoch() % aux::years(1) != aux::years(0)) {
    auto end_of_period = std::min(next_year, end);
    annual_demand_vec.push_back(demand_electric(start, end_of_period).Mean(start, end_of_period)
                                  * std::chrono::duration_cast<std::chrono::duration<double, aux::hours::period> >(
                                      end_of_period - start).count());
  } else {
      annual_demand_vec.push_back(demand_electric_per_a(start));
  }
  aux::SimulationClock annual_clock(begin_of_start_year, aux::years(1));
  while (annual_clock.tick() < end)
    annual_demand_vec.push_back(demand_electric_per_a(annual_clock.now()));
  if ((end.time_since_epoch() % aux::years(1) != aux::years(0)) && (begin_of_start_year != begin_of_end_year)) {
    annual_demand_vec.push_back(demand_electric(begin_of_end_year, end).Mean(begin_of_end_year, end)
                                * std::chrono::duration_cast<std::chrono::duration<double, aux::hours::period> >(
                                      end - begin_of_end_year).count());
  }
  return std::make_tuple(annual_demand_vec, begin_of_start_year);
}

double Region::calc_SelfSupplyQuota(const aux::SimulationClock& clock) {
  double quota = 0.;
  double demand_year = demand_electric_per_a(clock.now());

  auto start_tp = clock.now() - aux::years(1);
  try {
    quota = collect_generation(start_tp, clock.now(), clock.tick_length())/ demand_year;
  } catch (const std::exception& e) {
    std::cout << "ERROR in Region::calc_SelfSupplyQuota()" << std::endl;
    std::cout << e.what(); // information from error printed
  }
  annual_selfsupply_quota_ += aux::TimeSeriesConst(std::vector<double>{quota, 0.}, start_tp, clock.tick_length());
  //std::cout << "\t" << code() << " | Quota= " << quota << " | demand current year = "<< demand_year << " GWh" << std::endl;

  double barrier = 1.;//always transform to values > 1
  //double l_lim = genesys::ProgramSettings::SQ_lower_limit_();
  //double u_lim = genesys::ProgramSettings::SQ_upper_limit_();
  double l_lim = genesys::ProgramSettings::penalties().find("self_supply_quota_lower_limit")->second;
  double u_lim = genesys::ProgramSettings::penalties().find("self_supply_quota_upper_limit")->second;
  if (genesys::ProgramSettings::approx_epsilon() < quota &&
      quota < l_lim + genesys::ProgramSettings::approx_epsilon()) {
    barrier += u_lim + (l_lim -quota); // mirror values < lower_limit to the upper limit
    //return (l_lim+ u_lim)*quota;
  } else if (quota > u_lim + genesys::ProgramSettings::approx_epsilon()) {
    barrier += quota -u_lim;
  } else {
    return 0.;
  }
  return barrier;
}

double Region::collect_generation(const aux::SimulationClock::time_point start,
                                  const aux::SimulationClock::time_point end,
                                  const aux::SimulationClock::duration interval) {
  double generation = 0.;

  for(auto& it : converter_ptrs_) {
    auto cat =  it.second->get_HSMCategory();
    if (cat != HSMCategory::LINE_CONVERTER && cat  != HSMCategory::UNAVAILABLE) {
      //&& cat != HSMCategory::LT_STORAGE     && cat  != HSMCategory::ST_STORAGE
      //std::cout << " \t" << it.second->code() << " collecting generation " << std::endl;
      generation += it.second->get_sum("generation", start, end, interval);
    }
  }

  return generation;

}

const aux::TimeSeriesConstAddable Region::get_load() const{

	auto tick_length = genesys::ProgramSettings::simulation_step_length();
	aux::SimulationClock::time_point tp_start = genesys::ProgramSettings::simulation_start();
	aux::SimulationClock::time_point tp_end = genesys::ProgramSettings::simulation_end();
	aux::SimulationClock clock(tp_start, tick_length);

	std::vector<double> load;
	do {
		load.push_back(demand_electric(clock.now()));
	} while (clock.tick() < tp_end);
	load.push_back(0.0);//end of the timeSeries
	return aux::TimeSeriesConst(load, tp_start, tick_length);
}


void Region::connect_multi_converter(std::pair<std::string, std::shared_ptr<Converter>> conv){
  auto cat = conv.second->get_HSMCategory();
  //std::cout << " connect_multi_converter\t" << conv.second->code()
  //		  << " with hsm-cat= "<< static_cast<int>(cat) << std::endl;

  //CONNECT INPUTS**********************************************************************
  for (auto& in : conv.second->input_types()){
    //std::cout << "\t\t1) connect to INPUT:\t" << in << " of " << conv.first<< std::endl;
    if (primary_energy_ptrs_.find(in) != primary_energy_ptrs_.end()) {
      //    	std::cout << "\t\ttry to make connection to primary energy "
      //			  << primary_energy_ptrs_.find(in)->second->code() <<"...." << std::endl;
      try {
        conv.second->connectPrimaryEnergy(primary_energy_ptrs_.find(in)->second);
        primary_energy_ptrs_.find(in)->second->connect();
        //std::cout << "\t\t connection established to primary energy "
        //<< primary_energy_ptrs_.find(in)->second->code() << std::endl;
      } catch (...) {
        std::cerr <<"Region::connectReservoirs() for multiconverter : could not get Primary Energy type! "
                  <<  conv.second->input_type() << " for converter " << conv.first << std::endl;
      }
      //    } else if (storage_ptrs_.find(in) != storage_ptrs_.end()) {
      //      std::cout << "found " << in << " in storage ptrs" << std::endl;
    } else {
      if (in !="electric_energy") {
        std::cerr << "**Warning in Region::connect_multi_converter: could not process primary energy INPUT "
                  << in << " for " << conv.first<< std::endl;
        std::terminate();
      }
    }
  }
  //CONNECT OUTPUTS**********************************************************************
  for (auto& out : conv.second->output_types()) {
    //std::cout << "\t\t2) connect to OUTPUT:...." << out << std::endl;
    if (primary_energy_ptrs_.find(out) != primary_energy_ptrs_.end()) {
      //std::cout <<"\t\t"<< primary_energy_ptrs_.find(out)->second->code() << std::endl;
      //connect to primary energy -> save shared_ptr
      //std::cout << "\t\ttry to make connection to primary energy " <<primary_energy_ptrs_.find(out)->second->code() <<"...." << std::endl;
      try {
        if (out == "CO2" || out == "CO2_"+code()){
        //std::cout << "in region "<< code() << " found primary energy for CO2" << out << std::endl;
          conv.second->connectCo2Reservoir(primary_energy_ptrs_.find(out)->second);
          primary_energy_ptrs_.find(out)->second->connect();
        //} else if (out == "heat") {
        //  std::cout << "should connect to storage not PE" << std::endl;
          //if (module_heat_active())
          //  conv.second->connectPrimaryEnergy(primary_energy_ptrs_.find(out)->second);
        } else {
          std::cout << "*************no connection for output " << out << " defined " << std::endl;
          return;
        }
        //std::cout << "\t\t connection established to primary energy " <<primary_energy_ptrs_.find(out)->second->code() << std::endl;
      } catch (...) {
        std::cerr <<"Region::connectReservoirs() for multiconverter : could not get Primary Energy type! "
                  <<  conv.second->input_type() << " for converter " << conv.first << std::endl;
      }
//    } else if (storage_ptrs_.find(out) != storage_ptrs_.end()) {
//    //
//      std::cout << "\t\t"<<storage_ptrs_.find(out)->second->code() << std::endl;
//    //  std::cout << "\t\ttry to make connection to energy storage "<<storage_ptrs_.find(out)->second->code() <<"....todo" << std::endl;
//      if (out == "heat") {
//        std::cout << "DEBUG. should find heat-storage" << std::endl;
//      }
//      try {
//          conv.second->connectStorage(storage_ptrs_.find(out)->second);
//          storage_ptrs_.find(out)->second->connect(conv.second->bidirectional());
//        //std::cout << "\t\t connection established to energy storage  " <<primary_energy_ptrs_.find(out)->second->code() << std::endl;
//      } catch (...) {
//        std::cerr <<"Region::connectReservoirs() for multiconverter : could not get storage type! "
//                  <<  conv.second->input_type() << " for converter " << conv.first << std::endl;
//      }
    } else {
      for (auto& stor : storage_ptrs_){
        //        std::cout <<  " \t stor input: "<< stor.second->input_type() << std::endl;
        //        std::cout <<  " \t stor output: "<< stor.second->output_type() << std::endl;
        if (out == stor.second->input_type() ){
          //std::cout << ".......try to connect to storage " << stor.first << std::endl;
          try {
            conv.second->connectStorage(stor.second);
            stor.second->connect(conv.second->bidirectional());
            //            std::cout << "\t\t connection established to energy storage  " <<stor.first << std::endl;
          } catch (...) {
               std::cerr <<"Region::connectReservoirs() for multiconverter : could not get storage type! "
                         <<  conv.second->input_type() << " for converter " << conv.first << std::endl;
          }
        }
      }
    }
  }
}

void Region::connect_converter(std::pair<std::string, std::shared_ptr<Converter>> conv){
  //CONNECT INPUT**********************************************************************
  auto in = conv.second->input_type();
    //std::cout << "\t\t1)CONV " <<conv.first << " connect to INPUT:...." << in << std::endl;
    if (primary_energy_ptrs_.find(in) != primary_energy_ptrs_.end()) {
      //std::cout << "\t\t\tfound " << in << " in primary energy ptrs"<< std::endl;
      //std::cout << "\t\ttry to make connection to primary energy "
      //          << primary_energy_ptrs_.find(in)->second->code() <<"...." << std::endl;
      try {
        conv.second->connectPrimaryEnergy(primary_energy_ptrs_.find(in)->second);
        primary_energy_ptrs_.find(in)->second->connect();
        //        std::cout << "\t\t connection established to primary energy "
        //                  << primary_energy_ptrs_.find(in)->second->code() << std::endl;
      } catch (...) {
        std::cerr <<"Region::connectReservoirs() for converter : could not get Primary Energy type! "
                  <<  conv.second->input_type() << " for converter " << conv.first << std::endl;
      }
    } else if (storage_ptrs_.find(in) != storage_ptrs_.end()) {
      std::cerr << "ERROR in Region::connect_converter : case should not be called!" << std::endl;
      std::terminate();
      //      //std::cout << "found " << in  << " in storage ptrs"<< std::endl;
      //      try {
      //        conv.second->connectStorage(storage_ptrs_.find(in)->second);
      //        storage_ptrs_.find(in)->second->connect(conv.second->bidirectional());
      //        //        std::cout << "\t\t connection established to primary energy "
      //        //                  << primary_energy_ptrs_.find(in)->second->code() << std::endl;
      //       } catch (...) {
      //         std::cerr <<"Region::connectReservoirs() for converter : could not get Primary Energy type! "
      //                   <<  conv.second->input_type() << " for converter " << conv.first << std::endl;
      //       }

    } else {
      for (auto& stor : storage_ptrs_){
        //        std::cout <<  " \t stor input: "<< stor.second->input_type() << std::endl;
        //        std::cout <<  " \t stor output: "<< stor.second->output_type() << std::endl;
        if (in == stor.second->output_type() ){
          //std::cout << "DEBUG.......try to connect converter  input to storage " << stor.first << std::endl;
          try {
            conv.second->connectStorage(stor.second);
            stor.second->connect(conv.second->bidirectional());
            //            std::cout << "\t\t connection established to energy storage  " <<stor.first << std::endl;
          } catch (...) {
               std::cerr <<"Region::connectReservoirs() for converter : could not get storage type! "
                         <<  conv.second->output_type() << " for converter " << conv.first << std::endl;
          }
        }
      }
    }
  //CONNECT OUTPUT**********************************************************************
  auto out = conv.second->output_type();
  //std::cout << "\t\t2)CONV " <<conv.first << " connect to OUTPUT:...." << out << std::endl;
  if (primary_energy_ptrs_.find(out) != primary_energy_ptrs_.end()) {
  //  std::cout << "found " << out << " in primary energy ptrs"<< std::endl;
  //  } else if (storage_ptrs_.find(out) != storage_ptrs_.end()) {
  //    std::cout << "found " << out  << " in storage ptrs"<< std::endl;
  } else {
    for (auto& stor : storage_ptrs_){
      //        std::cout <<  " \t stor input: "<< stor.second->input_type() << std::endl;
      //        std::cout <<  " \t stor output: "<< stor.second->output_type() << std::endl;
      if (out == stor.second->input_type() ){
        //std::cout << ".......try to connect converter output to storage " << stor.first << std::endl;
        try {
          conv.second->connectStorage(stor.second);
          stor.second->connect(conv.second->bidirectional());
          //            std::cout << "\t\t connection established to energy storage  " <<stor.first << std::endl;
        } catch (...) {
             std::cerr <<"Region::connectReservoirs() for converter : could not get storage type! "
                       <<  conv.second->input_type() << " for converter " << conv.first << std::endl;
        }
      }
    }
  }
}


void Region::manual_connect_conv(std::pair<std::string, std::shared_ptr<Converter>> conv){
  auto cat = conv.second->get_HSMCategory();
  std::cout << " connect_converter\t" << conv.second->code()
            << " with hsm-cat= "<< static_cast<int>(cat) << std::endl;

  std::string conv_code = conv.second->code();
  std::string search_string;
  if (conv_code.compare("WIND1") == 0) {
    search_string = "WIND1";
  } else if (conv_code.compare("WIND2") == 0) {
    search_string = "WIND2";
  } else if (conv_code.compare("WIND_OFF") == 0) {
    search_string = "WIND_OFF";
  } else if (conv_code.compare("PV1") == 0) {
    search_string = "PV1";
  } else if (conv_code.compare("PV2") == 0) {
    search_string = "PV2";
  } else if (conv_code.compare("ROR") == 0) {
            search_string = "RORIVER";
  } else if (conv_code.compare("LIGNITE_OCTURBINE") == 0) {
    search_string = "LIGNITE";
  } else if (conv_code.compare("HARDCOAL_OCTURBINE") == 0) {
    search_string = "HARD_COAL";
  } else if (conv_code.compare("NGAS_TURBINE") == 0) {
    search_string = "METHANE";
  } else if (conv_code.compare("NUCLEAR_TURBINE") == 0 || conv_code.compare("NUCLEAR_TURBINE_DEU") == 0) {
    search_string = "URANIUM";
  } else if (conv_code.compare("BIOMASS_TURBINE") == 0) {
    search_string = "BIOMASS";
  } else if (conv_code.compare("BAT1POWER") == 0) {
    search_string = "BAT1";
  } else if (conv_code.compare("BAT2POWER") == 0) {
    search_string = "BAT2";
  } else if (conv_code.compare("BAT3POWER") == 0) {
    search_string = "BAT3";
  } else if (conv_code.compare("BAT4POWER") == 0) {
    search_string = "BAT4";
  } else if (conv_code.compare("PH_TURBINE") == 0) {
    search_string = "PH";
  } else if (conv_code.compare("H2_ELECTROLYSER") == 0) {
    search_string = "H2";
  } else if (conv_code.compare("CCH2_TURBINE") == 0) {
    search_string = "H2";
  }
  if (search_string.empty()) {
    std::cerr << "ERROR in dm_hsm::Region::connectReservoirs :" << std::endl
        << "unknown converter " << conv_code << " in region " << code() << std::endl;
    std::terminate();
  } else {
    if (cat == dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR || cat == dm_hsm::HSMCategory::RE_GENERATOR) {
      if (auto tmp_ptr = get_pointer_to<PrimaryEnergy>(primary_energy_ptrs_, search_string)) {
        conv.second->connectPrimaryEnergy(tmp_ptr);
        tmp_ptr->connect();
        //std::cout << "connected " << tmp_ptr->code() << " to " << it.second->code() << std::endl;
      } else {
        std::cerr << "ERROR in dm_hsm::Region::connectReservoirs :" << std::endl
            << "primary energy " << search_string << " not found in region " << name() << ", " << code() << std::endl;
        std::terminate();
      }
    } else if ((cat == dm_hsm::HSMCategory::LT_STORAGE) || (cat == dm_hsm::HSMCategory::ST_STORAGE)) {
      if (auto tmp_ptr = get_pointer_to<Storage>(storage_ptrs_, search_string)) {
        conv.second->connectStorage(tmp_ptr);
        tmp_ptr->connect(conv.second->bidirectional());
      } else {
        std::cerr << "ERROR in dm_hsm::Region::connectReservoirs :" << std::endl
            << "storage " << search_string << " not found in region " << name() << ", " << code() << std::endl;
        std::terminate();
      }
    } else {
      std::cerr << "ERROR in dm_hsm::Region::connectReservoirs :" << std::endl
          << "HSMCategory " << static_cast<int>(cat) << " not found in region " << name() << ", " << code()
          << std::endl;
      std::terminate();
    }
  }
}

/* Heat Integrationsversuch: kja
void Region::discharge_heat_storage(const dm_hsm::HSMCategory& category,
                                    const aux::SimulationClock& clock){
  //release heat from storage to virtual local heating grid
  std::multimap<double, std::weak_ptr<dm_hsm::Storage>, std::greater<double> > tmp_map_storage= collectStorages(category, clock);
  double avail_chrg_pwr =0.;
  for (auto &it : tmp_map_storage) {
    std::cout << it.first <<std::endl;
  //    if (residual_load_TP_ < -genesys::ProgramSettings::approx_epsilon()) { //check for excess generation after each converter
  //      avail_chrg_pwr = std::abs(residual_load_TP_);
  //    } else {
  //      break;
  //    }
    //auto storage_conv_cat= it.second.lock()->get_HSMSubCategory();
  }
}
*/

std::multimap<double, std::weak_ptr<dm_hsm::Storage>, std::greater<double> >
                                                            Region::collectStorages(dm_hsm::HSMCategory cat,
                                                                                    const aux::SimulationClock& clock){
  std::multimap<double, std::weak_ptr<dm_hsm::Storage>, std::greater<double> > tmp_map_storage;

  return tmp_map_storage;
}


} /* namespace dm_hsm */
