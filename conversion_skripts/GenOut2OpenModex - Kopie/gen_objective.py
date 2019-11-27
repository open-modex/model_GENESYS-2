'''
Created on Sep 24, 2019

@author: jou
'''

import csv
import os
import datetime
from xml.dom import minidom

def get_objective(file_to_read):
    print("extracting objective ...")

    #create CSV-file---------------------------------------------------------------------------------------------------
    dirpath = os.getcwd()

    if not os.path.exists(dirpath+'/results/'):
        os.mkdir(dirpath + "\\results")

    if os.path.isfile(dirpath+"/results/objective_genesys.csv"):
        extract_time = str(datetime.datetime.now())
        extract_time = extract_time.replace(':','-')
        save_path = dirpath+'\\results\\objective_genesys_'+extract_time[2:19]+'.csv'
    else:
        save_path = dirpath + '\\results\\objective_genesys.csv'

    #Benoetigte Daten aus Genesys extrahieren--------------------------------------------------------------------------
    mydoc = minidom.parse(dirpath+'/'+file_to_read)
    items = mydoc.getElementsByTagName('model_internal')

    allAttributes = list(items[0].attributes._attrs.keys())
    allUnits = items[0].firstChild.data.split(",")

    index_LCOE = allAttributes.index('LCOE')
    index_capex = allAttributes.index('capex')

    LCOE = items[0].attributes['LCOE'].value
    capex = float(items[0].attributes['capex'].value)
    fopex = float(items[0].attributes['fopex'].value)
    vopex = float(items[0].attributes['vopex'].value)
    objective_value = capex+fopex+vopex

    #Daten in das CSV-File schreiben-----------------------------------------------------------------------------------
    with open(save_path, 'w',newline='') as csvfile:
        filewriter = csv.writer(csvfile, delimiter=',', quoting=csv.QUOTE_MINIMAL)
        filewriter.writerow(['Levelized Cost of Energy,'+LCOE+', '+allUnits[index_LCOE]])
        filewriter.writerow(['Objective value,' + str(objective_value) + ', ' + allUnits[index_capex]])
