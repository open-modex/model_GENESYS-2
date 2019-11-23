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
// multi_converter_proto.h
//
// This file is part of the genesys-framework v.2

#ifndef BUILDER_MULTI_CONVERTER_PROTO_H_
#define BUILDER_MULTI_CONVERTER_PROTO_H_

#include <unordered_map>

#include <builder/converter_prototype.h>

namespace builder {

class MultiConverterProto : public virtual ConverterPrototype {
 public:
  MultiConverterProto() = delete;
  virtual ~MultiConverterProto() override = default;
  MultiConverterProto(const MultiConverterProto& other)
        : SysCompActPrototype(other),
          ConverterPrototype(other),
          output_types_(other.output_types_),
          input_types_(other.input_types_),
          output_conversion_(other.output_conversion_)//,
          /*new_vars(other.new_vars)*/ {}
  MultiConverterProto(MultiConverterProto&& other) = default;
  MultiConverterProto& operator=(const MultiConverterProto&) = delete;
  MultiConverterProto& operator=(MultiConverterProto&&) = delete;

  MultiConverterProto(const std::string& code,
                      const std::string& name,
                      const std::string& input_type,
                      const std::unordered_map<std::string, double>& output_conversion,
                      std::unique_ptr<aux::TimeBasedData> efficiency_new,
                      std::unique_ptr<aux::TimeBasedData> cost,
                      std::unique_ptr<aux::TimeBasedData> lifetime,
                      std::unique_ptr<aux::TimeBasedData> internal_rate_of_return,
                      std::unique_ptr<aux::TimeBasedData> OaM_rate);//,
					  //std::vector<std::string> input_types = {});

  virtual std::vector<std::string> output_types() const {return output_types_;}
  virtual std::vector<std::string>  input_types() const { return input_types_;}
  virtual std::unordered_map<std::string,double> output_conversion() const { return output_conversion_;}

 private:
  std::vector<std::string> output_types_;
  std::vector<std::string> input_types_;
  std::unordered_map<std::string, double> output_conversion_;
};

} /* namespace builder */

#endif /* BUILDER_MULTI_CONVERTER_PROTO_H_ */
