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
// mulit_converter.h
//
// This file is part of the genesys-framework v.2


#ifndef ABSTRACT_MODEL_MULTI_CONVERTER_H_
#define ABSTRACT_MODEL_MULTI_CONVERTER_H_

#include <abstract_model/converter.h>
#include <builder/multi_converter_proto.h>

namespace am {

class MultiConverter : public virtual builder::MultiConverterProto, public virtual Converter {
 public:
  MultiConverter() = delete;
  virtual ~MultiConverter() override = default;
  MultiConverter(const MultiConverter&) = default;
  MultiConverter(MultiConverter&&) = default;
  MultiConverter& operator=(const MultiConverter&) = delete;
  MultiConverter& operator=(MultiConverter&&) = delete;

  MultiConverter(const MultiConverterProto& origin,
                 std::unique_ptr<aux::TimeBasedData> installation,
                 aux::SimulationClock::time_point start_installation,
                 aux::SimulationClock::time_point end_installation,
                 aux::SimulationClock::duration interval_installation)
        : builder::SysCompActPrototype(origin), // virtual inheritance and deleted default c'tor,
          builder::ConverterPrototype(origin), // virtual inheritance and deleted default c'tor
          builder::MultiConverterProto(origin),
          SysComponentActive(origin, std::move(installation), start_installation, end_installation,
                             interval_installation), // virtual inheritance and deleted default c'tor
          Converter(*this) {}
};

} /* namespace am */

#endif /* ABSTRACT_MODEL_MULTI_CONVERTER_H_ */
