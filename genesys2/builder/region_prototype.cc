/// ==================================================================
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
// region_prototype.cc
//
// This file is part of the genesys-framework v.2

#include <builder/region_prototype.h>

#include <program_settings.h>

namespace builder {

RegionPrototype::RegionPrototype(const RegionPrototype& other)
    : SysComponent(other),
      demand_electric_dyn_(std::move(other.demand_electric_dyn_->clone())),
      demand_electric_per_a_(std::move(other.demand_electric_per_a_->clone())),
      module_heat_active_ (other.module_heat_active_){
  if (!other.primary_energy_list_.empty()) {
    for (const auto &it : other.primary_energy_list_) {
      primary_energy_list_.emplace(it.first, it.second);
    }
  }
  if (!other.converter_list_.empty()) {
    for (const auto &it : other.converter_list_) {
      converter_list_.emplace(it.first, std::make_tuple(std::get<0>(it.second)->clone(), std::get<1>(it.second),
                                                        std::get<2>(it.second), std::get<3>(it.second)));
    }
  }
  if (!other.multi_converter_list_.empty()) {
      for (const auto &it : other.multi_converter_list_) {
        multi_converter_list_.emplace(it.first, std::make_tuple(std::get<0>(it.second)->clone(), std::get<1>(it.second),
                                                          std::get<2>(it.second), std::get<3>(it.second)));
    }
  }
  if (!other.storage_list_.empty()) {
    for (const auto &it : other.storage_list_) {
      storage_list_.emplace(it.first, std::make_tuple(std::get<0>(it.second)->clone(), std::get<1>(it.second),
                                                      std::get<2>(it.second), std::get<3>(it.second)));
    }
  }
  if(other.demand_heat_dyn_)
    demand_heat_dyn_ = (std::move(other.demand_heat_dyn_->clone()));
  if(other.demand_heat_per_a_)
    demand_heat_per_a_ = (std::move(other.demand_heat_per_a_->clone()));
  if(other.ambient_temp_dyn_)
    ambient_temp_dyn_ = (std::move(other.ambient_temp_dyn_->clone()));
}

RegionPrototype::RegionPrototype(const std::string& code,
                                 const std::string& name,
                                 std::unique_ptr<aux::TimeBasedData> demand_electric_dyn,
                                 std::unique_ptr<aux::TimeBasedData> demand_electric_per_a,
                                 std::unordered_map<std::string, aux::TBDLookupTable> primary_energy_list,
                                 std::unordered_map<std::string,
                                                    std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                               aux::SimulationClock::time_point,
                                                               aux::SimulationClock::time_point,
                                                               aux::SimulationClock::duration> > converter_list,
								                 std::unordered_map<std::string,
								                                    std::tuple<std::unique_ptr<aux::TimeBasedData>,
								                                               aux::SimulationClock::time_point,
								                                               aux::SimulationClock::time_point,
								                                               aux::SimulationClock::duration> > storage_list,
                                 std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                                            aux::SimulationClock::time_point,
                                                                            aux::SimulationClock::time_point,
                                                                            aux::SimulationClock::duration> >
                                                                                                 multi_converter_list,//optional
                                 std::unique_ptr<aux::TimeBasedData> demand_heat_dyn,  //optional
                                 std::unique_ptr<aux::TimeBasedData> demand_heat_per_a,//optional
                                 std::unique_ptr<aux::TimeBasedData> ambient_temp_dyn,//optional
                                 bool module_heat_active)//optional
    : SysComponent(code, name),
	  demand_electric_dyn_(std::move(demand_electric_dyn)),
	  demand_electric_per_a_(std::move(demand_electric_per_a)),
    //	  demand_heat_dyn_(std::move(demand_heat_dyn)),  --> only adopted if value not empty
    //	  demand_heat_per_a_(std::move(demand_heat_per_a)),
    //	  ambient_temp_dyn_(std::move(ambient_temp_dyn)),
	  module_heat_active_(module_heat_active){
  if (!primary_energy_list.empty()) {
    for(auto &&it: primary_energy_list) {
      primary_energy_list_.emplace(it.first, std::move(it.second));
    }
  }
  if (!converter_list.empty()) {
    for(auto &&it: converter_list) {
      converter_list_.emplace(it.first, std::move(it.second));
    }
  }
  if (!multi_converter_list.empty()) {
    for(auto &&it: multi_converter_list) {
      multi_converter_list_.emplace(it.first, std::move(it.second));
    }
  }
  if (!storage_list.empty()) {
    for(auto &&it: storage_list) {
      storage_list_.emplace(it.first, std::move(it.second));
    }
  }
  //optional fields moved only if ptr exist
  if (demand_heat_dyn)
    demand_heat_dyn_ = (std::move(demand_heat_dyn));
  if (demand_heat_per_a)
    demand_heat_per_a_ = (std::move(demand_heat_per_a));
  if (ambient_temp_dyn)
    ambient_temp_dyn_ = (std::move(ambient_temp_dyn));
}

double RegionPrototype::demand_heat(aux::SimulationClock::time_point tp) const {
  if (module_heat_active_) {
    return (*demand_heat_dyn_)[tp] * (*demand_heat_per_a_)[tp];
    // Kevin wird doch nicht benötigt, wenn normiertes Leistungsprofil einer WP und Wärmeenergie pro Jahr bekannt
    //    if ((*ambient_temp_dyn_)[tp] - 10) > 0) {
    //      return ((*ambient_temp_dyn_)[tp] - 10) * (*demand_heat_per_a_)[tp];
    //   } else {
    //       return 0;
    //}
 } else {
    std::cerr << "No heat active in region" << code() << "! request invalid" << std::endl;
    std::terminate();
  }
  return 0.;
}

double RegionPrototype::demand_heat_per_a(aux::SimulationClock::time_point tp) const {
  if (module_heat_active_) {
    return (*demand_heat_per_a_)[tp];
  } else {
     std::cerr << "No heat active in region" << code() << "! request invalid" << std::endl;
     std::terminate();
   }
  return 0.;
}

double RegionPrototype::ambient_temp(aux::SimulationClock::time_point tp) const {
  if (module_heat_active_) {
    return (*ambient_temp_dyn_)[tp];
  } else {
     std::cerr << "No heat active in region" << code() << "! request invalid" << std::endl;
     std::terminate();
   }
  return 0.;
}

aux::TimeSeriesConst RegionPrototype::demand_electric(aux::SimulationClock::time_point start,
                                                      aux::SimulationClock::time_point end) const {
  aux::SimulationClock sub_clock(start, genesys::ProgramSettings::simulation_step_length());
  std::vector<double> demand_vec;
  do {
    demand_vec.push_back(demand_electric(sub_clock.now()));
  } while (sub_clock.tick() < end);
  demand_vec.push_back(0.0);
  return (aux::TimeSeriesConst(demand_vec, start, genesys::ProgramSettings::simulation_step_length()));
}

} /* namespace builder */
