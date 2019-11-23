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
// hsm_category.h
//
// This file is part of the genesys-framework v.2

#ifndef DYNAMIC_MODEL_HSM_HSM_CATEGORY_H_
#define DYNAMIC_MODEL_HSM_HSM_CATEGORY_H_

#include <iostream>
#include <string>

namespace dm_hsm {

//enum class HSMCategory { RE_GENERATOR, DISPATCHABLE_GENERATOR, CONV_MUSTRUN, ST_STORAGE, LT_STORAGE, STORAGE_CHARGER, STORAGE_DISCHARGER, STORAGE_BICHARGER, LINE_CONVERTER, UNAVAILABLE };
enum class HSMCategory { CONV_MUSTRUN, RE_GENERATOR, ST_STORAGE, LT_STORAGE, DISPATCHABLE_GENERATOR, LINE_CONVERTER, UNAVAILABLE, HEAT_GENERATOR, HEAT_STORAGE, EL2HEAT, HEAT_AND_EL, EL2HEAT_STORAGE};
enum class HSMSubCategory { UNAVAILABLE, CHARGER, DISCHARGER, BICHARGER };
//HEAT_GENERATOR, HEAT_DISSIPATOR, MULTI_DOCK_ELEMENT//

} /* namespace dm_hsm */

#endif /* DYNAMIC_MODEL_HSM_HSM_CATEGORY_H_ */
