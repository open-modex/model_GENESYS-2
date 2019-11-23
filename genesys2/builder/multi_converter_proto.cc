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
// multi_converter_proto.cc
//
// This file is part of the genesys-framework v.2

#include <builder/multi_converter_proto.h>

namespace builder {

MultiConverterProto::MultiConverterProto(const std::string& code,
                                         const std::string& name,
                                         const std::string& input_type,
                                         const std::unordered_map<std::string, double>& output_conversion,
                                         //const double spec_co2,
                                         std::unique_ptr<aux::TimeBasedData> efficiency_new,
                                         std::unique_ptr<aux::TimeBasedData> cost,
                                         std::unique_ptr<aux::TimeBasedData> lifetime,
                                         std::unique_ptr<aux::TimeBasedData> internal_rate_of_return,
									 std::unique_ptr<aux::TimeBasedData> OaM_rate)
										 //std::vector<std::string> input_types)
        : SysCompActPrototype(code, name, input_type, "error in builder::multi_converter_proto.cc",
                              std::move(efficiency_new),
                              std::move(cost),
                              std::move(lifetime),
                              std::move(internal_rate_of_return),
                              std::move(OaM_rate)),
          ConverterPrototype(code, name, input_type, "error in builder::multi_converter_proto.cc", false,
                             std::move(efficiency_new), std::move(cost), std::move(lifetime),
                             std::move(internal_rate_of_return), std::move(OaM_rate)),
          output_conversion_(output_conversion) {
  // output_conversion is guaranteed to be non-empty in model_builder.cc
  for (const auto& it : output_conversion)
    output_types_.push_back(it.first);
  //not yet implemented for multi-inputs
  input_types_.push_back(input_type);
	//  for (const auto &it : input_types)
	//     input_types_.push_back(it);


}

} /* namespace builder */
