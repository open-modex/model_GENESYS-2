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
// primary_energy.cc
//
// This file is part of the genesys-framework v.2

#include <analysis_hsm/primary_energy.h>
//#include <program_settings.h>

namespace analysis_hsm {

void PrimaryEnergy::writeDynamicVariables(io_routines::XmlWriter& xmlout) {
  //std::cout << "writeStaticVariables "<< std::endl;
  writeXmlElement(xmlout, "vopex_", get_vopex_() );
  writeXmlElement(xmlout, "consumed_energy_", get_consumed_energy_() );
}

void PrimaryEnergy::writeXmlElement(io_routines::XmlWriter& xml_target, std::string data_code, const aux::TimeSeriesConstAddable& dataObj) {
  xml_target.addSubElement("data");
    xml_target.addAttr("code", data_code);
    xml_target.addTBD(dataObj);
  xml_target.closeSubElement();
}

} /* namespace analysis_hsm */
