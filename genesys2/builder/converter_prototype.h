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
// converter_prototype.h
//
// This file is part of the genesys-framework v.2

#ifndef BUILDER_CONVERTER_PROTOTYPE_H_
#define BUILDER_CONVERTER_PROTOTYPE_H_

#include <string>
#include <utility>

#include <builder/sys_comp_act_prototype.h>

namespace builder {

class ConverterPrototype : public virtual SysCompActPrototype {
 public:
  ConverterPrototype() = delete;
  virtual ~ConverterPrototype() override = default;
  ConverterPrototype(const ConverterPrototype& other)
      : SysCompActPrototype(other),
        bidirectional_(other.bidirectional_) {}
  ConverterPrototype(ConverterPrototype&& other) = default;
  ConverterPrototype& operator=(const ConverterPrototype&) = delete;
  ConverterPrototype& operator=(ConverterPrototype&&) = delete;
  ConverterPrototype(const std::string& code,
                     const std::string& name,
                     const std::string& input,
                     const std::string& output,
                     bool bidirectional,
                     std::unique_ptr<aux::TimeBasedData> efficiency_new,
                     std::unique_ptr<aux::TimeBasedData> cost,
                     std::unique_ptr<aux::TimeBasedData> lifetime,
                     std::unique_ptr<aux::TimeBasedData> internal_rate_of_return,
                     std::unique_ptr<aux::TimeBasedData> OaM_rate)
      : SysCompActPrototype(code, name, input, output, std::move(efficiency_new), std::move(cost), std::move(lifetime),
                     std::move(internal_rate_of_return), std::move(OaM_rate)),
        bidirectional_(bidirectional) {}
  ConverterPrototype(const SysCompActPrototype& origin,
                     bool bidirectional)
      : SysCompActPrototype(origin),
        bidirectional_(bidirectional) {}

  bool bidirectional() const {return bidirectional_;}
  virtual std::vector<std::string> output_types() const {return std::vector<std::string>{output()};}
  virtual std::vector<std::string>  input_types() const {return std::vector<std::string>{ input()};}


 private:
  bool bidirectional_ = false;
};

} /* namespace builder */

#endif /* BUILDER_CONVERTER_PROTOTYPE_H_ */
