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
// global_proto.cc
//
// This file is part of the genesys-framework v.2

#include <builder/global_proto.h>
#include <program_settings.h>

namespace builder {

GlobalProto::GlobalProto(const GlobalProto& other)
    : SysComponent(other) {
  if (!other.primary_energy_list_.empty()) {
    for (const auto &it : other.primary_energy_list_) {
      primary_energy_list_.emplace(it.first, it.second);
    }
  }

  if (!other.storage_list_.empty()) {
    for (const auto &it : other.storage_list_) {
      storage_list_.emplace(it.first, std::make_tuple(std::get<0>(it.second)->clone(), std::get<1>(it.second),
                                                      std::get<2>(it.second), std::get<3>(it.second)));
    }
  }
}



GlobalProto::GlobalProto(const std::string& code,
                         const std::string& name,
                         std::unordered_map<std::string, aux::TBDLookupTable> primary_energy_list,
                         std::unordered_map<std::string,
						                                std::tuple<std::unique_ptr<aux::TimeBasedData>,
								                                       aux::SimulationClock::time_point,
								                                       aux::SimulationClock::time_point,
								                                       aux::SimulationClock::duration> > storage_list)
    : SysComponent(code, name) {
  if (!primary_energy_list.empty()) {
	  for(auto &&it: primary_energy_list) {
	      primary_energy_list_.emplace(it.first, std::move(it.second));
	  }
  }
  if (!storage_list.empty()) {
	  for(auto &&it: storage_list) {
	      storage_list_.emplace(it.first, std::move(it.second));
	  }
  }

}

}

