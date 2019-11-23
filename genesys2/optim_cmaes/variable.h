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
// optimiser_variable.h
//
// This file is part of the genesys-framework v.2

#ifndef OPTIMISER_VARIABLE_H_
#define OPTIMISER_VARIABLE_H_

namespace optim_cmaes {

class Variable {
 public:
  Variable(double init_pt,
           double lbound,
           double ubound,
           std::string mounting_code,
           std::string tech_code,
           aux::SimulationClock::time_point tp)
    : value_ (0.0),
      init_point_(init_pt),
      lbound_(lbound),
      ubound_(ubound),
      mounting_code_(mounting_code),
      tech_code_(tech_code),
      time_point_(tp){}

  Variable(double init_pt,
           double lbound,
           double ubound)
    : value_ (0.0),
      init_point_(init_pt),
      lbound_(lbound),
      ubound_(ubound),
      mounting_code_("empty"),
      tech_code_("empty"),
      time_point_(aux::SimulationClock::time_point_from_string("1900-01-01_00:00")){
    //std::cout << "**** \t ****called simple c'tor optim_cmaes::Variable for non-optimisation " << std::endl;
  }

  ~Variable() = default;
  void WriteValue(double value) {value_ = value;}
  //cbu std::tuple<double> getVariable();

  double value() const {return value_;}
  double lbound() const {return lbound_;}
  double ubound() const {return ubound_;}
  double init_point() const {return init_point_;}
  std::string mounting_code() const {return mounting_code_;}
  std::string tech_code() const {return tech_code_;}
  aux::SimulationClock::time_point time_point() const {return time_point_;}

 private:
  double value_;
  double init_point_;
  //boxed bounds
  double lbound_; //lower boundary
  double ubound_; // upper boundary
  std::string mounting_code_;
  std::string tech_code_;
  aux::SimulationClock::time_point time_point_;
};

} /* namespace optim_cmaes */

#endif /* OPTIMISER_VARIABLE_H_ */
