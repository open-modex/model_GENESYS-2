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
// primary_energy.h
//
// This file is part of the genesys-framework v.2

#ifndef ABSTRACT_MODEL_PRIMARY_ENERGY_H_
#define ABSTRACT_MODEL_PRIMARY_ENERGY_H_

#include <auxiliaries/time_tools.h>
#include <memory>

#include <builder/prim_energy_prototype.h>

namespace am {

class PrimaryEnergy : public builder::PrimEnergyPrototype {
 public:
  PrimaryEnergy() = delete;
  virtual ~PrimaryEnergy() override = default;
  PrimaryEnergy(const PrimaryEnergy&) = default;
  PrimaryEnergy(PrimaryEnergy&&) = default;
  PrimaryEnergy& operator=(const PrimaryEnergy&) = delete;
  PrimaryEnergy& operator=(PrimaryEnergy&&) = delete;
  PrimaryEnergy(const PrimEnergyPrototype& origin,
                const aux::TBDLookupTable& potential)
      : PrimEnergyPrototype(origin),
        potential_(potential) {}

  double potential(aux::SimulationClock::time_point tp) const {return potential_[tp];}
  double potential(aux::SimulationClock::time_point tp,
                   double query,
                   double offset = 0.0) const {return potential_.lookup(tp, query, offset);}

 private:
  const aux::TBDLookupTable potential_;
};

} /* namespace am */

#endif /* ABSTRACT_MODEL_PRIMARY_ENERGY_H_ */
