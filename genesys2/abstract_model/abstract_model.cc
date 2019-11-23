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
// abstract_model.cc
//
// This file is part of the genesys-framework v.2

#include <abstract_model/abstract_model.h>

#include <utility>

namespace am {

AbstractModel::AbstractModel(const AbstractModel& other) {
  if (!other.regions_.empty()) {
    for (const auto& it : other.regions_)
       regions_.emplace(it.first, std::shared_ptr<Region>(new Region(*it.second)));
  }
  if (!other.links_.empty()) {
    for (const auto& it : other.links_)
       links_.emplace(it.first, std::shared_ptr<Link>(new Link(*it.second, regions_)));
  }
  if (!other.global_.empty()) {
	  //DEBUG std::cout << "\t***DEBUG AbstractModel CopyC'Tor: found global in other " << std::endl;
    for (const auto& it : other.global_)
       global_.emplace(it.first, std::shared_ptr<Global>(new Global(*it.second)));
  }
}

AbstractModel::AbstractModel(const std::unordered_map<std::string, std::unique_ptr<
                               builder::PrimEnergyPrototype> >& prim_energy_prototypes,
                             const std::unordered_map<std::string, std::unique_ptr<
                               builder::SysCompActPrototype> >& storage_prototypes,
                             const std::unordered_map<std::string, std::unique_ptr<
                               builder::ConverterPrototype> >& converter_prototypes,
                             const std::unordered_map<std::string, std::unique_ptr<
                               builder::TransmConvPrototype> >& transm_conv_prototypes,
                               const std::unordered_map<std::string, std::unique_ptr<
                                 builder::RegionPrototype> >& region_prototypes,
                             const std::unordered_map<std::string, std::unique_ptr<
                               builder::LinkPrototype> >& link_prototypes,
                             const std::unordered_map<std::string, std::unique_ptr<
                               builder::MultiConverterProto> >& multi_converter_prototypes,
							 const std::unordered_map<std::string, std::unique_ptr<
							   builder::GlobalProto> >& global_proto  ) {
  if (!region_prototypes.empty()) {
    for (const auto& it : region_prototypes)
      regions_.emplace(it.first, std::move(std::shared_ptr<Region>(new Region(*it.second, prim_energy_prototypes,
                                                                              converter_prototypes,
                                                                              storage_prototypes,
                                                                              multi_converter_prototypes))));
  }
  if (!link_prototypes.empty()) {
    for (const auto& it : link_prototypes)
      links_.emplace(it.first, std::move(std::shared_ptr<Link>(new Link(*it.second, transm_conv_prototypes,
                                                                        regions_))));
  }
  if (!global_proto.empty()) {
	  //DEBUG std::cout << "***DEBUG AbstractModel::AbstractModel C'Tor: found global prototype! " << std::endl;
	  for (const auto& it : global_proto)
		  global_.emplace(it.first, std::move(std::shared_ptr<Global>(new Global(*it.second,
				                                                                 prim_energy_prototypes,
				                                                                 storage_prototypes))));
  }
}

} /* namespace am */
