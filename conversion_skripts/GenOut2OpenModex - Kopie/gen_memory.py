'''
Created on Sep 24, 2019

@author: jou
'''

import csv
import os
import datetime
from xml.dom import minidom

def get_memory():
    print("extracting memory ...")

    # create CSV-file---------------------------------------------------------------------------------------------------
    dirpath = os.getcwd()

    if not os.path.exists(dirpath + '/results/'):
        os.mkdir(dirpath + "\\results")

    if os.path.isfile(dirpath + "/results/memory_genesys.csv"):
        extract_time = str(datetime.datetime.now())
        extract_time = extract_time.replace(':', '-')
        save_path = dirpath + '\\results\\memory_genesys_' + extract_time[2:19] + '.csv'
    else:
        save_path = dirpath + '\\results\\memory_genesys.csv'

    # Daten in das CSV-File schreiben-----------------------------------------------------------------------------------
    with open(save_path, 'w', newline='') as csvfile:
        filewriter = csv.writer(csvfile, delimiter=',', quoting=csv.QUOTE_MINIMAL)
        filewriter.writerow(['Memory usage negligible as model is highly dynamic'])