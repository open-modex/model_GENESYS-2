'''
Created on Sep 24, 2019

@author: jou
'''

import csv
import os
import datetime
from xml.dom import minidom

def get_variables(file_to_read):
    print("extracting variables ...")

    # create CSV-file---------------------------------------------------------------------------------------------------
    dirpath = os.getcwd()

    if not os.path.exists(dirpath + '/results/'):
        os.mkdir(dirpath + "\\results")

    if os.path.isfile(dirpath + "/results/variables_genesys.csv"):
        extract_time = str(datetime.datetime.now())
        extract_time = extract_time.replace(':', '-')
        save_path = dirpath + '\\results\\variables_genesys_' + extract_time[2:19] + '.csv'
    else:
        save_path = dirpath + '\\results\\variables_genesys.csv'

    # Benoetigte Daten aus Genesys extrahieren--------------------------------------------------------------------------
    mydoc = minidom.parse(dirpath + '/'+file_to_read)
    converters = mydoc.getElementsByTagName('converter')
    storages = mydoc.getElementsByTagName('storage')
    lines = mydoc.getElementsByTagName('tr-converter')

    number_of_converters = len(converters)
    number_of_storage = len(storages)
    number_of_lines = len(lines)
    number_of_variables = sum([number_of_converters,number_of_storage,number_of_lines])

    # Daten in das CSV-File schreiben-----------------------------------------------------------------------------------
    with open(save_path, 'w', newline='') as csvfile:
        filewriter = csv.writer(csvfile, delimiter=',', quoting=csv.QUOTE_MINIMAL)
        filewriter.writerow(['Number of variables,"#",'+str(number_of_variables)])