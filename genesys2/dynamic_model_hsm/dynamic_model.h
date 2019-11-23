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
// dynamic_model.h
//
// This file is part of the genesys-framework v.2

#ifndef DYNAMIC_MODEL_HSM_DYNAMIC_MODEL_H_
#define DYNAMIC_MODEL_HSM_DYNAMIC_MODEL_H_

#include <static_model/static_model.h>
#include <dynamic_model_hsm/link.h>
#include <dynamic_model_hsm/region.h>
#include <dynamic_model_hsm/global.h>

namespace dm_hsm {

class DynamicModel : public sm::StaticModel {
 public:
  DynamicModel() = delete;
  virtual ~DynamicModel() = default;
  DynamicModel(const DynamicModel& other);
  DynamicModel(DynamicModel&&) = default;
  DynamicModel& operator=(const DynamicModel&) = delete;
  DynamicModel& operator=(DynamicModel&&) = delete;
  DynamicModel(const StaticModel& origin);

  double getDiscountedValue(std::string,
                     aux::SimulationClock::time_point start,
                     aux::SimulationClock::time_point end,
                     const aux::SimulationClock::time_point discount_present =
                                                                    aux::SimulationClock::time_point_from_string("1970-01-01_00:00")) const;
  double getSumValue(std::string,
                     aux::SimulationClock::time_point start,
                     aux::SimulationClock::time_point end) const;
  double calc_annual_selfsupply(const aux::SimulationClock& clock);

  void calculate_annual_electricity_prices(aux::SimulationClock::time_point start,
                                           aux::SimulationClock::time_point end);

  void calculate_annual_disc_capex(aux::SimulationClock::time_point start,
                              aux::SimulationClock::time_point end);

  void setTransferStoredEnergy(const aux::SimulationClock& clock);
  void CalculateResidualLoad(dm_hsm::HSMCategory hsm_cat,
                             aux::SimulationClock::time_point tp_start,
                             aux::SimulationClock::time_point tp_end_seq,
                             aux::SimulationClock::duration tick_length);
  void resetSequencedModel(const aux::SimulationClock& clock);
  void resetCurrentTP(const aux::SimulationClock& clock);
  //  void transfer_persisting_data(const aux::SimulationClock& clock);
  void decommission_plants();
  void uncheck_active_current_year();
  void add_OaM_cost(aux::SimulationClock::time_point tp_now);
  void activate_mustrun(const aux::SimulationClock& clock);
  void generalised_balance(bool local,
                           const dm_hsm::HSMCategory& cat,
                           const aux::SimulationClock& clock);
  void save_unsupplied_load(const aux::SimulationClock& clock);
  double calculate_unsupplied_load();
  //double get_selfsupply_ratio(const aux::SimulationClock& clock);
  void print_current_model_capacities(const aux::SimulationClock& clock);
  const aux::TimeSeriesConstAddable& get_annual_electricity_price_() const { return annual_electricity_price_;}
  const aux::TimeSeriesConstAddable& get_annual_unsupplied_total_() const { return annual_unsupplied_total_;}
  void set_annual_lookups(const aux::SimulationClock& clock);

  bool add_annual_unsupplied_total_(const aux::SimulationClock& clock){return false;};

  protected:
    const std::unordered_map<std::string, std::shared_ptr<Region> >& regions() const {return regions_;}
    const std::unordered_map<std::string, std::shared_ptr<Link> >& links() const {return links_;}
    const std::unordered_map<std::string, std::shared_ptr<Global> >& global() const {return global_;}

   private:
    void setHSMCategoriesFromCode();
    std::vector<std::string> region_codes_;
    std::vector<std::string> link_codes_;
    std::unordered_map<std::string, std::shared_ptr<Region> > regions_;
    std::unordered_map<std::string, std::shared_ptr<Link> > links_;
    std::unordered_map<std::string, std::shared_ptr<Global> > global_;

    aux::TimeSeriesConstAddable annual_electricity_price_;
    aux::TimeSeriesConstAddable annual_unsupplied_total_;
};

} /* namespace dm_hsm */

#endif /* DYNAMIC_MODEL_HSM_DYNAMIC_MODEL_H_ */
