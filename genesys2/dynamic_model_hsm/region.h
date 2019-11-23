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
// This file is part of the genesys-framework v.2/*

#ifndef DYNAMIC_MODEL_HSM_REGION_H_
#define DYNAMIC_MODEL_HSM_REGION_H_

#include <map>
#include <memory>
#include <unordered_map>
#include <thread>

#include <auxiliaries/time_tools.h>
#include <dynamic_model_hsm/multi_converter.h>
#include <dynamic_model_hsm/converter.h>
#include <dynamic_model_hsm/link.h>
#include <dynamic_model_hsm/primary_energy.h>
#include <dynamic_model_hsm/storage.h>
#include <dynamic_model_hsm/transmission_converter.h>
#include <static_model/region.h>

namespace dm_hsm {

class Region : public sm::Region {
 public:
  Region() = delete;
  virtual ~Region() override = default;
  Region (const Region& other);
  Region(Region&& other) = delete; //  if this move c'tor is needed, special implementation is necessary, taking care of correct handling of the weak pointers of the converter
  Region& operator=(const Region&) = delete;
  Region& operator=(Region&&) = delete;
  Region(const sm::Region& origin);

  /** \name Main Model Interface*/
  ///@{
  void updateConverterHSMCategory(std::unordered_map<std::string, dm_hsm::HSMCategory>& keymap) const;
  void updateConverterHSMSubCategory(std::unordered_map<std::string, dm_hsm::HSMSubCategory>& subKeyMap) const;
  void autoSetHSMCategory();
  void connectReservoirs();
  void connectGlobalReservoirs(std::string output_type,std::shared_ptr<PrimaryEnergy> co2_ptr);
  //  	void connectReservoirs(bool local = true);
  void connectLink(std::weak_ptr<Link> link,
                   const std::unordered_map<std::string, std::shared_ptr<TransmissionConverter> >& converter_ptrs);
  void save_unsupplied_load(const aux::SimulationClock& clock) {
    remaining_residual_load_ += aux::TimeSeriesConst(std::vector<double>{residual_load_TP_, 0.}, clock.now(), clock.tick_length());
    remaining_residual_heat_load_ += aux::TimeSeriesConst(std::vector<double>{residual_heat_TP_, 0.}, clock.now(), clock.tick_length());
  }
    //return (residual_load_TP_ > 0.0+genesys::ProgramSettings::approx_epsilon() ? residual_load_TP_ : 0);}
  ///@}
  /** \name HSM Operation Interface*/
  ///@{
  void calculateResidualLoad(dm_hsm::HSMCategory category,
                             aux::SimulationClock::time_point tp_start_seq,
                             aux::SimulationClock::time_point tp_end_seq,
                             aux::SimulationClock::duration tick_length);
  void init_heat_load(aux::SimulationClock::time_point tp_start_seq,
                      aux::SimulationClock::time_point tp_end_seq,
                      aux::SimulationClock::duration tick_length);
  void resetSequencedRegion(const aux::SimulationClock& clock);
  void resetCurrentTP(const aux::SimulationClock& clock);
  void set_annual_lookups(const aux::SimulationClock& clock);
  void set_annual_unsupplied(double unsupplied_el_wh, aux::SimulationClock::time_point tp);
  void decommission_plants();
  void uncheck_active_current_year();
  void add_OaM_cost(aux::SimulationClock::time_point tp_now);
  void activate_mustrun(const aux::SimulationClock& clock);
  void generalised_balance_local(const dm_hsm::HSMCategory& category,
                                 const aux::SimulationClock& clock);
  /** \name GridBalance Interface Start in Demand Region X */
  ///@{
  void balance_start(int hops,
               const dm_hsm::HSMCategory& hsm_cat,
               const aux::SimulationClock& clock);
  ///@}
  /** \name GridBalance Interface to Request */
  ///@{
  double getExportCapacity(const dm_hsm::HSMCategory cat,
                           const aux::SimulationClock& clock,
                           const double transmission_limit);
  bool useExportCapacity(const dm_hsm::HSMCategory cat,
                         const double export_request,
                         const aux::SimulationClock& clock,
                         const std::weak_ptr<Link>& active_Link);
  ///@}
  void setTransferStoredEnergy(const aux::SimulationClock& clock);
  //  double getDiscountedValue(std::string query,
  //                            aux::SimulationClock::time_point start,
  //                            aux::SimulationClock::time_point end) const;
  double getDiscountedValue(std::string query,
                            aux::SimulationClock::time_point start,
                            aux::SimulationClock::time_point end,
                            const aux::SimulationClock::time_point discount_present =
                                               aux::SimulationClock::time_point_from_string("1970-01-01_00:00")) const;
  double get_sum(std::string query,
                     aux::SimulationClock::time_point start,
                     aux::SimulationClock::time_point end) const;

