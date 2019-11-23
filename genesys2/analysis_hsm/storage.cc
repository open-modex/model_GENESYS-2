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
// storage.cc
//
// This file is part of the genesys-framework v.2

#include <analysis_hsm/storage.h>

#include <iomanip>

#include <program_settings.h>

namespace analysis_hsm {

void Storage::writeXmlVariables(io_routines::XmlWriter& xmlout, bool dynamic_output) {
  //std::cout << "FUNC-ID: Storage::writeXmlVariables \t" << code() << std::endl;
  analysis_hsm::SysComponentActive::writeXmlVariables(xmlout, dynamic_output);
  if (dynamic_output) {
    writeXmlElement(xmlout, "charged_energy", get_charged_energy() );
    writeXmlElement(xmlout, "energy_lost_by_transfer", get_energy_lost_by_transfer() );
  }
}

} /* namespace analysis_hsm */
