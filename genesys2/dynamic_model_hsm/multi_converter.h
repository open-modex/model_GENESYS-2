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
// multi_converter.h
//
// This file is part of the genesys-framework v.2

#ifndef DYNAMIC_MODEL_HSM_MULTI_CONVERTER_H_
#define DYNAMIC_MODEL_HSM_MULTI_CONVERTER_H_

#include <dynamic_model_hsm/converter.h>
#include <static_model/multi_converter.h>

namespace dm_hsm {

class MultiConverter : public virtual sm::MultiConverter, public virtual Converter {
 public:
  MultiConverter() = delete;
  virtual ~MultiConverter() = default;
  MultiConverter(const MultiConverter& other)  // correct handling of weak pointer members not possible in this context
      : builder::SysCompActPrototype(other), // virtual inheritance and deleted default c'tor
        builder::ConverterPrototype(other), // virtual inheritance and deleted default c'tor
        builder::MultiConverterProto(other), // virtual inheritance and deleted default c'tor
        am::SysComponentActive(other), // virtual inheritance and deleted default c'tor
        am::Converter(other), // virtual inheritance and deleted default c'tor
        am::MultiConverter(other), // virtual inheritance and deleted default c'tor
        sm::SysComponentActive(other), // virtual inheritance and deleted default c'tor
        sm::Converter(other), // virtual inheritance and deleted default c'tor
        sm::MultiConverter(other),
        SysComponentActive(other), // virtual inheritance and deleted default c'tor
        Converter(other),
        co2ptr_(),
        primary_energy_wptrs_(),
        storage_wptrs_(),
        co2_connected_(false){} // no connected copy possible
  MultiConverter(MultiConverter&& other) = delete; // maybe also ok as default, but consider again weak pointer members before!
  MultiConverter& operator=(const MultiConverter&) = delete;
  MultiConverter& operator=(MultiConverter&&) = delete;
  MultiConverter(const sm::MultiConverter& origin)
        : builder::SysCompActPrototype(origin), // virtual inheritance and deleted default c'tor
          builder::ConverterPrototype(origin), // virtual inheritance and deleted default c'tor
          builder::MultiConverterProto(origin), // virtual inheritance and deleted default c'tor
          am::SysComponentActive(origin), // virtual inheritance and deleted default c'tor
          am::Converter(origin), // virtual inheritance and deleted default c'tor
          am::MultiConverter(origin), // virtual inheritance and deleted default c'tor
          sm::SysComponentActive(origin), // virtual inheritance and deleted default c'tor
          sm::Converter(origin), // virtual inheritance and deleted default c'tor
          sm::MultiConverter(origin),
          SysComponentActive(origin), // virtual inheritance and deleted default c'tor
          Converter(origin),
          co2ptr_(),
          primary_energy_wptrs_(),
          storage_wptrs_(),
          co2_connected_(false) {}
  ///@{ initialisation
  virtual bool is_multi_converter() const override { return true;}
  virtual bool connectCo2Reservoir(std::shared_ptr<dm_hsm::PrimaryEnergy> primenergy_ptr);
  ///@}
  virtual double usable_power_out_tp(const aux::SimulationClock& clock,
                                     const double requestPower);
  //virtual double reserveConverterOutput(const aux::SimulationClock& clock,
  //                                      const double input);
  virtual bool useConverterOutput(const aux::SimulationClock& clock,
                                  const double output_request);//returns true if successful
  //virtual bool useConverterOutputMustrun(const aux::SimulationClock& clock);
  virtual double get_spec_vopex(const aux::SimulationClock& clock);
  /* Heat Integrationsversuch: kja
  virtual double get_output_conversion(const std::string sType);
  */
 protected:
  /** \name Accessors to analysis_hsm*/
    ///@{
    //const aux::TimeSeriesConstAddable& get_example() const { return example_;}
    ///@}

 private:
  std::weak_ptr<PrimaryEnergy> co2ptr_;
  std::pair<std::string, std::weak_ptr<PrimaryEnergy>> primary_energy_wptrs_;
  std::pair<std::string, std::weak_ptr<Storage>> storage_wptrs_;
  bool co2_connected_ = false;
};

} /* namespace dm_hsm */

#endif /* DYNAMIC_MODEL_HSM_MULTI_CONVERTER_H_ */
