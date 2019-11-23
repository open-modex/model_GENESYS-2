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
// sys_component_active.cc
//
// This file is part of the genesys-framework v.2

#include <analysis_hsm/sys_component_active.h>

#include <chrono>
#include <iomanip>

#include <program_settings.h>
#include <auxiliaries/functions.h>

namespace analysis_hsm {

void SysComponentActive::writeXmlVariables(io_routines::XmlWriter& xmlout, bool dynamic_output) {
  //std::cout << "FUNC-ID: SysComponentActive::writeXmlVariables \t" << code() << std::endl;
  writeXmlElement(xmlout, "capacity", get_capacity_() );
  writeXmlElement(xmlout, "capex", get_capex_() );
  writeXmlElement(xmlout, "fopex", get_fopex());
  writeXmlElement(xmlout, "deprecated_capex", get_discounted_capex());
  if (dynamic_output) { //output of dynamic class properties hereafter
    writeXmlElement(xmlout, "used_capacity", get_used_capacity());
    writeXmlElement(xmlout, "vopex", get_vopex());
  }
}

void SysComponentActive::writeXmlElement(io_routines::XmlWriter& xml_target, std::string data_code, const aux::TimeSeriesConstAddable& dataObj) {
  xml_target.addSubElement("data");
    xml_target.addAttr("code", data_code);
    xml_target.addTBD(dataObj);
  xml_target.closeSubElement();
}

} /* namespace analysis_hsm */
