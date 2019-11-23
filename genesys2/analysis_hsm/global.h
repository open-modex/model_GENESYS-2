/*
 * global.h
 *
 *  Created on: May 25, 2017
 *      Author: cbu_loc
 */

#ifndef ANALYSIS_HSM_GLOBAL_H_
#define ANALYSIS_HSM_GLOBAL_H_

#include <memory>
#include <unordered_map>

#include <dynamic_model_hsm/global.h>
#include <analysis_hsm/primary_energy.h>
#include <analysis_hsm/storage.h>
#include <io_routines/xml_writer.h>


namespace analysis_hsm {

class Global : public dm_hsm::Global {
 public:
  Global() = delete;
  virtual ~Global() override = default;
  Global(const Global& other) = delete; // if this copy c'tor is needed, special implementation is necessary, taking care of correct handling of the weak pointers of the converter
  Global(Global&& other) = delete; //  if this move c'tor is needed, special implementation is necessary, taking care of correct handling of the weak pointers of the converter
  Global& operator=(const Global&) = delete;
  Global& operator=(Global&&) = delete;
  Global(const dm_hsm::Global& origin);

  /** \name Output Interface */
  ///@{
  void writeXmlGlobal(io_routines::XmlWriter& xmlout, bool dynamic_output = false);

 private:
  void writeXmlElement(io_routines::XmlWriter& xml_target, std::string data_code, const aux::TimeSeriesConst& dataObj);
  std::unordered_map<std::string, std::shared_ptr<PrimaryEnergy> > primary_energy_ptrs_;
  std::unordered_map<std::string, std::shared_ptr<Storage> > storage_ptrs_;
};

} /* namespace analysis_hsm */

#endif /* ANALYSIS_HSM_GLOBAL_H_ */
