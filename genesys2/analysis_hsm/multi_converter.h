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

#ifndef ANALYSIS_HSM_MULTI_CONVERTER_H_
#define ANALYSIS_HSM_MULTI_CONVERTER_H_

#include <analysis_hsm/converter.h>
#include <dynamic_model_hsm/multi_converter.h>

namespace analysis_hsm {

class MultiConverter : public dm_hsm::MultiConverter, public Converter {
 public:
  MultiConverter() = delete;
  virtual ~MultiConverter() = default;
  MultiConverter(const MultiConverter&) = delete;
  MultiConverter(MultiConverter&&) = default;
  MultiConverter& operator=(const MultiConverter&) = delete;
  MultiConverter& operator=(MultiConverter&&) = delete;

  MultiConverter(const dm_hsm::MultiConverter& origin)
        : builder::SysCompActPrototype(origin), // virtual inheritance and deleted default c'tor
          builder::ConverterPrototype(origin), // virtual inheritance and deleted default c'tor
          builder::MultiConverterProto(origin), // virtual inheritance and deleted default c'tor
          am::SysComponentActive(origin), // virtual inheritance and deleted default c'tor
          am::Converter(origin), // virtual inheritance and deleted default c'tor
          am::MultiConverter(origin), // virtual inheritance and deleted default c'tor
          sm::SysComponentActive(origin), // virtual inheritance and deleted default c'tor
          sm::Converter(origin), // virtual inheritance and deleted default c'tor
          sm::MultiConverter(origin), // virtual inheritance and deleted default c'tor
          dm_hsm::SysComponentActive(origin), // virtual inheritance and deleted default c'tor
          dm_hsm::Converter(origin), // virtual inheritance and deleted default c'tor
          SysComponentActive(origin), // virtual inheritance and deleted default c'tor
          dm_hsm::MultiConverter(origin),
          Converter(origin) {}

  virtual void writeXmlVariables(io_routines::XmlWriter& xmlout, bool dynamic_output) override;
};

} /* namespace analysis_hsm */

#endif /* ANALYSIS_HSM_MULTI_CONVERTER_H_ */
