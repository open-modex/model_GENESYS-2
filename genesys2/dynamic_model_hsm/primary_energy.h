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

#ifndef DYNAMIC_MODEL_HSM_PRIMARY_ENERGY_H_
#define DYNAMIC_MODEL_HSM_PRIMARY_ENERGY_H_

#include <abstract_model/primary_energy.h>

namespace dm_hsm {

class PrimaryEnergy : public am::PrimaryEnergy {
 public:
  PrimaryEnergy() = delete;
  virtual ~PrimaryEnergy() override = default;
  //PrimaryEnergy(const PrimaryEnergy&) = default;
  PrimaryEnergy(const PrimaryEnergy& other)
              : am::PrimaryEnergy(other),
                connected_(false),
                num_connections_(0),
                annual_potential_(0.),
                annual_vopex_(0.),
                potential_current_tp_(other.potential_current_tp_),
                reserved_capacity_current_tp_(other.reserved_capacity_current_tp_),
                consumed_energy_(other.consumed_energy_),
                internal_vopex_(other.internal_vopex_),
                total_consumed_(other.total_consumed_){}
  PrimaryEnergy(PrimaryEnergy&&) = default;
  PrimaryEnergy& operator=(const PrimaryEnergy&) = delete;
  PrimaryEnergy& operator=(PrimaryEnergy&&) = delete;
  PrimaryEnergy(const am::PrimaryEnergy& origin)
      : am::PrimaryEnergy(origin),
        connected_(false),
        num_connections_(0),
        annual_potential_(0.),
        annual_vopex_(0.),
        potential_current_tp_(0.),
        reserved_capacity_current_tp_(0.),
        consumed_energy_(),
        internal_vopex_(),
        total_consumed_(0.){}
  /** \name Reset Functions.*/
  ///@{
  void resetCurrentTP(const aux::SimulationClock& clock);
  void set_annual_lookups(const aux::SimulationClock& clock);
  void resetSequencedPrimaryEnergy (const aux::SimulationClock& clock);
  ///@}

  /** \name Hierarchical Operation Functions.*/
  ///@{
  double get_potential_current_tp() { return potential_current_tp_;}
  bool reserveCapacity(const double request,
                       const aux::SimulationClock& clock);
  bool useCapacity(const double request,
                   const aux::SimulationClock& clock);
  double get_vopex (const aux::SimulationClock::time_point tp) const { return internal_vopex_[tp];}

  double get_specific_vopex(const aux::SimulationClock& clock) { return annual_vopex_;}
  ///@}

  double get_total_consumed_() const {return total_consumed_;}

  void print_current_capacities(const aux::SimulationClock&clock);

  bool connected() const {/* std::cout << "\t " << num_connections_ << " connected\n";*/
                          if (connected_) return true; else return false;}
  bool connect() {++num_connections_;return connected_ = true;}


 protected:
  const aux::TimeSeriesConstAddable& get_consumed_energy_() const { return consumed_energy_;}
  const aux::TimeSeriesConstAddable& get_vopex_() const { return internal_vopex_;}

 private:
  void add_internal_counters (double request,const aux::SimulationClock& clock);
  //  void add_internal_vopex (double request, const aux::SimulationClock& clock );
  //  void add_internal_vopex ( double request, aux::SimulationClock::time_point tp_now, const aux::SimulationClock::duration& interval_tp );
  //  void add_consumed_energy(double value, const aux::SimulationClock& clock);

  bool connected_;
  int num_connections_;
  double annual_potential_;
  double annual_vopex_;
  double potential_current_tp_;
  double reserved_capacity_current_tp_;
  aux::TimeSeriesConstAddable consumed_energy_;  //monitoring the used energy
  aux::TimeSeriesConstAddable internal_vopex_;
  double total_consumed_;
};

} /* namespace dm_hsm */

#endif /* DYNAMIC_MODEL_HSM_PRIMARY_ENERGY_H_ */
