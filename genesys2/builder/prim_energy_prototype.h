/// ==================================================================
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
// prim_energy_prototype.h
//
// This file is part of the genesys-framework v.2

#ifndef BUILDER_PRIM_ENERGY_PROTOTYPE_H_
#define BUILDER_PRIM_ENERGY_PROTOTYPE_H_

#include <builder/sys_component.h>

#include <auxiliaries/simulation_clock.h>
#include <auxiliaries/tbd_lookup_table.h>

#include <string>
#include <limits>

namespace builder {

class PrimEnergyPrototype : public SysComponent {
 public:
  PrimEnergyPrototype() = delete;
  virtual ~PrimEnergyPrototype() override = default;
  PrimEnergyPrototype(const PrimEnergyPrototype&) = default;
  PrimEnergyPrototype(PrimEnergyPrototype&&) = default;
  PrimEnergyPrototype& operator=(const PrimEnergyPrototype&) = delete;
  PrimEnergyPrototype& operator=(PrimEnergyPrototype&&) = delete;
  PrimEnergyPrototype(const std::string& code,
                      const std::string& name,
                      const aux::TBDLookupTable& cost)
      : SysComponent(code, name),
        cost_(cost) {}

 protected:
  double cost(aux::SimulationClock::time_point tp) const {return cost_[tp];}
  double cost(aux::SimulationClock::time_point tp,
              double query,
              double offset = 0.0) const {
	  double rval = cost_.lookup(tp, query, offset);
	  if(rval == -1) { //this is not a valid cost value! / last changed on 2019/10/18 from 0 to -1
		  return std::numeric_limits<double>::infinity();
	  }
	  return rval;}

 private:
  const aux::TBDLookupTable cost_;

};

} /* namespace builder */

#endif /* BUILDER_PRIM_ENERGY_PROTOTYPE_H_ */
