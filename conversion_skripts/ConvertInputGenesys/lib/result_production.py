'''
Created on Sep 24, 2019

@author: jou
'''

import csv
import os
import datetime
from xml.dom import minidom
from datetime import datetime as dt
import pandas as pd
import numpy as np

date_format = '%Y-%m-%d %H:%M:%S'

def get_production(mydoc_xml):
    print("extracting production ...")

    # create CSV-file---------------------------------------------------------------------------------------------------
    dirpath = os.getcwd()

    if not os.path.exists(dirpath + '/results/'):
        os.mkdir(dirpath + "/results")

    if os.path.isfile(dirpath + "/results/production_genesys.csv"):
        extract_time = str(datetime.datetime.now())
        extract_time = extract_time.replace(':', '-')
        save_path = dirpath + '/results/production_genesys_' + extract_time[2:19] + '.csv'
    else:
        save_path = dirpath + '/results/production_genesys.csv'

    # Benoetigte Daten aus Genesys extrahieren--------------------------------------------------------------------------
    mydoc = mydoc_xml
    converters = mydoc.getElementsByTagName('converter')
    storages = mydoc.getElementsByTagName('storage')
    internal_data = mydoc.getElementsByTagName('model_internal')
    all_gen_elements = converters+storages
    full_dataframe = []
    float_conv_data = []

    for ctr_converters in range(len(all_gen_elements)):
        #extract values from xml
        converter_typ = all_gen_elements[ctr_converters].attributes._attrs['code']._value
        converter_interval = all_gen_elements[ctr_converters].childNodes[8].nextSibling._attrs['interval'].value
        converter_start = all_gen_elements[ctr_converters].childNodes[8].nextSibling._attrs['start'].value
        converter_data = all_gen_elements[ctr_converters].childNodes[9].childNodes[0].data.split(',')
        init_start = internal_data[1]._attrs['start']._value

        #convert values from xml
        conv_start_converted = str(dt.strptime(converter_start, '%Y-%m-%d_%H:%M'))
        init_start_converted = str(dt.strptime(init_start, '%Y-%m-%d_%H:%M'))
        number_of_entries = len(converter_data)
        try:
            time_series_tmp = pd.date_range(conv_start_converted, freq=converter_interval, periods=number_of_entries-1)
        except:
            time_series_tmp = pd.date_range(init_start_converted, freq='S',periods=1)
            print(converter_typ + ' cannot be analysed - error in solution of data')

        #extract data
        for entry in converter_data[0:len(converter_data)-1]:
            # Factor 1000 to Convert from GWh to MWh
            float_conv_data.append(float(entry)*1000)

        if ctr_converters == 0:
            full_dataframe = pd.DataFrame({converter_typ: float_conv_data})
            full_dataframe.index = time_series_tmp
        else:
            temp_dataframe = pd.DataFrame({converter_typ: float_conv_data})
            temp_dataframe.index = time_series_tmp
            #full_dataframe = pd.concat([full_dataframe,temp_dataframe], ignore_index=False, axis=1)
            full_dataframe = pd.concat([full_dataframe, temp_dataframe],axis=1,sort=False)

        #reset der variablen
        float_conv_data = []

    # Daten in das CSV-File schreiben-----------------------------------------------------------------------------------
    with open(save_path, 'w', newline='') as csvfile:
        filewriter = csv.writer(csvfile, delimiter=',', quoting=csv.QUOTE_MINIMAL)
        testN = ",".join(str(x) for x in list(full_dataframe))
        filewriter.writerow(['Production per timestep in MWh, '+testN])
        for ctr_data_points in full_dataframe.iterrows():
            datestamp_point = str(ctr_data_points[0])
            data_point = str(list(ctr_data_points[1])).replace('[','').replace(']','')
            filewriter.writerow([datestamp_point+','+data_point])