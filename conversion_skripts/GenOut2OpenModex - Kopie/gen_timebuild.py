'''
Created on Sep 24, 2019

@author: jou
'''

import csv
import os
import datetime
from xml.dom import minidom

def get_timebuild(file_to_read):
    print("extracting timebuild ...")

    # create CSV-file---------------------------------------------------------------------------------------------------
    dirpath = os.getcwd()

    if not os.path.exists(dirpath + '/results/'):
        os.mkdir(dirpath + "\\results")

    if os.path.isfile(dirpath + "/results/timebuild_genesys.csv"):
        extract_time = str(datetime.datetime.now())
        extract_time = extract_time.replace(':', '-')
        save_path = dirpath + '\\results\\timebuild_genesys_' + extract_time[2:19] + '.csv'
    else:
        save_path = dirpath + '\\results\\timebuild_genesys.csv'

    # Benoetigte Daten aus Genesys extrahieren--------------------------------------------------------------------------
    mydoc = minidom.parse(dirpath + '/'+file_to_read)
    items = mydoc.getElementsByTagName('model_internal')

    allAttributes = list(items[0].attributes._attrs.keys())
    allUnits = items[0].firstChild.data.split(",")

    index_analysis_timer_sec = allAttributes.index('analysis_timer_sec')

    analysis_timer_sec = items[0].attributes['analysis_timer_sec'].value

    # Daten in das CSV-File schreiben-----------------------------------------------------------------------------------
    with open(save_path, 'w', newline='') as csvfile:
        filewriter = csv.writer(csvfile, delimiter=',', quoting=csv.QUOTE_MINIMAL)
        filewriter.writerow(['Time to simulate the model,' + analysis_timer_sec + ',' +
        allUnits[index_analysis_timer_sec].replace('\n','')])