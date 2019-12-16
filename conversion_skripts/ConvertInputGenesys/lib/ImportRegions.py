import os, sys
import csv
import pandas

def f_import(*, dfs, scn, path):
    #function information
    #input arguments:
    #   - scn   : string of the scenario name (exkluding file ending xlsx!)
    #   - path  : folder root path to create parameter files
    # v1.2, cbu
    # v1.0, kja
    ########################################################################
    if not os.path.exists(path+'/regions/'):
        os.mkdir(path+'/regions/')

    # try:
    #     df_m_input = pandas.read_excel('./input/'+scn+'.xlsx', sheet_name='Site')
    # except:
    #     print("failed to read regions in excel input file for scenario: ", scn)
    df_m_input = dfs['Site']

    def f_get_template_region_general(dict_site, index):
        #generate reference to actual region file
        return [['/include(./regions/' + dict_site['id'] +'.csv)']]

    with open(path+'/Region.csv', 'w') as writeFile:
        writer = csv.writer(writeFile, delimiter=";", lineterminator='\n')
        writer.writerows([['#blockwise']])

        for index, row in df_m_input.iterrows():
            # print(row.to_dict())
            writer.writerows(f_get_template_region_general(row.to_dict(), index))

    writeFile.close()
    # except:
    #     print("No sheet for storages!")
