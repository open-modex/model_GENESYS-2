import os, sys
import csv
import pandas



def f_import(*, dfs, scn, path, b_MWh_to_GWh=True):
    # function information
    # input arguments:
    #   - scn   : string of the scenario name (excluding file ending xlsx!)
    #   - path  : folder root path to create parameter files
    # returns:
    #   - d_annual_demands  : dict of annual demand per region
    # v1.2, cbu
    # v1.0, kja
    ########################################################################

    #define return-dict
    d_annual_demands = {}

    if not os.path.exists(path + '/TimeSeries/'):
        os.mkdir(path + '/TimeSeries/')

    # try: # open input excel file and export sheet demand into df
    #     df_m_input = pandas.read_excel('./input/'+scn+'.xlsx', sheet_name='Demand', index_col=0)
    # except:
    #     print("failed to read demand in excel input file for scenario: ", scn)
    df_m_input = dfs['Demand']

    for col in df_m_input:
        total_demand = df_m_input[col].sum()
        if not ((df_m_input[col].__len__() -1) == 8760):
            print("ERROR: Assumed length of demand time series is 8760 steps / function not defined for multi-or-sub-annual time-series")
            print("\tFound length: ",df_m_input[col].__len__()  -1)
            sys.exit()

        with open(path+'/TimeSeries/' + col.replace(".", "_demand_") + '.csv', 'w') as writeFile:
            writer = csv.writer(writeFile, delimiter=";", lineterminator='\n')
            #convert from MWh to GWh?
            #normalisation of time-series to 1 for 1 year
            if b_MWh_to_GWh and total_demand > 0.:
                d_annual_demands[col] = total_demand / 1000
                writer.writerow((df_m_input[col] / 1000 / total_demand).to_list())
            elif total_demand > 0:
                d_annual_demands[col] = total_demand
                writer.writerow((df_m_input[col]/total_demand).to_list())
            else:
                print("ERROR: total annual demand = 0 // div by zero error!")
                sys.exit()


        writeFile.close()
        # except:
        #     print("No sheet for storages!")

    return d_annual_demands
