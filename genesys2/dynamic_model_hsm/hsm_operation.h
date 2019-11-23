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
// hsm_operation.h
//
// This file is part of the genesys-framework v.2

#ifndef DYNAMIC_MODEL_HSM_HSM_OPERATION_H_
#define DYNAMIC_MODEL_HSM_HSM_OPERATION_H_

#include <tuple>

#include <dynamic_model_hsm/dynamic_model.h>
#include <dynamic_model_hsm/hsm_category.h>
#include <static_model/static_model.h>
#include <io_routines/csv_file.h>

namespace dm_hsm {

class HSMOperation {
 public:
  HSMOperation() = delete;
  virtual ~HSMOperation() = default;
  HSMOperation(const sm::StaticModel& model);

  //std::unordered_map<std::string, double > CalculateFitness(bool analyse);
  std::unordered_map<std::string, double > CalculateFitnessMinCost(bool analyse);
  std::unordered_map<std::string, double > CalculateFitnessMinLCOE(bool analyse);

 protected:
  const DynamicModel& model() const {return model_;}
  double fitness() {return fitness_;}

 private:
  /** \name Sequencer Functions.*/
  ///@{
  void startSequencer();
  void solveSequence(aux::SimulationClock::time_point start_year_sequence,
                     aux::SimulationClock::duration duration); /// Solves one sequence of the System for each hour independently.
  ///@}

  /** \name Reset Functions.*/
  ///@{
  void resetSequencedModel(const aux::SimulationClock& sim_clock);
  void resetCurrentTP(const aux::SimulationClock& clock);
//  void transfer_persisting_data(const aux::SimulationClock& clock);
  //TODO FORESIGHT  void resetFuture(int lenghtforesight);
  //TODO FORESIGHT  void updateFuture(aux::SimulationClock::time_point tp_start);
  ///@}

  void decommission_plants();
  void uncheck_active_current_year();
  void calc_self_supply_quota_and_apply_penalties(const aux::SimulationClock& clock);

  void calculate_sum_values( const aux::SimulationClock::time_point tp_start_seq,
                             const aux::SimulationClock::time_point tp_end_seq);
  /** \name Hierarchical Operation Functions.*/
  ///@{
  void calculateResidualLoadTS(dm_hsm::HSMCategory cat_with_timeseries,
                               aux::SimulationClock::time_point tp_now,
                               aux::SimulationClock::time_point tp_end_seq,
                               aux::SimulationClock::duration tick_length_minutes);
  void setTransferStoredEnergy(const aux::SimulationClock& clock);
  //  void balance_local_storage(dm_hsm::HSMCategory cat,
  //                             const aux::SimulationClock& clock);
  void generalised_balance(bool local,
                           const dm_hsm::HSMCategory& cat,
                           const aux::SimulationClock& clock);
  void add_OaM_cost(aux::SimulationClock::time_point tp_now);
  void set_annual_lookups(const aux::SimulationClock& clock);
  void save_unsupplied_load(const aux::SimulationClock& clock);
  void add_penalties_selfsupply_quota_(const aux::SimulationClock& clock);
  void add_energy_unsupplied_(const aux::SimulationClock& clock);

  std::tuple<std::pair<double,double> , std::pair<double, double> , std::pair<double, double>>calculate_penalties(double realCost, double Energy);
  ///@}

  /** \name Debug Functions.*/
  ///@{
  void print_current_model_capacities(const aux::SimulationClock& clock);
  ///@}

  dm_hsm::DynamicModel model_;
  aux::SimulationClock::time_point tp_start_operation_;
  aux::SimulationClock::time_point tp_end_operation_;
  aux::SimulationClock::duration duration_operation_sequence_;
  int num_operation_sequence_iterations_;
  aux::SimulationClock::duration duration_last_operation_sequence_;
  int future_lookahead_time_;
  double accumulated_penalties_unsupplied_load_;
  double accumulated_penalties_selfsupply_quota_;
  double fitness_;
};

} /* namespace dm_hsm */

#endif /* DYNAMIC_MODEL_HSM_HSM_OPERATION_H_ */

