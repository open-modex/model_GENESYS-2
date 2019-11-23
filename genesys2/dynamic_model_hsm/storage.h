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
// storage.h
//
// This file is part of the genesys-framework v.2

#ifndef DYNAMIC_MODEL_HSM_STORAGE_H_
#define DYNAMIC_MODEL_HSM_STORAGE_H_

#include <utility>

#include <auxiliaries/time_series_const_addable.h>
#include <dynamic_model_hsm/sys_component_active.h>

namespace dm_hsm {

class Storage : public virtual SysComponentActive {
 public:
  Storage() = delete;
  virtual ~Storage() = default;
  Storage(const Storage& other)
      : builder::SysCompActPrototype(other), // virtual inheritance and deleted default c'tor
        am::SysComponentActive(other), // virtual inheritance and deleted default c'tor
        sm::SysComponentActive(other), // virtual inheritance and deleted default c'tor
        SysComponentActive(other),
        charged_energy_(other.charged_energy_),
        energy_lost_by_transfer_(other.energy_lost_by_transfer_) {}
  Storage(Storage&&) = default;
  Storage& operator=(const Storage&) = delete;
  Storage& operator=(Storage&&) = delete;
  Storage(const sm::SysComponentActive& origin)
      : builder::SysCompActPrototype(origin), // virtual inheritance and deleted default c'tor
        am::SysComponentActive(origin), // virtual inheritance and deleted default c'tor
        sm::SysComponentActive(origin), // virtual inheritance and deleted default c'tor
        SysComponentActive(origin),
        initial_SOC_(0.),
        stored_energy_transfer_(-1.),
        charged_energy_(),
        energy_lost_by_transfer_() {}

  /** \name Reset interface.*/
    ///@{
  void ResetSequencedStorage(const aux::SimulationClock& clock); ///< Reset for new sequence
  void ResetStorageCurrentTP(const aux::SimulationClock& clock); ///< Reset for new tp
  void setTransferStoredEnergy(const aux::SimulationClock& clock);
  ///@}

  /** \name Converter interface.*/
    ///@{
    double getCapacityCharge (const double InputEnergyRequest, const aux::SimulationClock& clock) const;
    double getCapacityDischarge (const double OutputEnergyRequest, const aux::SimulationClock& clock ) const ;
    bool reserveCapacityDischarge(double OutputEnergyRequest,
                                  const aux::SimulationClock& clock); ///<Reserves capacity for useCapacityDischarge
    bool reserveCapacityCharge(const aux::SimulationClock& clock,///<Reserves capacity for  useCapacityCharge
                               double energyInput);
    ///Use the storage for discharge of energy
    /** Returns true if discharge succeeds
      \param [in] energyOutput - the amount of energy to be discharged
      \param [in] clock - the simulation clock which holds the current time of now()
      \param [out] bool - is true in case the discharge succeeds
     */
    bool useCapacityDischarge(const double energyOutput,
                              const aux::SimulationClock& clock);
    ///Stores energy in the storage object
    /** Returns true if storing succeeds
      \param [in] energyInput - the amount of energy to be stored
      \param [in] clock - the simulation clock which holds the current time of now()
      \param [out] bool - is true in case the storing succeeds
     */
    bool useCapacityCharge(double inputEnergyRated, const aux::SimulationClock& clock);
  ///@}
  /** \name DEBUG interface.*/
    ///@{
  void print_current_capacities(const aux::SimulationClock& clock);
  ///@}

  bool connected() const { if (connected_.first && connected_.second) return true; else return false;}
  void connect(bool bi = false) {
      if (bi){
        connected_.first = true,
        connected_.second = true;
      }else {if(connected_.first) connected_.second = true; else connected_.first = true;}
      //std::cout << "DEBUG\t\t\t\t\tsuccess storage " << code()<< std::endl;
    }


 protected:
  /** \name Accessors to analysis_hsm*/
    ///@{
    const aux::TimeSeriesConstAddable& get_charged_energy() const { return charged_energy_;}
    const aux::TimeSeriesConstAddable& get_energy_lost_by_transfer() const { return energy_lost_by_transfer_;}
    ///@}

 private:
  double calculate_soc(const aux::SimulationClock& clock);
  std::pair <bool, bool> connected_;
  double initial_SOC_ = 0.0;
  double stored_energy_transfer_ = -1.0; //energy transfer between sequences
  aux::TimeSeriesConstAddable charged_energy_;  //monitoring the state_of_charge
  aux::TimeSeriesConstAddable energy_lost_by_transfer_;//not used atm
};

} /* namespace dm_hsm */

#endif /* DYNAMIC_MODEL_HSM_STORAGE_H_ */
