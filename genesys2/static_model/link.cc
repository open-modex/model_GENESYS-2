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
// link.cc
//
// This file is part of the genesys-framework v.2

#include <static_model/link.h>

namespace sm {

Link::Link(const Link& other)
    : am::Link(other) {
  if (!other.converter_ptrs_.empty()) {
    for (const auto &it : other.converter_ptrs_)
      converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>( new Converter(*it.second)));
  }
}

Link::Link(const am::Link& origin,
           const std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                            aux::SimulationClock::time_point,
                                                            aux::SimulationClock::time_point,
                                                            aux::SimulationClock::duration> >& installations,
           const std::unordered_map<std::string, std::shared_ptr<Region> >& region)
    : am::Link(origin) {
  if (!am::Link::converter_ptrs().empty()) {
    for (const auto& it : am::Link::converter_ptrs()) {
      auto converter_pos = installations.find(it.first);
      if (converter_pos != installations.end()) {
        converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>(
            new Converter(*(it.second), std::move((*std::get<0>(converter_pos->second)).clone()),
                                   std::get<1>(converter_pos->second), std::get<2>(converter_pos->second),
                                   std::get<3>(converter_pos->second))));
        //std::cout << "link" << it.first << std::endl;
        //installations.erase(it.first);
      } else {
        converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>(
            new Converter(*(it.second))));
      }
    }
  }
  //  if (installations.size() > 0) {
  //      std::cerr << "ERROR: could not apply all installations due to faulty parametrisation in " << code() << std::endl;
  //      for (auto& it : installations){
  //        std::cout << "\t" << code()<< "." << it.first <<" needs to be removed" << std::endl;
  //      }
  //      std::cerr << "ERROR in sm::Link c'tor : installation list contained invalid entries" << std::endl;
  //      std::terminate();
  //  }
  RegisterRegions(region);
}

Link::Link(const am::Link& origin,
           const std::unordered_map<std::string, std::shared_ptr<Region> >& region)
    : am::Link(origin) {
  if (!am::Link::converter_ptrs().empty()) {
    for (const auto& it : am::Link::converter_ptrs())
        converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>(
            new Converter(*(it.second))));
  }
  RegisterRegions(region);
}

void Link::RegisterRegions(const std::unordered_map<std::string, std::shared_ptr<Region> >& region) {
  auto region_A_pos = region.find(region_A());
  if (region_A_pos != region.end()) {
    region_A_ptr_ = region_A_pos->second;
  } else {
    std::cerr << "ERROR in sm::Link::Link :" << std::endl
        << "Region_A " << region_A() << " not found in Region list region" << std::endl;
    std::terminate();
  }
  auto region_B_pos = region.find(region_B());
  if (region_B_pos != region.end()) {
    region_B_ptr_ = region_B_pos->second;
  } else {
    std::cerr << "ERROR in sm::Link::Link :" << std::endl
        << "Region_B " << region_B() << " not found in Region list region" << std::endl;
    std::terminate();
  }
}

} /* namespace sm */
