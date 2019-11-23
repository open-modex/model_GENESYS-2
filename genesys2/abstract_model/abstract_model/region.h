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
// region.h
//
// This file is part of the genesys-framework v.2

#ifndef ABSTRACT_MODEL_REGION_H_
#define ABSTRACT_MODEL_REGION_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include <abstract_model/multi_converter.h>
#include <abstract_model/converter.h>
#include <abstract_model/primary_energy.h>
#include <abstract_model/sys_component_active.h>
#include <builder/multi_converter_proto.h>
#include <builder/converter_prototype.h>
#include <builder/prim_energy_prototype.h>
#include <builder/sys_comp_act_prototype.h>

#include <builder/region_prototype.h>

namespace am {

class Region : public builder::RegionPrototype {
 public:
  Region() = delete;
  virtual ~Region() override = default;
  Region(const Region& other);
  Region(Region&& other) = default;
  Region& operator=(const Region&) = delete;
  Region& operator=(Region&&) = delete;
  Region(const builder::RegionPrototype& origin,
         const std::unordered_map<std::string, std::unique_ptr<builder::PrimEnergyPrototype> >& primary_energy,
         const std::unordered_map<std::string, std::unique_ptr<builder::ConverterPrototype> >& converter,
         const std::unordered_map<std::string, std::unique_ptr<builder::SysCompActPrototype> >& storage,
         const std::unordered_map<std::string, std::unique_ptr<builder::MultiConverterProto> >& multi_converter = {});

  void print_detail() const;

 protected:
  const std::unordered_map<std::string, std::shared_ptr<PrimaryEnergy> >& primary_energy_ptrs() {
    return primary_energy_ptrs_;
  }
  const std::unordered_map<std::string, std::shared_ptr<Converter> >& converter_ptrs() const {return converter_ptrs_;}
  const std::unordered_map<std::string, std::shared_ptr<MultiConverter> >& multi_converter_ptrs() const {
    return multi_converter_ptrs_;}
  const std::unordered_map<std::string, std::shared_ptr<SysComponentActive> >& storage_ptrs() const {
    return storage_ptrs_;
  }

 private:
  std::unordered_map<std::string, std::shared_ptr<PrimaryEnergy> > primary_energy_ptrs_;
  std::unordered_map<std::string, std::shared_ptr<Converter> > converter_ptrs_;
  std::unordered_map<std::string, std::shared_ptr<MultiConverter> > multi_converter_ptrs_;
  std::unordered_map<std::string, std::shared_ptr<SysComponentActive> > storage_ptrs_;
};

} /* namespace am */

#endif /* ABSTRACT_MODEL_REGION_H_ */
