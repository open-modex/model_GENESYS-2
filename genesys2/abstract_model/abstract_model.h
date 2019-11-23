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
// abstract_model.h
//
// This file is part of the genesys-framework v.2

#ifndef ABSTRACT_MODEL_ABSTRACT_MODEL_H_
#define ABSTRACT_MODEL_ABSTRACT_MODEL_H_

#include <string>
#include <unordered_map>

#include <abstract_model/link.h>
#include <abstract_model/region.h>
#include <abstract_model/global.h>
#include <builder/converter_prototype.h>
#include <builder/link_prototype.h>
#include <builder/prim_energy_prototype.h>
#include <builder/region_prototype.h>
#include <builder/sys_comp_act_prototype.h>
#include <builder/transm_conv_prototype.h>
#include <builder/global_proto.h>

namespace am {

class AbstractModel {
 public:
  AbstractModel() = delete;
  virtual ~AbstractModel() = default;
  AbstractModel(const AbstractModel& other);
  AbstractModel(AbstractModel&&) = default;
  AbstractModel& operator=(const AbstractModel&) = delete;
  AbstractModel& operator=(AbstractModel&&) = delete;
  AbstractModel(const std::unordered_map<std::string,
                                         std::unique_ptr<builder::PrimEnergyPrototype> >& prim_energy_prototypes,
                const std::unordered_map<std::string,
                                         std::unique_ptr<builder::SysCompActPrototype> >& storage_prototypes,
                const std::unordered_map<std::string,
                                         std::unique_ptr<builder::ConverterPrototype> >& converter_prototypes,
                const std::unordered_map<std::string,
                                         std::unique_ptr<builder::TransmConvPrototype> >& transm_conv_prototypes,
                const std::unordered_map<std::string, std::unique_ptr<builder::RegionPrototype> >& region_prototypes,
                const std::unordered_map<std::string, std::unique_ptr<builder::LinkPrototype> >& link_prototypes,
                const std::unordered_map<std::string, std::unique_ptr<
                                                     builder::MultiConverterProto> >& multi_converter_prototypes = {},
				const std::unordered_map<std::string, std::unique_ptr<builder::GlobalProto> >& global_proto = {});

 protected:
  const std::unordered_map<std::string, std::shared_ptr<Region> >& regions() const {return regions_;}
  const std::unordered_map<std::string, std::shared_ptr<Link> >&   links()   const {return links_;}
  const std::unordered_map<std::string, std::shared_ptr<Global> >& global()  const {return global_;}

 private:
  std::unordered_map<std::string, std::shared_ptr<Region> > regions_;
  std::unordered_map<std::string, std::shared_ptr<Link> >   links_;
  std::unordered_map<std::string, std::shared_ptr<Global> > global_;
};

} /* namespace am */

#endif /* ABSTRACT_MODEL_ABSTRACT_MODEL_H_ */
