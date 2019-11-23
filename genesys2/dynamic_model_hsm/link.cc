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
// link.cc
//
// This file is part of the genesys-framework v.2

#include <dynamic_model_hsm/link.h>

#include <iostream>

#include <auxiliaries/functions.h>
#include <dynamic_model_hsm/region.h>

namespace dm_hsm {

Link::Link(const Link& other)
    : sm::Link(other),
      region_A_ptr_(),
      region_B_ptr_() {
  if(!other.converter_ptrs_.empty()) {
    for (const auto& it : other.converter_ptrs_)
        converter_ptrs_.emplace(it.first, std::shared_ptr<TransmissionConverter>(new TransmissionConverter(*(it.second))));
  }
}

Link::Link(const sm::Link& origin,
           const std::unordered_map<std::string, std::shared_ptr<Region> >& input_region)
    : sm::Link(origin) {
  if(!converter_ptrs_sm().empty()){
    for (const auto& it : converter_ptrs_sm())
        converter_ptrs_.emplace(it.first, std::shared_ptr<TransmissionConverter>(new TransmissionConverter(*(it.second))));
  }
  auto region_A_pos = input_region.find(region_A());
  if (region_A_pos != input_region.end()) {
    region_A_ptr_ = region_A_pos->second;
  } else {
    std::cerr << "ERROR in dm_hsm::Link::Link :" << std::endl
        << "Region_A " << region_A() << " not found in Region list region" << std::endl;
    std::terminate();
  }
  auto region_B_pos = input_region.find(region_B());
  if (region_B_pos != input_region.end()) {
    region_B_ptr_ = region_B_pos->second;
  } else {
    std::cerr << "ERROR in dm_hsm::Link::Link :" << std::endl
        << "Region_B " << region_B() << " not found in Region list region" << std::endl;
    std::terminate();
  }
}

void Link::RegisterWithRegions(std::shared_ptr<Link> this_link) const {
  region_A_ptr_.lock()->connectLink(std::weak_ptr<Link>(this_link), converter_ptrs_);
  region_B_ptr_.lock()->connectLink(std::weak_ptr<Link>(this_link), converter_ptrs_);
}

void Link::resetCurrentTP(const aux::SimulationClock& clock) {
  //std::cout << "FUNC-ID:  Link::resetCurrentTP() " << std::endl;
  if(!converter_ptrs_.empty()){
    for (auto&& it : converter_ptrs_) {
        //TODO find out what has to be reset.
        //std::cout << "Link::resetCurrentTP on Link between " << region_A() <<"-"<< region_B() <<  std::endl;
        it.second->resetCurrentTP(clock);//set active = 0
    }
  } else {
    std::cerr << "***Link::resetCurrentTP: No Converter on Link between " << region_A() <<"-"<< region_B() <<  std::endl;
  }
  //std::cout << "FUNC-END:  Link::resetCurrentTP() " << std::endl;
}

void Link::set_annual_lookups(const aux::SimulationClock& clock) {
	if(!converter_ptrs_.empty()) {
		for(const auto& it : converter_ptrs_){
			it.second->set_annaul_lookups(clock);
		}
	}
}

void Link::resetSequencedLink(const aux::SimulationClock&) {
	if(!converter_ptrs_.empty()){
	    for (auto&& it : converter_ptrs_) {
	    	it.second->ResetSequencedTransmConverter();
	    }
	}
}

void Link::uncheck_active_current_year(){
	if(!converter_ptrs_.empty()){
	    for (auto&& it : converter_ptrs_) {
	    	it.second->uncheck_active_current_year();
	    }
	}
}

void Link::add_OaM_cost(aux::SimulationClock::time_point tp_now) {
  if (!converter_ptrs_.empty()) {
    for (const auto&it : converter_ptrs_) {
      it.second->add_OaM_cost(tp_now);
    }
  }
//	if (!converter_ptrs_.empty()) {
//		for (auto&& it : converter_ptrs_) {
//			if (it.second->active_current_year()) {
//				it.second->add_OaM_cost(tp_now);
//			} else {
//				it.second->add_OaM_cost_zero(tp_now);
//			}
//		}
//	}
}

void  Link::updateConverterHSMCategory(std::unordered_map<std::string, dm_hsm::HSMCategory>& keymap) {
  //DEBUG  std::cout << "FUNC-ID:  Link::updateConverterHSMCategory() " << std::endl;
  if(!converter_ptrs_.empty()){
    for (auto&& it : converter_ptrs_) {
        auto converter_pos = keymap.find(it.first);
        if (converter_pos != keymap.end()) {
          it.second->setHSMcategory(converter_pos->second);
        } else {
          // error it.first not found in keymap of HSMCategory
        }
    }
  }
}

bool Link::useImportCapacity(const dm_hsm::HSMCategory cat,
                             const double import_request,
                             const aux::SimulationClock& clock,
                             const std::string& requesting_region_code,
                             const std::weak_ptr<TransmissionConverter>& active_converter,
                             const std::weak_ptr<Link>& active_Link) {
  //std::cout << "\tLink::useImportCapacity called with request " << import_request << " GW from " << requesting_region_code << std::endl;
  if (import_request > 0) {
    if (requesting_region_code != region_A_ptr_.lock()->code()) {//forward operation A-->B
      //std::cout << "\tlink " << code() << "|" << active_converter.lock()->code()<< " operating request forward" << std::endl;
      if(active_converter.lock()->activate_forward()){
        //operate successfully
        return active_converter.lock()->useTransmission(cat, import_request, clock, otherRegion(requesting_region_code), active_Link.lock() );
      } else {
        std::cout << "\t\tDEBUG Link::useImportCapacity | TR-Converter reversed illegally" << std::endl;
        return false;
      }
    } else {//backward operation A<--B
      //std::cout << "\tlink " << code() << "|" << active_converter.lock()->code()<< " operating request backwards" << std::endl;
      if(active_converter.lock()->activate_backwards()){
        //operate successfully
        return active_converter.lock()->useTransmission(cat, import_request, clock, otherRegion(requesting_region_code), active_Link.lock() );
      } else {
        std::cout << "\t\tDEBUG Link::useImportCapacity | TR-Converter reversed illegally" << std::endl;
        return false;
      }
    }
    //  std::cout << "\t\tDEBUG TR-Converter reserved: " << active_converter.lock()->reserved_capacity_tp() << " | request= " << import_request << std::endl;
    //  return active_converter.lock()->useTransmission(cat, import_request, clock, otherRegion(requesting_region_code), active_Link.lock() );
  }
  return false;
}

//double Link::getDiscountedValue(std::string query,
//                                aux::SimulationClock::time_point start,
//                                aux::SimulationClock::time_point end) const {
//  double discounted_value = 0.0;
//  if (!converter_ptrs_.empty()) {
//    for (const auto& it : converter_ptrs_) {
//      discounted_value += it.second->getDiscountedValue(query, start, end);
//    }
//  }
//  return (discounted_value);
//}

double Link::getDiscountedValue(std::string query,
                                aux::SimulationClock::time_point start,
                                aux::SimulationClock::time_point end,
                                const aux::SimulationClock::time_point discount_present) const {
  aux::SimulationClock::time_point present;
  if (discount_present == aux::SimulationClock::time_point_from_string("1970-01-01_00:00")){
    present = start;
  } else {
    present = discount_present;
  }
  double discounted_value = 0.0;
  if (!converter_ptrs_.empty()) {
    for (const auto& it : converter_ptrs_) {
      discounted_value += it.second->getDiscountedValue(query, start, end, present);
    }
  }
  return (discounted_value);

}

double Link::getSumValue(std::string query,
                         aux::SimulationClock::time_point start,
                         aux::SimulationClock::time_point end) const {
  double sum_value = 0.0;
  if (!converter_ptrs_.empty()) {
    for (const auto& it : converter_ptrs_) {
      sum_value += it.second->get_sum(query, start, end);
    }
  }
  return (sum_value);

}

void Link::calculated_annual_disc_capex(aux::SimulationClock::time_point start) {
  //std::cout << "FUNC-ID: Link::calculated_annual_capex" << std::endl;
  if (!converter_ptrs_.empty()) {
    for (const auto &it : converter_ptrs_) {
      it.second->calculated_annual_disc_capex(start);
    }
  }
}


void Link::print_current_capacities() const {
  std::cout << code() << "\t | " << std::endl ;
  for (const auto& it : converter_ptrs_)
    it.second->print_current_capacities();
  std::cout << "----------------------------------------------------------------------------------" << std::endl;
}

double Link::requestRegionExportLimit(const dm_hsm::HSMCategory cat,
                                  const aux::SimulationClock& clock,
                                  const double requested_export_infeed,
                                  const std::weak_ptr<Region>& supply_region) {
  return supply_region.lock()->getExportCapacity(cat, clock, requested_export_infeed);
}

std::weak_ptr<Region> Link::otherRegion(const std::string& requesting_region_code) {
  if (region_A_ptr_.lock()->code() == requesting_region_code) {
    return region_B_ptr_.lock();
  } else if (region_B_ptr_.lock()->code() == requesting_region_code) {
    return region_A_ptr_.lock();
  } else {
    std::cerr <<"Error in dm_hsm::Link::otherRegion - region not correctly identified" << std::endl;
    std::terminate();
    return std::weak_ptr<Region>();
  }
}


double Link::getImportCapacity(const dm_hsm::HSMCategory cat,
                          const aux::SimulationClock& clock,
                          const std::string& requesting_region_code,
                          const std::weak_ptr<TransmissionConverter>& active_converter,
                          const double requested_max_import)  {
  //transmittable pwr is inflated request / efficiency of link
  double transmittablePwrInfeed = active_converter.lock()->get_transmittable_pwr_infeed(clock, requested_max_import);
  //  std::cout << "Link::getImportCapacity:------------- "<<std::endl;
  //  std::cout << "\t| new transmittablePwr = " << transmittablePwrInfeed << " GW" << std::endl;
  if (transmittablePwrInfeed  > genesys::ProgramSettings::approx_epsilon()) {
    //check transmission converter direction - if fails -> return 0 else continue
      if (requesting_region_code != region_A_ptr_.lock()->code()) {//forward operation A-->B
        if (active_converter.lock()->is_active()){
          if (active_converter.lock()->direction_forward()) {
          } else {
            return 0;
          }
        } //else continue to check import capacity
      } else {//backwards operation A-->B
        if (active_converter.lock()->is_active()){
          if (!active_converter.lock()->direction_forward()) {//!forward = backwards
          } else {
            return 0;
          }
        } //else continue to check import capacity
      }
    //check import capacity:
     transmittablePwrInfeed = requestRegionExportLimit(cat,clock,transmittablePwrInfeed,otherRegion(requesting_region_code) );
     if (transmittablePwrInfeed > genesys::ProgramSettings::approx_epsilon()) {
       //       std::cout << "\t\t\t"<<  otherRegion(requesting_region_code).lock()->code() << " can supply " << regio_limit << " GW" << std::endl;
       return active_converter.lock()->reserveTransmission(clock,transmittablePwrInfeed);
     } else {
       //std::cout << "\t\t\t"<<otherRegion(requesting_region_code).lock()->code() << " has no capacity " << std::endl;
     }
   } else {
     //
	   std::cout << "\t\t\t"<<code() << " has no capacity " << std::endl;
     return 0;
   }
   return 0;
}

} /* namespace dm_hsm */
