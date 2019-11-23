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
// analysed_model.cc
//
// This file is part of the genesys-framework v.2

#include <analysis_hsm/analysed_model.h>
#include <version.h>
#include <cmd_parameters.h>

namespace analysis_hsm {

AnalysedModel::AnalysedModel(const DynamicModel& origin)
    : dm_hsm::DynamicModel(origin) {
  //DEBUG std::cout << "DEBUG: C-Tor AnalysedModel::AnalysedModel" << std::endl;
  if (!dm_hsm::DynamicModel::regions().empty()) {
    for (const auto& it : DynamicModel::regions()) {
      regions_.emplace(it.first, std::shared_ptr<Region>(new Region(*(it.second))));
    }
  }
  //DEBUG std::cout << "DEBUG: C-Tor AnalysedModel::AnalysedModel Regions finisehd" << std::endl;
  if (!dm_hsm::DynamicModel::links().empty()) {
    for (const auto& it : DynamicModel::links()) {
      links_.emplace(it.first, std::shared_ptr<Link>(new Link(*(it.second))));
    }
  }
  if (!dm_hsm::DynamicModel::global().empty()) {
    for (const auto& it : DynamicModel::global()) {
      global_.emplace(it.first, std::shared_ptr<Global>(new Global(*(it.second))));
    }
  }
  //std::cout << "DEBUG: END C-Tor AnalysedModel::AnalysedModel" << std::endl;
}

void AnalysedModel::XmlOutput(std::unordered_map<std::string, double >fitness_results, const std::string& outFile,  const bool dynamic_output) {
  //std::cout << "FUNC-ID: AnalysedModel::XmlOutput " << std::endl;
  io_routines::XmlWriter xml_out(outFile);
  if(dynamic_output){//xml root element MODEL_TYPE
    xml_out.addTag("MODEL_DYNAMIC");
    xml_out.addElement("version-info");
      xml_out.addAttr("software_version", VERSION_STRING);
      xml_out.addAttr("software_compile_date", __DATE__);
      xml_out.addData("GENESYS-2");
    xml_out.closeElement();//version-info
    xml_out.addElement("scenario-info");
      xml_out.addAttr("scenario", genesys::CmdParameters::GetScenarioName());
      xml_out.addData("GENESYS-2");
    xml_out.closeElement();//scenario info
  } else {
    xml_out.addTag("MODEL_STATIC");
    xml_out.addElement("version-info");
      xml_out.addAttr("software_version", VERSION_STRING);
      xml_out.addAttr("software_compile_date", __DATE__);
      xml_out.addData("GENESYS-2");
    xml_out.closeElement();//version-info
    xml_out.addElement("scenario-info");
        xml_out.addAttr("scenario", genesys::CmdParameters::GetScenarioName());
        xml_out.addData("GENESYS-2");
    xml_out.closeElement();//scenario info
  }


    for (const auto& it : regions_) {
      xml_out.addElement("region");
      xml_out.addAttr("code", it.first);
        it.second->writeXmlRegion(xml_out, dynamic_output);
      xml_out.closeElement();//Region
    }
    for (const auto& it : links_) {
      xml_out.addElement("link");
      xml_out.addAttr("code", it.first);
      it.second->writeXmlLink(xml_out, dynamic_output);//
      xml_out.closeElement();//Link
    }
    for (const auto& it : global_) {
      xml_out.addElement("global");
      xml_out.addAttr("code", it.first);
      it.second->writeXmlGlobal(xml_out, dynamic_output);//
      xml_out.closeElement();//Global
    }
    xml_out.addElement("model_internal");
		if (fitness_results.find("lcoe_ct/kWh") !=fitness_results.end()){
			xml_out.addAttr("LCOE",   std::to_string(fitness_results.find("lcoe_ct/kWh")->second));
		}
		if (fitness_results.find("energy") !=fitness_results.end()){
			xml_out.addAttr("energy",   std::to_string(fitness_results.find("energy")->second));
		}
		if (fitness_results.find("capex") !=fitness_results.end()){
			xml_out.addAttr("capex",   std::to_string(fitness_results.find("capex")->second));
		}
		if (fitness_results.find("fopex") !=fitness_results.end()){
			xml_out.addAttr("fopex",   std::to_string(fitness_results.find("fopex")->second));
	    }
		if (fitness_results.find("vopex") !=fitness_results.end()){
			xml_out.addAttr("vopex",   std::to_string(fitness_results.find("vopex")->second));
	    }
		if (fitness_results.find("pen_unsupplied_load") !=fitness_results.end()){
			xml_out.addAttr("pen_unsupplied_load",   std::to_string(fitness_results.find("pen_unsupplied_load")->second));
        }
		if (fitness_results.find("pen_self_supply") !=fitness_results.end()){
			xml_out.addAttr("pen_self_supply",   std::to_string(fitness_results.find("pen_unsupplied_load")->second));
	    }
		if (fitness_results.find("fitness") !=fitness_results.end()){
			xml_out.addAttr("fitness",   std::to_string(fitness_results.find("fitness")->second));
	    }
    if (fitness_results.find("analysis_timer_sec") !=fitness_results.end()){
      xml_out.addAttr("analysis_timer_sec",   std::to_string(fitness_results.find("analysis_timer_sec")->second));
      }

      xml_out.addData("ct/kWh, GWh, EUR, EUR, EUR, EUR, EUR, EUR, sec");
    xml_out.closeElement();
    xml_out.addElement("model_internal");
      xml_out.addAttr("code", "annual_electricity_price_EUR/GWh");
      xml_out.addTBD(get_annual_electricity_price_());
    xml_out.closeElement();

  xml_out.closeTag();//MODEL_TYPE
  xml_out.close();
}

void AnalysedModel::write_model_internal(io_routines::XmlWriter xml_out) {

}

} /* namespace analysis_hsm */
