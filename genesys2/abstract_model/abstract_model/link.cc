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

#include <abstract_model/link.h>

#include <exception>
#include <iostream>

namespace am {

Link::Link(const LinkPrototype& origin,
		       const std::unordered_map<std::string, std::unique_ptr<builder::TransmConvPrototype> >& converter,
		       const std::unordered_map<std::string, std::shared_ptr<Region> >& region)
    : LinkPrototype(origin) {
  for (const auto& it : converter_list()) {
    auto converter_pos = converter.find(it.first);
    if (converter_pos != converter.end()) {
      converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>(
          new Converter(*converter_pos->second, std::move((*std::get<0>(it.second)).clone()),
                                 std::get<1>(it.second), std::get<2>(it.second), std::get<3>(it.second), length())));
    } else {
      std::cerr << "ERROR in am::Link::Link :" << std::endl
          << "Converter " << it.first << " not found in TransmConvPrototype list converter" << std::endl;
      std::terminate();
    }
  }
  RegisterRegions(region);
}

Link::Link(const Link& other,
           const std::unordered_map<std::string, std::shared_ptr<Region> >& region)
  : LinkPrototype(other) {
  if (!other.converter_ptrs_.empty()) {
    for (const auto &it : other.converter_ptrs_)
      converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>(
          new Converter(*it.second)));
  }
  RegisterRegions(region);
}

Link::Link(const Link& other)
    : LinkPrototype(other) {
  if (!other.converter_ptrs_.empty()) {
    for (const auto &it : other.converter_ptrs_)
      converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>( new Converter(*it.second)));
  }
}

void Link::RegisterRegions(const std::unordered_map<std::string, std::shared_ptr<Region> >& region) {
  auto region_A_pos = region.find(region_A());
  if (region_A_pos != region.end()) {
    region_A_ptr_ = region_A_pos->second;
  } else {
    std::cerr << "ERROR in am::Link::Link :" << std::endl
        << "Region_A " << region_A() << " not found in Region list region" << std::endl;
    std::terminate();
  }
  auto region_B_pos = region.find(region_B());
  if (region_B_pos != region.end()) {
    region_B_ptr_ = region_B_pos->second;
  } else {
    std::cerr << "ERROR in am::Link::Link :" << std::endl
        << "Region_B " << region_B() << " not found in Region list region" << std::endl;
    std::terminate();
  }
}

} /* namespace am */
