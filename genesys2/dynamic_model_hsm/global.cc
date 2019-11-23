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
#include <dynamic_model_hsm/global.h>

namespace dm_hsm {


Global::Global(const sm::Global& origin)
    : sm::Global(origin){
  //DEBUG std::cout << "\t***DEBUG: dm::Global::Global ctor" << std::endl;
  if(!primary_energy_ptrs().empty()) {
    for (const auto& it : primary_energy_ptrs()) {
       //DEBUG std::cout << "\t***dm_hsm::Global-CTor found global primary energy in upper model level" << std::endl;
       primary_energy_ptrs_.emplace(it.first, std::shared_ptr<PrimaryEnergy>(new PrimaryEnergy(*it.second)));
    }
  } else {
    //std::cout << "\t***DEBUG: dm::Global::Global ctor - found no global primary energy!" << std::endl;
  }


  if(!storage_ptrs_sm().empty()) {
    for (const auto& it : storage_ptrs_sm()){
      //DEBUG std::cout << "Global-CTor found global storage in upper model level" << std::endl;
       storage_ptrs_.emplace(it.first, std::shared_ptr<Storage>(new Storage(*it.second)));
    }
  } else {
    //std::cout << "\t***DEBUG: dm::Global::Global ctor - found no global storage!" << std::endl;
  }
}

Global::Global(const Global& other)
    : sm::Global(other) {
  //std::cout << "\t***DEBUG: dm::Global::Global CopyC-Tor " << std::endl;
  if (!other.storage_ptrs_.empty()) {
    //std::cout << "copy dm_hsm found storage: " << other.storage_ptrs_.size() << std::endl;
    for (const auto &it : other.storage_ptrs_)
      storage_ptrs_.emplace(it.first, std::shared_ptr<Storage>(new Storage(*it.second)));
  }
  if (!other.primary_energy_ptrs_.empty()) {
    //std::cout << "copy dm_hsm found primary energies: " << other.primary_energy_ptrs_.size() << std::endl;
    for (const auto &it : other.primary_energy_ptrs_)
      primary_energy_ptrs_.emplace(it.first, std::shared_ptr<PrimaryEnergy>(new PrimaryEnergy(*it.second)));
  }
}

std::shared_ptr<PrimaryEnergy> Global::getCO2ptr() {
	if(!primary_energy_ptrs_.empty()) {
		if (primary_energy_ptrs_.find("CO2") != primary_energy_ptrs_.end()) {
			return primary_energy_ptrs_.find("CO2")->second;
		}
		else {
			std::cerr << "ERROR in dm::Global::getCO2ptr - could not find Co2 ptr" << std::endl;
			std::terminate();
		}

	}
	return std::shared_ptr<PrimaryEnergy> ();
}
void Global::resetCurrentTP(const aux::SimulationClock& clock) {
	  if(!primary_energy_ptrs_.empty()) {
	    for (auto&& it : primary_energy_ptrs_) {
	      it.second->resetCurrentTP(clock);
	    }
	  }
	  if(!storage_ptrs_.empty()){
      for( auto&&it : storage_ptrs_){
         it.second->ResetStorageCurrentTP(clock);
      }
	  }
}

void Global::set_annual_lookups(const aux::SimulationClock& clock) {
	if(!primary_energy_ptrs_.empty()) {
	    for (auto& it : primary_energy_ptrs_) {
	      it.second->set_annual_lookups(clock);
	    }
    }

	if(!storage_ptrs_.empty()) {
		for(const auto& it : storage_ptrs_){
			it.second->set_annaul_lookups(clock);

		}
	}

}

void Global::resetSequencedGlobal(const aux::SimulationClock& clock) {
  if (!storage_ptrs_.empty()) {
    //reset all storages
    for (auto &it : storage_ptrs_) {
       it.second->ResetSequencedStorage(clock);
    }
  }
  if(!primary_energy_ptrs_.empty()) {
    for (auto &it : primary_energy_ptrs_) {
      it.second->resetSequencedPrimaryEnergy(clock);
    }
  }
}

} /* namespace dm_hsm */
