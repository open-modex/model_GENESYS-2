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
// link.h
//
// This file is part of the genesys-framework v.2

#ifndef DYNAMIC_MODEL_HSM_LINK_H_
#define DYNAMIC_MODEL_HSM_LINK_H_

#include <memory>
#include <string>
#include <unordered_map>

#include <dynamic_model_hsm/hsm_category.h>
#include <dynamic_model_hsm/transmission_converter.h>
#include <static_model/link.h>

namespace dm_hsm {

class Region;

class Link : public sm::Link {
 public:
  Link() = delete;
  virtual ~Link() override = default;
  Link(const Link& other);
  Link(Link&& other) = delete; // if this move c'tor is needed, special implementation is necessary, taking care of correct handling of the weak pointers
  Link& operator=(const Link&) = delete;
  Link& operator=(Link&&) = delete;
  Link(const sm::Link& origin,
       const std::unordered_map<std::string, std::shared_ptr<Region> >& region);
  void RegisterWithRegions(std::shared_ptr<Link> this_link) const;

  /** \name Reset interface.*/
  ///@{
  void resetCurrentTP(const aux::SimulationClock& clock);
  void set_annual_lookups(const aux::SimulationClock& clock);
  void resetSequencedLink(const aux::SimulationClock& clock);
  void uncheck_active_current_year();
  void add_OaM_cost(aux::SimulationClock::time_point tp_now);
  ///@}

  /** \name Region Connectivity interface.*/
  ///@{
  void updateConverterHSMCategory(std::unordered_map<std::string, dm_hsm::HSMCategory>& keymap);
  ///@}

  /** \name hsm_operation Interface.*/
  ///@{
  double get_link_length(const aux::SimulationClock& clock) const {return (length(clock.now()));}

  double getImportCapacity(const dm_hsm::HSMCategory cat,
                           const aux::SimulationClock& clock,
                           const std::string& requesting_region_code,
                           const std::weak_ptr<TransmissionConverter>& active_converter,
                           const double requested_max_import);

  bool useImportCapacity(const dm_hsm::HSMCategory cat,
                         const double import_request,
                         const aux::SimulationClock& clock,
                         const std::string& requesting_region_code,
                         const std::weak_ptr<TransmissionConverter>& active_converter,
                         const std::weak_ptr<Link>& active_Link );
//  double getDiscountedValue(std::string query,
//                            aux::SimulationClock::time_point start,
//                            aux::SimulationClock::time_point end) const;
  double getDiscountedValue(std::string query,
                            aux::SimulationClock::time_point start,
                            aux::SimulationClock::time_point end,
                            const aux::SimulationClock::time_point discount_present =
                                               aux::SimulationClock::time_point_from_string("1970-01-01_00:00")) const;
  double getSumValue(std::string query,
                     aux::SimulationClock::time_point start,
                     aux::SimulationClock::time_point end) const;
  ///@}
  void calculated_annual_disc_capex(aux::SimulationClock::time_point start);


  /** \name DEBUG Interface.*/
  ///@{
  void print_current_capacities() const;
  ///@}
 protected:
  const std::unordered_map<std::string, std::shared_ptr<TransmissionConverter> >& tr_converter_ptrs_dm() const {return converter_ptrs_;}

 private:
  double requestRegionExportLimit(const dm_hsm::HSMCategory cat,
                                  const aux::SimulationClock& clock,
                                  const double requestedExport,
                                  const std::weak_ptr<Region>& supply_region);

  std::weak_ptr<Region> otherRegion(const std::string& requesting_region_code); ///< Identifies the opposing region_ptr to code_ of destinationRegion

  std::unordered_map<std::string, std::shared_ptr<TransmissionConverter> > converter_ptrs_;
  std::weak_ptr<Region> region_A_ptr_;
  std::weak_ptr<Region> region_B_ptr_;
};

} /* namespace dm_hsm */

#endif /* DYNAMIC_MODEL_HSM_LINK_H_ */
