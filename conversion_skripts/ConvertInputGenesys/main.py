import os, sys
import time

import lib.scenario_selector
import lib.scenario_writer
from lib import ts_tools as tto


if not os.path.exists('./output/'):
    os.mkdir('./output/')

l_scenarios = lib.scenario_selector.generate_scenario_list('./input')
print("Available scenarios in './input/': \n", l_scenarios)

#DEFINE global start and end year for installation and operation
i_year_start = 2020
i_year_end = 2020

if i_year_start>i_year_end:
    print("Error: End year before start year")
    sys.exit()


for scenario in l_scenarios:
    str_scenario_path = './output/'+scenario
    if not os.path.exists(str_scenario_path):
        os.mkdir(str_scenario_path)
    else:
        #write new folder with current time stamp
        print("WARNING: Path ", str_scenario_path, "already existing")
        now = str(time.strftime("%Y%m%d-%H%M%S"))
        str_scenario_path = str_scenario_path+now
        print('\t using new path:', str_scenario_path)
        os.mkdir(str_scenario_path)
    lib.scenario_writer.generate_scenario_param(scenario, str_scenario_path, i_year_start, i_year_end)
