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
// transmission_converter.h
//
// This file is part of the genesys-framework v.2

#ifndef DYNAMIC_MODEL_HSM_TRANSMISSION_CONVERTER_H_
#define DYNAMIC_MODEL_HSM_TRANSMISSION_CONVERTER_H_

#include <memory>
#include <string>

#include <auxiliaries/simulation_clock.h>
#include <auxiliaries/time_series_const_addable.h>
#include <auxiliaries/functions.h>
#include <dynamic_model_hsm/hsm_category.h>
#include <dynamic_model_hsm/converter.h>
#include <program_settings.h>

namespace dm_hsm {

class Region;
class Link;

class TransmissionConverter : public virtual Converter {
 public:
  TransmissionConverter() = delete;
  virtual ~TransmissionConverter() = default;
  TransmissionConverter(const TransmissionConverter& other)
  : builder::SysCompActPrototype(other), // virtual inheritance and deleted default c'tor
    builder::ConverterPrototype(other), // virtual inheritance and deleted default c'tor
    am::SysComponentActive(other), // virtual inheritance and deleted default c'tor
    am::Converter(other), // virtual inheritance and deleted default c'tor
    sm::SysComponentActive(other), // virtual inheritance and deleted default c'tor
    sm::Converter(other), // virtual inheritance and deleted default c'tor
    SysComponentActive(other), // virtual inheritance and deleted default c'tor
    Converter(other),
    hsm_category_(other.hsm_category_),
    hsm_sub_category_(other.hsm_sub_category_),
    delivered_energy_(other.delivered_energy_),
    active_ (0),
    forward_(0){}
  TransmissionConverter(TransmissionConverter&&) = default;
  TransmissionConverter& operator=(const TransmissionConverter&) = delete;
  TransmissionConverter& operator=(TransmissionConverter&&) = delete;
  TransmissionConverter(const sm::Converter& origin)
      : builder::SysCompActPrototype(origin), // virtual inheritance and deleted default c'tor
        builder::ConverterPrototype(origin), // virtual inheritance and deleted default c'tor
        am::SysComponentActive(origin), // virtual inheritance and deleted default c'tor
        am::Converter(origin), // virtual inheritance and deleted default c'tor
        sm::SysComponentActive(origin), // virtual inheritance and deleted default c'tor
        sm::Converter(origin), // virtual inheritance and deleted default c'tor
        SysComponentActive(origin), // virtual inheritance and deleted default c'tor
        Converter(origin),
        hsm_category_(),
        hsm_sub_category_(dm_hsm::HSMSubCategory::UNAVAILABLE),
        delivered_energy_(),
        active_ (0),
        forward_(0){}
  /** \name Reset interface.*/
    ///@{
    void ResetSequencedTransmConverter();
    void resetCurrentTP(const aux::SimulationClock& clock) {
      SysComponentActive::resetCurrentTP(clock);
      active_ = false; }
      //std::cout << code() << " capacity=" << capacity(clock.now()) << " | usable_cap=" << usable_capacity_out_tp() << std::endl;}
    ///@}
  ///@{ initialisation
  std::string get_input() const {return input();}
  void setHSMcategory(dm_hsm::HSMCategory category) {hsm_category_ = category;}
  void setHSMSubCategory(dm_hsm::HSMSubCategory category) {hsm_sub_category_ = category;}
  dm_hsm::HSMCategory hsm_category() const {return hsm_category_;}
  dm_hsm::HSMSubCategory hsm_sub_category() const {return hsm_sub_category_;}
  ///@}

  /** \name HSM Operation Interface.*/
  ///@{
  double get_transmittable_pwr_infeed (const aux::SimulationClock& clock, double req_output) const;

  double reserveTransmission(const aux::SimulationClock& clock, const double transmission_feed_in);

  bool useTransmission(const dm_hsm::HSMCategory cat,
                           const double import_request,
                           const aux::SimulationClock& clock,
                           const std::weak_ptr<Region>& supply_region,
                           const std::weak_ptr<Link>& active_Link);
  ///@}
  void print_current_capacities() const;
  //  double getDiscountedLosses(aux::SimulationClock::time_point start,
  //                             aux::SimulationClock::time_point end) {
  //    return (aux::DiscountFuture(get_losses(), start, end, aux::years(1),
  //                                genesys::ProgramSettings::interest_rate())); }
  double getDiscountedLosses(aux::SimulationClock::time_point start,
                              aux::SimulationClock::time_point end,
                             const aux::SimulationClock::time_point present) {
     return (aux::DiscountFuture2(get_losses(),
                                  start,
                                  end,
                                  aux::years(1),
                                  genesys::ProgramSettings::interest_rate(),
                                  present)); }
  bool is_active() const {return active_;}
  bool direction_forward() const {return forward_;}
  bool activate_forward();
  bool activate_backwards();

 protected:
  /** \name Accessors to analysis_hsm*/
    ///@{
    const aux::TimeSeriesConstAddable& get_delivered_energy() const {return delivered_energy_;}
    ///@}

 private:
  bool update_current_state(const bool region_provide_status,
                            const double output_request,
                            const aux::SimulationClock& clock);
  ///Use of capacity which has to be requested/reserved  by getImportableCapacityTP
  dm_hsm::HSMCategory hsm_category_;
  dm_hsm::HSMSubCategory hsm_sub_category_ = dm_hsm::HSMSubCategory::UNAVAILABLE;
  ///Dynamic information
  aux::TimeSeriesConstAddable delivered_energy_;
  bool active_;
  bool forward_; //forward A->B
};

} /* namespace dm_hsm */

#endif /* DYNAMIC_MODEL_HSM_TRANSMISSION_CONVERTER_H_ */
