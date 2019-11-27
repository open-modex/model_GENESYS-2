'''
Created on Sep 24, 2019

@author: jou
'''

import csv
import os
import datetime

def get_constraints():
    print("extracting constraints ...")

    # create CSV-file---------------------------------------------------------------------------------------------------
    dirpath = os.getcwd()

    if not os.path.exists(dirpath + '/results/'):
        os.mkdir(dirpath + "\\results")

    if os.path.isfile(dirpath + "/results/constraints_genesys.csv"):
        extract_time = str(datetime.datetime.now())
        extract_time = extract_time.replace(':', '-')
        save_path = dirpath + '\\results\\constraints_genesys_' + extract_time[2:19] + '.csv'
    else:
        save_path = dirpath + '\\results\\constraints_genesys.csv'

    # Daten in das CSV-File schreiben-----------------------------------------------------------------------------------
    with open(save_path, 'w', newline='') as csvfile:
        filewriter = csv.writer(csvfile, delimiter=',', quoting=csv.QUOTE_MINIMAL)
        filewriter.writerow(['Number of constraints,"#",none as genesys is not a linear model'])