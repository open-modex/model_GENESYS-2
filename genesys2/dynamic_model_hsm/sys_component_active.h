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
// sys_component_active.h
//
// This file is part of the genesys-framework v.2

#ifndef DYNAMIC_MODEL_HSM_SYS_COMPONENT_ACTIVE_H_
#define DYNAMIC_MODEL_HSM_SYS_COMPONENT_ACTIVE_H_

#include <auxiliaries/time_series_const_addable.h>
#include <static_model/sys_component_active.h>
#include <auxiliaries/functions.h>
#include <program_settings.h>

namespace dm_hsm {

class SysComponentActive : public virtual sm::SysComponentActive {
 public:
  SysComponentActive() = delete;
  virtual ~SysComponentActive() = default;
  SysComponentActive& operator=(const SysComponentActive&) = delete;
  SysComponentActive& operator=(SysComponentActive&&) = delete;

  /** \name Reset Functions.*/
  ///@{
    void resetCurrentTP(const aux::SimulationClock& clock);
    void set_annaul_lookups(const aux::SimulationClock& clock);
    void resetSequencedSysComponent();
  ///@}

  /** \name Hierarchical Operation Functions.*/
  ///@{
    double usable_capacity_el(const aux::SimulationClock& clock) const;
    double reserved_capacity_tp() const {return reserved_capacity_el_tp_;}
    bool useCapacity(double capacity_request,
                     const aux::SimulationClock& clock,
                     const bool charging = false);

    double usable_capacity_out_tp() const {return usable_capacity_tp();}
    double usable_capacity_out_tp(const aux::SimulationClock& clock, double reqInput) const;
    double usable_capacity_in_tp(const aux::SimulationClock& clock, double requestedOutput) const ;
  ///@}

  /** \name Cost calculation.*/
  ///@{
    void add_OaM_cost(aux::SimulationClock::time_point tp_now);
    void add_vopex(double value,
                   const aux::SimulationClock& clock);

    void calculated_annual_disc_capex(aux::SimulationClock::time_point start);
    double get_sum(std::string query,
                       aux::SimulationClock::time_point start,
                       aux::SimulationClock::time_point end,
                       aux::SimulationClock::duration interval = aux::years(1) ) const;
    double getDiscountedValue(std::string query,
                              aux::SimulationClock::time_point start,
                              aux::SimulationClock::time_point end,
                              const aux::SimulationClock::time_point present) const;
  ///@}

  /** \name Operation Status.*/
  ///@{
    void uncheck_active_current_year(){set_active_current_year(false);}
    bool active_current_year() const {return active_current_year_;}
  ///@}

  void print_current_capacities(const aux::SimulationClock& clock);

 protected:
  SysComponentActive(const SysComponentActive& other)
  : builder::SysCompActPrototype(other), // virtual inheritance and deleted default c'tor
    am::SysComponentActive(other), // virtual inheritance and deleted default c'tor
    sm::SysComponentActive(other),
    vopex_(other.vopex_),
    fopex_(other.fopex_),
    used_capacity_(other.used_capacity_),
    discounted_capex_(other.discounted_capex_),
    usable_capacity_el_tp_(0.),
    reserved_capacity_el_tp_(0.){}//DEBUG std::cout << "dm_hsm::SCA::CopyC'tor called for " << code() << std::endl;}
  SysComponentActive(SysComponentActive&&) = default;
  SysComponentActive(const sm::SysComponentActive& origin)
      : builder::SysCompActPrototype(origin), // virtual inheritance and deleted default c'tor
        am::SysComponentActive(origin), // virtual inheritance and deleted default c'tor
        sm::SysComponentActive(origin),
        active_current_year_(true),
        vopex_(),
        fopex_(),
        used_capacity_(),
        discounted_capex_(),
        usable_capacity_el_tp_(0.),
        reserved_capacity_el_tp_(0.) {}//DEBUGstd::cout << "dm_hsm::SCA::C'tor called for " << code() << std::endl;}

  void set_active_current_year(bool state) {active_current_year_ = state;}
  double used_capacity( aux::SimulationClock::time_point tp) const {return used_capacity_[tp];}
  void add_used_capacity(const aux::SimulationClock& clock, double value) {
              used_capacity_ += aux::TimeSeriesConst(std::vector<double>{value, 0.}, clock.now(), clock.tick_length());
              //std::cout << code() << " | " << aux::SimulationClock::time_point_to_string(clock.now()) << " used_capacity = " << value << std::endl;
  }
  void set_usable_capacity_tp(double usable_capacity) {usable_capacity_el_tp_ = usable_capacity;}
  void add_usable_capacity(double added_used_capcity, const aux::SimulationClock& clock) {
    if (0 > (added_used_capcity + usable_capacity_tp()) ) {
      std::cerr << "WARNING add_usable_capacity: resulting in negative capacity " << added_used_capcity  << " | usable_cap=" << usable_capacity_tp()<< std::endl;
    } else if (aux::cmp_equal(capacity(clock.now()), (added_used_capcity + usable_capacity_tp()), genesys::ProgramSettings::approx_epsilon())) {
      usable_capacity_el_tp_ = capacity(clock.now());
      return;
    }
    usable_capacity_el_tp_ += added_used_capcity;}
  //void set_reserved_capacity_tp(double reserved_capacity_tp); // {reserved_capacity_tp_ = reserved_capacity_tp;}
  bool set_reserve_capacity_tp(const aux::SimulationClock& clock,
                               double reserved_capacity_tp);
  //  void set_reserved_cap_input(const aux::SimulationClock::time_point tp,
  //                              double input);
  /** \name Accessors to analysis_hsm*/
    ///@{
    const aux::TimeSeriesConstAddable& get_fopex() const {return fopex_;}
    const aux::TimeSeriesConstAddable& get_vopex() const {return vopex_;}
    const aux::TimeSeriesConstAddable& get_used_capacity() const {return used_capacity_;}
    const aux::TimeSeriesConstAddable& get_discounted_capex() const { return discounted_capex_;}
    ///@}

 private:
  void add_vopex_zero(aux::SimulationClock::time_point tp_now, aux::SimulationClock::duration tick_length) {
                      vopex_ += aux::TimeSeriesConst(std::vector<double>{1.,0.}, tp_now, tick_length); }
  double usable_capacity_tp() const ;//{return usable_capacity_tp_;}
  bool active_current_year_ = true;
  aux::TimeSeriesConstAddable vopex_;
  aux::TimeSeriesConstAddable fopex_;
  aux::TimeSeriesConstAddable used_capacity_; ///stores the operation information in detail
  aux::TimeSeriesConstAddable discounted_capex_;
  double usable_capacity_el_tp_;  /// possible amount of deliverable capacity for current time point
  double reserved_capacity_el_tp_; /// stores amount of capacity which could be requested for transport to other region
};

} /* namespace dm_hsm */

#endif /* DYNAMIC_MODEL_HSM_SYS_COMPONENT_ACTIVE_H_ */