  void calculated_annual_disc_capex(aux::SimulationClock::time_point start);
  double calc_SelfSupplyQuota(const aux::SimulationClock& clock);

  ///DEBUG functions:
  void print_current_capacities(const aux::SimulationClock& clock);

  /*
   * Get-Functions for copy-C'tor access in lower model level
   */
//  const aux::TimeSeriesConstAddable& get_annual_capex_() const { return annual_capex_;}
  const aux::TimeSeriesConstAddable& get_annual_co2_emissions_() const { return annual_co2_emissions_;}
  const aux::TimeSeriesConstAddable& get_annual_consumed_heat_GWh_() const { return annual_consumed_heat_GWh_;}
  const aux::TimeSeriesConstAddable& get_annual_generated_heat_GWh_() const { return annual_generated_heat_GWh_;}
  const aux::TimeSeriesConstAddable& get_annual_imported_electricity_GWh_ () const { return annual_imported_electricity_GWh_;}
  const aux::TimeSeriesConstAddable& get_annual_imported_gas_GWh_() const { return annual_imported_gas_GWh_;}
  const aux::TimeSeriesConstAddable& get_annual_exported_electricity_GWh_() const { return annual_exported_electricity_GWh_;}
  const aux::TimeSeriesConstAddable& get_annual_annual_exported_gas_GWh_() const { return annual_exported_gas_GWh_;}
  const aux::TimeSeriesConstAddable& get_co2_emissions_() const { return co2_emissions_;}
  const aux::TimeSeriesConstAddable& get_exported_energy_() const { return exported_energy_;}
  const aux::TimeSeriesConstAddable& get_exported_electricity_() const { return exported_electricity_;}
  const aux::TimeSeriesConstAddable& get_exported_gas_() const { return exported_gas_;}
  const aux::TimeSeriesConstAddable& get_imported_energy_() const { return imported_energy_;}
  const aux::TimeSeriesConstAddable& get_imported_electricity_() const { return imported_electricity_;}
  const aux::TimeSeriesConstAddable& get_imported_gas_() const { return imported_gas_;}
  const aux::TimeSeriesConstAddable& get_import_for_local_balance_() const { return import_for_local_balance_;}

  const aux::TimeSeriesConstAddable get_load() const;
  const aux::TimeSeriesConstAddable& get_residual_load_() const { return  residual_load_;}
  const aux::TimeSeriesConstAddable& get_residual_heat_load_() const { return  residual_heat_;}
  const aux::TimeSeriesConstAddable& get_remaining_residual_load_() const { return  remaining_residual_load_;}
  const aux::TimeSeriesConstAddable& get_remaining_residual_heat_load_() const { return  remaining_residual_heat_load_;}



 protected:
  const std::unordered_map<std::string, std::shared_ptr<Converter> >& converter_ptrs_dm() const {
    return converter_ptrs_;}
  const std::unordered_map<std::string, std::shared_ptr<Storage> >& storage_ptrs_dm() const {
    return storage_ptrs_;}
  const std::unordered_map<std::string, std::shared_ptr<PrimaryEnergy> >& primary_energy_ptrs_dm() const {
    return primary_energy_ptrs_;}

 private:
  template <typename T>
  std::shared_ptr<T> get_pointer_to(const std::unordered_map<std::string, std::shared_ptr<T> >& ptr_map,
                                    const std::string& query);

  std::multimap<double,
                std::weak_ptr<dm_hsm::Converter>,
                std::greater<double> >collectConverter(dm_hsm::HSMCategory cat,
                                                       const aux::SimulationClock& clock);
  std::multimap<double,
                std::weak_ptr<dm_hsm::Storage>,
                std::greater<double> >collectStorages(dm_hsm::HSMCategory cat,
                                                      const aux::SimulationClock& clock);
  /** \name GridBalance Private Functions */
  ///@{
  double balance(int hops,
                 const double balance_demand,
                 const dm_hsm::HSMCategory& cat,
                 const aux::SimulationClock& clock,
                 const std::weak_ptr<Link>& hotlink = std::weak_ptr<Link>()  );

