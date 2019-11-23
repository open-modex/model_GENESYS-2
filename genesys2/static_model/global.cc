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
// global.cc
//
// This file is part of the genesys-framework v.2

#include <static_model/global.h>

namespace sm {

Global::Global(const Global& other)
    : am::Global(other) {
  if (!other.storage_ptrs_.empty()) {
    for (const auto &it : other.storage_ptrs_)
      storage_ptrs_.emplace(it.first, std::shared_ptr<SysComponentActive>(
          new SysComponentActive(*it.second)));
  }
}

Global::Global(const am::Global& origin,
               const std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                                aux::SimulationClock::time_point,
                                                                aux::SimulationClock::time_point,
                                                                aux::SimulationClock::duration> >& installations)
    : am::Global(origin) {
  //if parent class has a component installation create for each from origin with additional installations
  if (!am::Global::storage_ptrs().empty()) {
    for (const auto& it : am::Global::storage_ptrs()) {
      auto storage_pos = installations.find(it.first);
      if (storage_pos != installations.end()) {
        storage_ptrs_.emplace(it.first, std::shared_ptr<SysComponentActive>(
            new SysComponentActive(*(it.second), std::move((*std::get<0>(storage_pos->second)).clone()),
                                   std::get<1>(storage_pos->second), std::get<2>(storage_pos->second),
                                   std::get<3>(storage_pos->second))));
      } else {
        storage_ptrs_.emplace(it.first, std::shared_ptr<SysComponentActive>(
            new SysComponentActive(*(it.second))));
      }
    }
  }
}

Global::Global(const am::Global& origin)
    : am::Global(origin) {
  if (!am::Global::storage_ptrs().empty()) {
    for (const auto& it : am::Global::storage_ptrs())
        storage_ptrs_.emplace(it.first, std::shared_ptr<SysComponentActive>(
            new SysComponentActive(*(it.second))));
  }
}

} /* namespace sm */
