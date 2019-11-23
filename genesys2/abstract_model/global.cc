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

#include <abstract_model/global.h>
#include <builder/global_proto.h>

#include <exception>
#include <iostream>
#include <tuple>

namespace am {

Global::Global(const Global& other) : GlobalProto(other) {
  if (!other.primary_energy_ptrs_.empty()) {
    for (const auto &it : other.primary_energy_ptrs_)
      primary_energy_ptrs_.emplace(it.first, std::shared_ptr<PrimaryEnergy>(
          new PrimaryEnergy(*it.second)));
  }
  if (!other.storage_ptrs_.empty()) {
    for (const auto &it : other.storage_ptrs_)
      storage_ptrs_.emplace(it.first, std::shared_ptr<SysComponentActive>(
          new SysComponentActive(*it.second)));
  }
}

Global::Global(const builder::GlobalProto& origin,
               const std::unordered_map<std::string, std::unique_ptr<builder::PrimEnergyPrototype> >& primary_energy,
               const std::unordered_map<std::string, std::unique_ptr<builder::SysCompActPrototype> >& storage)
    : GlobalProto(origin) {
  if (!primary_energy_list().empty()) {
	  for (const auto& it : primary_energy_list()) {
	      auto primary_energy_pos = primary_energy.find(it.first);
	      if (primary_energy_pos != primary_energy.end()) {
	        primary_energy_ptrs_.emplace(it.first, std::shared_ptr<PrimaryEnergy>(
	            new PrimaryEnergy(*primary_energy_pos->second, it.second)));
	      } else {
	        std::cerr << "ERROR in am::Global::Global :" << std::endl
	            << "Primary Energy " << it.first << " not found in PrimEnergyPrototype list primary_energy" << std::endl;
	        std::terminate();
	      }
	  }
  }
  if (!storage_list().empty()) {
	  for (const auto& it : storage_list()) {
	      auto storage_pos = storage.find(it.first);
	      if (storage_pos != storage.end()) {
	        storage_ptrs_.emplace(it.first, std::shared_ptr<SysComponentActive>(
	            new SysComponentActive(*storage_pos->second, std::move(std::get<0>(it.second)->clone()),
	                                   std::get<1>(it.second), std::get<2>(it.second), std::get<3>(it.second))));
	      } else {
	        std::cerr << "ERROR in am::Global::Global :" << std::endl
	            << "Storage " << it.first << " not found in SysCompActPrototype list storage" << std::endl;
	        std::terminate();
	      }
	   }
  }

}

} /* namespace am */