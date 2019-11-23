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
// region.cc
//
// This file is part of the genesys-framework v.2

#include <static_model/region.h>

namespace sm {

Region::Region(const sm::Region& other)
    : am::Region(other) {
  if (!other.converter_ptrs_.empty()) {
    for (const auto &it : other.converter_ptrs_)
      converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>(
          new Converter(*it.second)));
  }
  if (!other.multi_converter_ptrs_.empty()) {
      for (const auto &it : other.multi_converter_ptrs_)
        multi_converter_ptrs_.emplace(it.first, std::shared_ptr<MultiConverter>(
            new MultiConverter(*it.second)));
  }
  if (!other.storage_ptrs_.empty()) {
    for (const auto &it : other.storage_ptrs_)
      storage_ptrs_.emplace(it.first, std::shared_ptr<SysComponentActive>(
          new SysComponentActive(*it.second)));
  }
}

Region::Region(const am::Region& origin,
               std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                                aux::SimulationClock::time_point,
                                                                aux::SimulationClock::time_point,
                                                                aux::SimulationClock::duration> >& installations)
    : am::Region(origin) {
  //if parent class has a component installation create for each from origin with additional installations
  if (!am::Region::converter_ptrs().empty()) {
    for (const auto& it : am::Region::converter_ptrs()) {
      auto converter_pos = installations.find(it.first);
      if (converter_pos != installations.end()) {
        //        std::cout << " found installation for " << it.second->code() << std::endl;
        //        std::cout << "\t " << converter_pos->first << std::endl;
        converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>(
            new Converter(*(it.second), std::move((*std::get<0>(converter_pos->second)).clone()),
                                   std::get<1>(converter_pos->second), std::get<2>(converter_pos->second),
                                   std::get<3>(converter_pos->second))));
        installations.erase(it.first);
      } else {
        //in case no additional installations, create new from origin
        converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>(
            new Converter(*(it.second))));
      }
    }
  }
  if (!am::Region::multi_converter_ptrs().empty()) {
    for (const auto& it : am::Region::multi_converter_ptrs()) {
      auto multi_conv_pos = installations.find(it.first);
      if (multi_conv_pos != installations.end()) {
        multi_converter_ptrs_.emplace(it.first, std::shared_ptr<MultiConverter>(
            new MultiConverter(*(it.second), std::move((*std::get<0>(multi_conv_pos->second)).clone()),
                                   std::get<1>(multi_conv_pos->second), std::get<2>(multi_conv_pos->second),
                                   std::get<3>(multi_conv_pos->second))));
        installations.erase(it.first);
      } else {
        multi_converter_ptrs_.emplace(it.first, std::shared_ptr<MultiConverter>(
            new MultiConverter(*(it.second))));
      }
    }
  }
  if (!am::Region::storage_ptrs().empty()) {
    //std::cout << am::Region::storage_ptrs().size() << "size storage ptrs" << std::endl;
    for (const auto& it : am::Region::storage_ptrs()) {
      auto storage_pos = installations.find(it.first);
      if (storage_pos != installations.end()) {
        storage_ptrs_.emplace(it.first, std::shared_ptr<SysComponentActive>(
            new SysComponentActive(*(it.second), std::move((*std::get<0>(storage_pos->second)).clone()),
                                   std::get<1>(storage_pos->second), std::get<2>(storage_pos->second),
                                   std::get<3>(storage_pos->second))));
        installations.erase(it.first);
      } else {
        storage_ptrs_.emplace(it.first, std::shared_ptr<SysComponentActive>(
            new SysComponentActive(*(it.second))));
      }
    }
  }
  if (installations.size() > 0) {
      std::cerr << "ERROR: could not apply all installations due to faulty parametrisation in " << code() << std::endl;
      for (auto& it : installations){
        std::cout << "\t" << code()<< "." << it.first <<" needs to be removed" << std::endl;
      }
      std::cerr << "ERROR in sm::Region c'tor : installation list contained invalid entries" << std::endl;
      std::terminate();
  }
}

Region::Region(const am::Region& origin)
    : am::Region(origin) {
  if (!am::Region::converter_ptrs().empty()) {
    for (const auto& it : am::Region::converter_ptrs())
        converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>(
            new Converter(*(it.second))));
  }
  if (!am::Region::multi_converter_ptrs().empty()) {
      for (const auto& it : am::Region::multi_converter_ptrs())
          multi_converter_ptrs_.emplace(it.first, std::shared_ptr<MultiConverter>(
              new MultiConverter(*(it.second))));
    }
  if (!am::Region::storage_ptrs().empty()) {
    for (const auto& it : am::Region::storage_ptrs())
        storage_ptrs_.emplace(it.first, std::shared_ptr<SysComponentActive>(
            new SysComponentActive(*(it.second))));
  }
}

} /* namespace sm */
