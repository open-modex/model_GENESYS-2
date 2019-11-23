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
// link_prototype.cc
//
// This file is part of the genesys-framework v.2

#include <builder/link_prototype.h>
#include <exception> //cerr
#include <iostream> //std::cout

namespace builder {

LinkPrototype::LinkPrototype(const LinkPrototype& other)
    : SysComponent(other),
      region_A_(other.region_A_),
      region_B_(other.region_B_),
      length_(std::move(other.length_->clone())) {
  if (!other.converter_list_.empty()) {
    for (const auto &it : other.converter_list_) {
      converter_list_.emplace(
          it.first, std::make_tuple(std::get<0>(it.second)->clone(), std::get<1>(it.second),
                                    std::get<2>(it.second), std::get<3>(it.second), std::get<4>(it.second)));
    }
  }
}

LinkPrototype::LinkPrototype(const std::string& code,
                             const std::string& region_A,
                             const std::string& region_B,
                             std::unique_ptr<aux::TimeBasedData> length,
                             std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                                        aux::SimulationClock::time_point,
                                                                        aux::SimulationClock::time_point,
                                                                        aux::SimulationClock::duration,
                                                                        model::TransmConverterType> > converter_list)
    : SysComponent(code, region_A + "-" + region_B),
      region_A_(region_A),
      region_B_(region_B),
      length_(std::move(length)) {
  if(!converter_list.empty()){
    for(auto &&it: converter_list) {
      converter_list_.emplace(it.first, std::move(it.second));
    }
  } else {
    std::cerr << "***WARNING: LinkPrototype::LinkPrototype - converter_list was empty - "
                 "possible wrong parametrisation in Link.csv - code:" << code << std::endl;
  }
}

} /* namespace am */
