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
// converter.h
//
// This file is part of the genesys-framework v.2

#ifndef DYNAMIC_MODEL_HSM__CONVERTER_H_
#define DYNAMIC_MODEL_HSM__CONVERTER_H_

#include <memory>

#include <auxiliaries/time_tools.h>
#include <dynamic_model_hsm/hsm_category.h>
#include <dynamic_model_hsm/primary_energy.h>
#include <dynamic_model_hsm/storage.h>
#include <dynamic_model_hsm/sys_component_active.h>
#include <static_model/converter.h>

namespace dm_hsm {

class Converter : public virtual sm::Converter, public virtual SysComponentActive {
 public:
  Converter() = delete;
  virtual ~Converter() = default;
  Converter(const Converter& other)  // correct handling of weak pointer members not possible in this context
      : builder::SysCompActPrototype(other), // virtual inheritance and deleted default c'tor
        builder::ConverterPrototype(other), // virtual inheritance and deleted default c'tor
        am::SysComponentActive(other), // virtual inheritance and deleted default c'tor
        am::Converter(other), // virtual inheritance and deleted default c'tor
        sm::SysComponentActive(other), // virtual inheritance and deleted default c'tor
        sm::Converter(other),
        SysComponentActive(other),
        primenergyptr_(), //no ptr in copy
        storageptr_(),//no ptr in copy
        hsm_category_(other.hsm_category_),
        hsm_sub_category_(other.hsm_sub_category_),
        storage_connected_(other.storage_connected_),
        primary_energy_connected_(other.primary_energy_connected_),
        used_co2_emissions_(other.used_co2_emissions_),
        losses_(other.losses_) {}
  Converter(Converter&&) = delete; // maybe also ok as default, but consider again weak pointer members before!
  Converter& operator=(const Converter&) = delete;
  Converter& operator=(Converter&&) = delete;
  Converter(const sm::Converter& origin)
      : builder::SysCompActPrototype(origin), // virtual inheritance and deleted default c'tor
        builder::ConverterPrototype(origin), // virtual inheritance and deleted default c'tor
        am::SysComponentActive(origin), // virtual inheritance and deleted default c'tor
        am::Converter(origin), // virtual inheritance and deleted default c'tor
        sm::SysComponentActive(origin), // virtual inheritance and deleted default c'tor
        sm::Converter(origin),
        SysComponentActive(origin),
        primenergyptr_(),
        storageptr_(),
        hsm_category_(),
        hsm_sub_category_(dm_hsm::HSMSubCategory::UNAVAILABLE),
        storage_connected_(false),
        primary_energy_connected_(false),
        is_starting_(false),
        is_rampingdown_(false),
        price_per_unit_current_tp_(std::numeric_limits<double>::infinity()),
        co2_price_per_unit_current_tp_(std::numeric_limits<double>::infinity()),
        used_co2_emissions_(),
        losses_() {}

  /** \name Init.*/
  ///@{
    dm_hsm::HSMCategory get_HSMCategory() const {return hsm_category_;}
    dm_hsm::HSMSubCategory get_HSMSubCategory() const { return hsm_sub_category_;}
    virtual double get_spec_vopex(const aux::SimulationClock& clock) {return 0.;}
    void setHSMcategory(dm_hsm::HSMCategory category);
    void setHSMSubCategory(dm_hsm::HSMSubCategory category) {hsm_sub_category_ = category;}
    void connectReservoir();
    void connectStorage(std::shared_ptr<dm_hsm::Storage> storage_ptr);
    void connectPrimaryEnergy(std::shared_ptr<dm_hsm::PrimaryEnergy> primenergy);
    virtual bool connectCo2Reservoir(std::shared_ptr<dm_hsm::PrimaryEnergy> primenergy_ptr);
    virtual bool is_multi_converter() const { return false;}
  ///@}

  /** \name Reset Functions.*/
  ///@{
    void ResetSequencedConverter();
  ///@}

  /** \name Hierarchical Operation Functions.*/
  ///@{
    double generation_fromPotential(const aux::SimulationClock& clock);
    //DEPRECATED
    virtual double usable_power_out_tp(const aux::SimulationClock& clock, const double requestPower);
    /* Heat Integrationsversuch: kja
    virtual double usable_power_out_tp_el2heat(const aux::SimulationClock& clock, const double requestPower);
    */
    //virtual double reserveConverterOutput(const aux::SimulationClock& clock, const double el_output );//returns el_output
    virtual bool useConverterOutput(const aux::SimulationClock& clock,
                                    const double output_request);//returns true if successful
    /* Heat Integrationsversuch: kja
    virtual bool useConverterOutput_el2heat(const aux::SimulationClock& clock,
                                    const double output_request,
                                    const std::string sType);//returns true
    */
    double reserveDischarger(const aux::SimulationClock& clock,
                                const double output_request);

    double reserveChargerInput(const aux::SimulationClock& clock,
                               double inputRequest);//returns  accepted input
    bool useChargeStorage(double power,
                          const aux::SimulationClock& clock); //returns true if successful

    /* Heat Integrationsversuch: kja
    virtual double get_output_conversion(const std::string sType);
    */

    //accessors to create converter maps
    double get_efficiency(const aux::SimulationClock& clock) const;
    double get_capacity (const aux::SimulationClock& clock) const;

    //virtual bool useConverterOutputMustrun(const aux::SimulationClock& clc) {
    //	std::cerr <<"Defined only for dispatchable converter/ multi-conferter"<< std::endl; return false;}
  ///@}

  /** \name Operation Status.*/
  ///@{
    void check_for_decommission();
  ///@}

 protected:
  std::weak_ptr<PrimaryEnergy> primenergyptr() const {return primenergyptr_.lock();}
  double losses( aux::SimulationClock::time_point tp) const { std::cout << "losses " << std::endl; return losses_[tp];}
  void set_losses(aux::TimeSeriesConstAddable TSA) {losses_+=TSA;}
  void set_co2_emissions(const aux::TimeSeriesConstAddable& TSA) {used_co2_emissions_+=TSA;}


  /** \name Accessors to analysis_hsm*/
  ///@{
  const aux::TimeSeriesConstAddable& get_used_co2_emissions() const { return used_co2_emissions_;}
  const aux::TimeSeriesConstAddable& get_losses() const { return losses_;}
  ///@}

 private:

  std::weak_ptr<PrimaryEnergy> primenergyptr_;
  std::weak_ptr<Storage> storageptr_;

  dm_hsm::HSMCategory hsm_category_;
  dm_hsm::HSMSubCategory hsm_sub_category_ = dm_hsm::HSMSubCategory::UNAVAILABLE;

  bool storage_connected_ = false;
  bool primary_energy_connected_ = false;
  bool is_starting_ = false;
  bool is_rampingdown_ = false;
  double price_per_unit_current_tp_ = std::numeric_limits<double>::infinity();
  double co2_price_per_unit_current_tp_ = std::numeric_limits<double>::infinity();
  aux::TimeSeriesConstAddable used_co2_emissions_; ///stores the produced co2 amount in detail
  aux::TimeSeriesConstAddable losses_;
};

} /* namespace dm_hsm */

#endif /* DYNAMIC_MODEL_HSM_CONVERTER_H_ */
