/// ==================================================================
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
// sys_comp_act_prototype.h
//
// This file is part of the genesys-framework v.2

#ifndef BUILDER_SYS_COMP_ACT_PROTOTYPE_H_
#define BUILDER_SYS_COMP_ACT_PROTOTYPE_H_

#include <memory>
#include <string>
#include <utility>

#include <auxiliaries/simulation_clock.h>
#include <auxiliaries/time_based_data.h>
#include <builder/sys_component.h>

namespace builder {

class SysCompActPrototype : public SysComponent {
 public:
  SysCompActPrototype() = delete;
  virtual ~SysCompActPrototype() override = default;
  SysCompActPrototype(const SysCompActPrototype& other)
      : SysComponent(other),
        input_(other.input_),
        output_(other.output_),
        efficiency_new_(std::move(other.efficiency_new_->clone())),
        cost_(std::move(other.cost_->clone())),
        lifetime_(std::move(other.lifetime_->clone())),
        internal_rate_of_return_(std::move(other.internal_rate_of_return_->clone())),
        OaM_rate_(std::move(other.OaM_rate_->clone())) {}
  SysCompActPrototype(SysCompActPrototype&& other) = default;
  SysCompActPrototype& operator=(const SysCompActPrototype&) = delete;
  SysCompActPrototype& operator=(SysCompActPrototype&&) = delete;
  SysCompActPrototype(const std::string& code,
                      const std::string& name,
                      const std::string& input,
                      const std::string& output,
                      std::unique_ptr<aux::TimeBasedData> efficiency_new,
                      std::unique_ptr<aux::TimeBasedData> cost,
                      std::unique_ptr<aux::TimeBasedData> lifetime,
                      std::unique_ptr<aux::TimeBasedData> internal_rate_of_return,
                      std::unique_ptr<aux::TimeBasedData> OaM_rate)
      : SysComponent(code, name),
        input_(input),
        output_(output),
        efficiency_new_(std::move(efficiency_new)),
        cost_(std::move(cost)),
        lifetime_(std::move(lifetime)),
        internal_rate_of_return_(std::move(internal_rate_of_return)),
        OaM_rate_(std::move(OaM_rate)) {}

  virtual std::string input_type() const {return input();}
  virtual std::string output_type() const {return output();}

 protected:
  std::string input() const {return input_;}
  std::string output() const {return output_;}
  double efficiency_new(aux::SimulationClock::time_point tp) const {return ((*efficiency_new_)[tp]);}
  double cost(aux::SimulationClock::time_point tp) const {return ((*cost_)[tp]);}
  double lifetime(aux::SimulationClock::time_point tp) const {return ((*lifetime_)[tp]);}
  double internal_rate_of_return(aux::SimulationClock::time_point tp) const {return ((*internal_rate_of_return_)[tp]);}
  double OaM_rate(aux::SimulationClock::time_point tp) const {return ((*OaM_rate_)[tp]);}
  std::unique_ptr<aux::TimeBasedData> efficiency_new() const {return efficiency_new_->clone();}
  std::unique_ptr<aux::TimeBasedData> cost() const {return cost_->clone();}
  std::unique_ptr<aux::TimeBasedData> lifetime() const {return lifetime_->clone();}
  std::unique_ptr<aux::TimeBasedData> internal_rate_of_return() const {return internal_rate_of_return_->clone();}
  std::unique_ptr<aux::TimeBasedData> OaM_rate() const {return OaM_rate_->clone();}

 private:
  std::string input_;
  std::string output_;
  std::unique_ptr<aux::TimeBasedData> efficiency_new_;
  std::unique_ptr<aux::TimeBasedData> cost_;
  std::unique_ptr<aux::TimeBasedData> lifetime_;
  std::unique_ptr<aux::TimeBasedData> internal_rate_of_return_;
  std::unique_ptr<aux::TimeBasedData> OaM_rate_;
};

} /* namespace builder */

#endif /* BUILDER_SYS_COMP_ACT_PROTOTYPE_H_ */
