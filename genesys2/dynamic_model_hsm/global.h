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
// global.h
//
// This file is part of the genesys-framework v.2

#ifndef DYNAMIC_MODEL_HSM_GLOBAL_H_
#define DYNAMIC_MODEL_HSM_GLOBAL_H_

#include <static_model/global.h>

#include <dynamic_model_hsm/primary_energy.h>
#include <dynamic_model_hsm/storage.h>

namespace dm_hsm {

class Global: public sm::Global {
public:
	Global() = delete;
  virtual ~Global() override = default;
  Global (const Global& other);
  Global(Global&& other) = delete; //  if this move c'tor is needed, special implementation is necessary, taking care of correct handling of the weak pointers of the converter
  Global& operator=(const Global&) = delete;
  Global& operator=(Global&&) = delete;
  Global(const sm::Global& origin);

  void resetCurrentTP(const aux::SimulationClock& clock);
  void set_annual_lookups(const aux::SimulationClock& clock);
  void resetSequencedGlobal(const aux::SimulationClock& clock);
  std::shared_ptr<PrimaryEnergy> getCO2ptr();

  const std::unordered_map<std::string, std::shared_ptr<Storage>>& storage_ptrs() const { return storage_ptrs_; }
  const std::unordered_map<std::string, std::shared_ptr<Storage> >& storage_ptrs_dm() const {
    return storage_ptrs_;}
  const std::unordered_map<std::string, std::shared_ptr<PrimaryEnergy> >& primary_energy_ptrs_dm() const {
    return primary_energy_ptrs_;}

private:
  std::unordered_map<std::string, std::shared_ptr<PrimaryEnergy> > primary_energy_ptrs_;
  std::unordered_map<std::string, std::shared_ptr<Storage> > storage_ptrs_;
};

} /* namespace dm_hsm */

#endif /* DYNAMIC_MODEL_HSM_GLOBAL_H_ */
