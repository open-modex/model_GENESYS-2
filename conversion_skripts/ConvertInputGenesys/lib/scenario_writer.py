import sys
import pandas as pd
import lib.ImportMultiConverter
import lib.ImportConverter
import lib.ImportDemand
import lib.ImportLink
import lib.ImportPrimaryEnergy
import lib.ImportRegionComponents
import lib.ImportRegions
import lib.ImportStorage
import lib.ImportSupIm
import lib.ImportTransmissionConverter

import lib.WriteProgramSettings
import lib.WriteInstallationListResult
import lib.WriteShellScript
import lib.WritePrimaryEnergyUnlimited

from lib import ts_tools as tto

@tto.timer
def generate_scenario_param(str_scenario, str_path, i_year_start, i_year_end):
    print('writing screnario', str_scenario)

    b_conversion_to_GWh = True
    str_start_date = str(i_year_start)+'-01-01_00:00'
    print('\tINFO: Using global start date: ', str_start_date)

    try:
        # read excel file once!
        xlsx = pd.ExcelFile('./input/' + str_scenario + '.xlsx')
        # print(xlsx)
        # print(xlsx.sheet_names)
        scenario_data = {sheet_name: xlsx.parse(sheet_name) for sheet_name in xlsx.sheet_names}
    except:
        print("ERROR could not read input excel file for scenario ", str_scenario)
        sys.exit()

    #write main scenario parametrisation
    lib.ImportRegions.f_import(dfs=scenario_data, scn= str_scenario, path=str_path)
    dict_region_demand = lib.ImportDemand.f_import(dfs=scenario_data, scn= str_scenario, path=str_path, b_MWh_to_GWh=b_conversion_to_GWh)
    lib.ImportRegionComponents.f_import(dfs=scenario_data, scn= str_scenario, path=str_path, region_demand=dict_region_demand, start_date=str_start_date)
    lib.ImportSupIm.f_import(dfs=scenario_data, scn= str_scenario, path=str_path, start_date=str_start_date)
    lib.ImportMultiConverter.f_import(dfs=scenario_data, scn= str_scenario, path=str_path, start_date=str_start_date)
    lib.ImportConverter.f_import(dfs=scenario_data, scn= str_scenario, path=str_path, start_date=str_start_date)
    lib.ImportLink.f_import(dfs=scenario_data, scn=str_scenario, path=str_path, start_date=str_start_date)
    lib.ImportPrimaryEnergy.f_import(dfs=scenario_data, scn=str_scenario, path=str_path, start_date=str_start_date)
    lib.ImportStorage.f_import(dfs=scenario_data, scn=str_scenario, path=str_path, start_date=str_start_date)
    lib.ImportTransmissionConverter.f_import(dfs=scenario_data, scn=str_scenario, path=str_path, start_date=str_start_date)

    #write additional files
    lib.WriteProgramSettings.f_write(scn=str_scenario, path=str_path, start_date=str_start_date, end_date=str(i_year_end)+'-12-31_00:00')
    lib.WriteShellScript.f_write(path=str_path)
    lib.WriteInstallationListResult.f_write(path=str_path)
    lib.WritePrimaryEnergyUnlimited.f_write(path=str_path, start_date=str_start_date)