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
// static_model.cc
//
// This file is part of the genesys-framework v.2

#include <static_model/static_model.h>

namespace sm {

StaticModel::StaticModel(const StaticModel& other)
    : AbstractModel(other) {
  if (!other.regions_.empty()) {
    for (const auto& it : other.regions_)
      regions_.emplace(it.first, std::shared_ptr<Region>(new Region(*it.second)));
  }
  if (!other.links_.empty()) {
    for (const auto& it : other.links_)
      links_.emplace(it.first, std::shared_ptr<Link>(new Link(*it.second)));
  }
  if (!other.global_.empty()) {
	  //DEBUG std::cout << "\t***DEBUG StaticModel::StaticModel CopyC'Tor: found global in other model! " << std::endl;
      for (const auto& it : other.global_)
         global_.emplace(it.first, std::shared_ptr<Global>(new Global(*it.second)));
    }
}

StaticModel::StaticModel(const am::AbstractModel& origin,
                         std::unordered_map<std::string,
                                                  std::unordered_map<std::string,
                                                                     std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                                     aux::SimulationClock::time_point,
                                                                     aux::SimulationClock::time_point,
                                                                     aux::SimulationClock::duration>
                                                                     >
                                                  > installations)
    : AbstractModel(origin) {
  if (!AbstractModel::regions().empty()) {
    for (const auto& it : AbstractModel::regions()) {
      auto region_pos = installations.find(it.first);
      if (region_pos != installations.end()) {
        //std::cout << " found installation for " << it.second->code() << std::endl;
        //std::cout << "\t " << region_pos->first << std::endl;
        regions_.emplace(it.first, std::shared_ptr<Region>(new Region(*(it.second), region_pos->second)));
        installations.erase(it.first);
      } else {
        //std::cout << " found no installation for " << it.second->code() << std::endl;
        regions_.emplace(it.first, std::shared_ptr<Region>(new Region(*(it.second))));
      }
    }
  }
  if (!AbstractModel::links().empty()) {
    for (const auto& it : AbstractModel::links()) {
      auto link_pos = installations.find(it.first);
    	if (link_pos != installations.end()){
    	  links_.emplace(it.first, std::shared_ptr<Link>(new Link(*(it.second), link_pos->second, regions())));
    	  installations.erase(it.first);
      } else {
    		links_.emplace(it.first, std::shared_ptr<Link>(new Link(*(it.second), regions())));
    	}
    }
  }
  if (!AbstractModel::global().empty()) {
    //DEBUGstd::cout << "\t***DEBUG StaticModel::StaticModel C'Tor: found global in abstract model! " << std::endl;
    for (const auto& it : AbstractModel::global()) {
      auto glob_pos = installations.find(it.first);
      if (glob_pos != installations.end()) {
        global_.emplace(it.first, std::shared_ptr<Global>(new Global(*(it.second), glob_pos->second)));
        installations.erase(it.first);
      } else {
        global_.emplace(it.first, std::shared_ptr<Global>(new Global(*(it.second))));
      }
    }
  }
  if (installations.size() > 0) {
    std::cerr << "ERROR: could not apply all installations due to faulty parametrisation" << std::endl;
    for (auto& it : installations){
      std::cout << "\t" << it.first << std::endl;
      for (auto&& tt : it.second) {
        std::cout << "\t" << tt.first <<" needs to be removed" << std::endl;
      }
    }
    std::cerr << "ERROR in StaticModel c'tor : installation list contained invalid entries" << std::endl;
    std::terminate();
  }
}

} /* namespace sm */
