'''
Created on Sep 24, 2019

@author: jou
'''

import csv
import os
import datetime
from xml.dom import minidom

def get_investment(file_to_read):
    print("extracting investment ...")

    #create CSV-file---------------------------------------------------------------------------------------------------
    dirpath = os.getcwd()

    if not os.path.exists(dirpath+'/results/'):
        os.mkdir(dirpath + "\\results")

    if os.path.isfile(dirpath+"/results/investments_genesys.csv"):
        extract_time = str(datetime.datetime.now())
        extract_time = extract_time.replace(':','-')
        save_path = dirpath+'\\results\\investments_genesys_'+extract_time[2:19]+'.csv'
    else:
        save_path = dirpath + '\\results\\investments_genesys.csv'

    #Benoetigte Daten aus Genesys extrahieren--------------------------------------------------------------------------
    mydoc = minidom.parse(dirpath+'/'+file_to_read)
    items = mydoc.getElementsByTagName('model_internal')

    allAttributes = list(items[0].attributes._attrs.keys())
    allUnits = items[0].firstChild.data.split(",")

    index_capex = allAttributes.index('capex')
    index_fopex = allAttributes.index('fopex')
    index_vopex = allAttributes.index('vopex')

    capex = items[0].attributes['capex'].value
    fopex = items[0].attributes['fopex'].value
    vopex = items[0].attributes['vopex'].value

    #Daten in das CSV-File schreiben-----------------------------------------------------------------------------------
    with open(save_path, 'w',newline='') as csvfile:
        filewriter = csv.writer(csvfile, delimiter=',', quoting=csv.QUOTE_MINIMAL)
        filewriter.writerow(['Investment capacity of each technology'])
        filewriter.writerow(['capex,' + capex + ',' + allUnits[index_capex]])
        filewriter.writerow(['fopex,' + fopex + ',' + allUnits[index_fopex]])
        filewriter.writerow(['vopex,' + vopex + ',' + allUnits[index_vopex]])