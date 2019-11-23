/*
 * global.cc
 *
 *  Created on: May 25, 2017
 *      Author: cbu_loc
 */

#include <analysis_hsm/global.h>

namespace analysis_hsm {

Global::Global(const dm_hsm::Global& origin)
    : dm_hsm::Global(origin) {
  //  std::cout << "DEBUG: C-Tor an::Global::Global" << std::endl;
  if (!storage_ptrs_dm().empty()) {
    std::cout << "found global energy storage in dm..." << std::endl;
    for (const auto& it : storage_ptrs_dm()) {
        if (auto storage = std::dynamic_pointer_cast<dm_hsm::Storage>(it.second)) {
          storage_ptrs_.emplace(it.first, std::shared_ptr<Storage>(new Storage(*storage)));
          //std::cout << "copy2 storage_an = " << storage_ptrs_.find(it.first)->second->get_fopex().PrintToString() << std::endl;
        } else {
          storage_ptrs_.emplace(it.first, std::shared_ptr<analysis_hsm::Storage>(
                      new analysis_hsm::Storage(static_cast<dm_hsm::Storage>(*it.second)))); // static_cast is necessary because of multiple inheritance
        }
    }
  } else {
    std::cout << "\t***info NO global storage ptrs found in dm" << std::endl;
  }
  if(!primary_energy_ptrs_dm().empty()) {
    //std::cout << "found global primary energy in dm..." << std::endl;
    for (const auto& it : primary_energy_ptrs_dm())
      primary_energy_ptrs_.emplace(it.first, std::shared_ptr<PrimaryEnergy>(new PrimaryEnergy(*it.second)));
  } else std::cout << "\t***info NO global primary energy ptrs found in dm" << std::endl;

  //std::cout << "DEBUG: END C-Tor an::Global::Global" << std::endl;
}

void Global::writeXmlGlobal(io_routines::XmlWriter& xmlout, bool dynamic_output) {
  //  std::cout << "FUNC-ID: Global::writeXmlGlobal" << std::endl;
  //STORAGES
    if(!storage_ptrs_.empty()) {
      //std::cout << "global write xml for storage" << std::endl;
      for (const auto& it : storage_ptrs_) {
                xmlout.addElement("storage");
                xmlout.addAttr("code", it.first);
                it.second->writeXmlVariables(xmlout, dynamic_output);
                xmlout.closeElement();//primary energy
      }
    }

  //PRIMARY_ENERGIES
    if (dynamic_output) {
      if(!primary_energy_ptrs_.empty()) {
            //std::cout << "global write xml for primary energy" << std::endl;
            for (const auto& it : primary_energy_ptrs_) {
                      xmlout.addElement("primary_energy");
                      xmlout.addAttr("code", it.first);
                      it.second->writeDynamicVariables(xmlout);
                      xmlout.closeElement();//primary energy
            }
      }
    }

}

void Global::writeXmlElement(io_routines::XmlWriter& xml_target, std::string data_code, const aux::TimeSeriesConst& dataObj) {
  xml_target.addSubElement("data");
    xml_target.addAttr("code", data_code);
    xml_target.addTBD(dataObj);
  xml_target.closeSubElement();
}

} /* namespace analysis_hsm */