  std::map<double, std::pair<std::weak_ptr<Link>, std::weak_ptr<TransmissionConverter>>,
                             std::greater<double> > createTempLinkMap(const std::weak_ptr<Link> hotlink,
                                                                        const aux::SimulationClock& clock);
  ///@}

  /** \name LocalBalance Private Functions */
  ///@{
  void converter_balance(const dm_hsm::HSMCategory& category,
                         const aux::SimulationClock& clock);
  void storage_charging(const dm_hsm::HSMCategory& category,
                       const aux::SimulationClock& clock);
  ///@}

  std::tuple<std::vector<double>, aux::SimulationClock::time_point> calc_energy_vector(aux::SimulationClock::time_point start,
                                                                                       aux::SimulationClock::time_point end) const;
  double collect_generation(const aux::SimulationClock::time_point start,
                            const aux::SimulationClock::time_point end,
                            const aux::SimulationClock::duration interval);

  void connect_multi_converter(std::pair<std::string, std::shared_ptr<Converter>>);
  void connect_converter(std::pair<std::string, std::shared_ptr<Converter>> conv);
  void manual_connect_conv(std::pair<std::string, std::shared_ptr<Converter>> conv);


  void discharge_heat_storage(const dm_hsm::HSMCategory& category,
                              const aux::SimulationClock& clock);

  std::unordered_map<std::string, std::shared_ptr<PrimaryEnergy> > primary_energy_ptrs_;
  std::unordered_map<std::string, std::shared_ptr<Converter> > converter_ptrs_;
  std::unordered_map<std::string, std::shared_ptr<Storage> > storage_ptrs_;
  std::unordered_map<std::string, std::pair<std::weak_ptr<Link>, std::weak_ptr<TransmissionConverter> > > line_map_el_;
  std::unordered_map<std::string, std::pair<std::weak_ptr<Link>, std::weak_ptr<TransmissionConverter> > > line_map_ch4_;
//  std::map<double, std::weak_ptr<Link>> map_indirect_imports_;
//  std::map<double, std::pair<std::weak_ptr<Link>, std::weak_ptr<TransmissionConverter> > > map_indirect_imports1_;
  std::map<double, std::weak_ptr<Converter>> map_reserved_exports_tp_;
  ///only annual values
      //cost related
//  aux::TimeSeriesConstAddable annual_capex_;
  aux::TimeSeriesConstAddable annual_co2_emissions_;
  aux::TimeSeriesConstAddable annual_consumed_heat_GWh_;
  aux::TimeSeriesConstAddable annual_generated_heat_GWh_;
      //energy related
  aux::TimeSeriesConstAddable annual_imported_electricity_GWh_;
  aux::TimeSeriesConstAddable annual_imported_gas_GWh_;
  aux::TimeSeriesConstAddable annual_exported_electricity_GWh_;
  aux::TimeSeriesConstAddable annual_exported_gas_GWh_;
  aux::TimeSeriesConstAddable annual_selfsupply_quota_;
  aux::TimeSeriesConstAddable annual_unsupplied_electricity_; //GWh
  //aux::TimeSeriesConstAddable annual_max_demand_;
  ///highly dynamic values
           //should be resetSequence()
  aux::TimeSeriesConstAddable residual_load_;         ///< Stores the amount of local residual electricity load (unmet demand > 0).
  aux::TimeSeriesConstAddable residual_heat_;
  aux::TimeSeriesConstAddable remaining_residual_load_;
  aux::TimeSeriesConstAddable remaining_residual_heat_load_;
  aux::TimeSeriesConstAddable remaining_excess_heat_;
  aux::TimeSeriesConstAddable co2_emissions_;         ///< Stores annual values of co2 Emissions in the region
  aux::TimeSeriesConstAddable exported_energy_;
  aux::TimeSeriesConstAddable exported_electricity_;
  aux::TimeSeriesConstAddable exported_gas_;
  aux::TimeSeriesConstAddable imported_energy_;
  aux::TimeSeriesConstAddable imported_electricity_;
  aux::TimeSeriesConstAddable imported_gas_;
  aux::TimeSeriesConstAddable import_for_local_balance_;
  bool RL_init_;
  bool HEAT_init_;
  bool connected_to_grid_;
  bool supply_region_;
  bool balanced_;
  double residual_load_TP_;
  double residual_heat_TP_;
  double reserved_residual_load_tp_;
  double available_power_for_export_;
  double max_pwr_exchange_grid_tp_;
  //double max_demand_current_year_;

};

} /* namespace dm_hsm */

#endif /* DYNAMIC_MODEL_HSM_REGION_H_ */
