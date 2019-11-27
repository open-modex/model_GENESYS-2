import sys
import csv
import pandas as pd

def f_import(*, dfs, scn, path, region_demand, start_date='2030-01-01_00:00'):
    #define input-sheet dataframes from global df
    df_converter = dfs['Process']
    df_region = dfs['Site']
    df_commodity =dfs['Commodity']
    #storage is an optional setting!
    b_use_storage = True
    try:
        df_storage = dfs['Storage']
    except:
        b_use_storage = False
        print("\tINFO: No definition of STORAGE found in scenario ", scn)



    def f_get_template_demand(str_region, demands, start_date):
        ####################################################################
        # input parameters
        # - str_region  : string of region code
        # - demands     : dict with regional demands for heat and electric
        # output
        # - return      : list of lists(lines) including strings
        # v1.1 - cbu
        # v1.0 - kja
        ####################################################################
        #DEBUG: print(str_region, "demand =", demands)
        if demands["heat"]>0:
            print("ERROR: connot process heat demand / need to extend script!")
            sys.exit()
        else:
            #print("demand generator : #data", start_date)
            return [['#code',str_region,'#name',str_region],
                ['demand_electric_dyn', '#type', 'TS_repeat_const', '#interval', '1h', '#start', str(start_date), '#data_source_path', './TimeSeries/'+str_region+'_demand_Elec.csv'],
                ['demand_electric_per_a', '#type', 'DVP_linear', '#data', str(start_date), str(demands['electric'])]]

    def f_get_template_primary_energy(df_m_input_commodity, str_region="test-region"):
        #add primary energy only if Site matches the current region!
        if df_m_input_commodity['Site'] == str_region:
            #print("region =", str_region)
            #print(df_m_input_commodity)
            if df_m_input_commodity['Commodity'] == 'Solar' or df_m_input_commodity['Commodity'] == 'Wind' or df_m_input_commodity[
                'Commodity'] == 'Run of River':
                return [['#primary_energy', '#code', df_m_input_commodity['Commodity'].replace(" ", "_") + '_' + df_m_input_commodity['Site'].replace(" ", "_")],
                        ['potential', '#type', 'TBD_lookupTable', '#data_source_path', './TimeSeries/'+df_m_input_commodity['Site']+'_'+df_m_input_commodity['Commodity']+'.csv']]
            else:
                return [['#primary_energy', '#code', df_m_input_commodity['Commodity'].replace(" ", "_") + '_' + df_m_input_commodity['Site'].replace(" ", "_")],
                        ['potential', '#type', 'TBD_lookupTable', '#data_source_path', './TimeSeries/PrimaryEnergyUnlimited_minusOne.csv']]

        else:
            return[]

    def f_get_template_converter(df_m_input_converter, start_date, str_region="test-region"):
        #print(df_m_input_converter["Site"], str_region)
        # add only if Site matches the current region!
        if df_m_input_converter['Site'] == str_region:
            if 'inst-cap' not in df_m_input_converter:
                dict_converter_location['inst-cap'] = 0

            return [['#converter', '#code', dict_converter_location['Process'].replace(" ", "_")+'_'+dict_converter_location['Site'].replace(" ", "_")],
                    ['installation', '#type', 'DVP_const', '#data', str(start_date), str(dict_converter_location['inst-cap']/1000)]]
        else:
            return[]

    def f_get_template_storage_converter(dict_storage, start_date, str_region="test-region"):
        # add only if Site matches the current region!
        if dict_storage['Site'] == str_region:

            if 'inst-cap-c' not in dict_storage:
                dict_storage['inst-cap-c'] = 0

            return [['#converter', '#code', 'converter'+'_'+dict_storage['Storage'].replace(" ", "_")+'_'+dict_storage['Site'].replace(" ", "_")],
                    ['installation', '#type', 'DVP_const', '#data', str(start_date), str(dict_storage['inst-cap-c']/1000)]]
        else:
            return[]

    def f_get_template_multiconverter(df_m_input_converter, start_date, str_region="test-region"):
        # add only if Site matches the current region!
        if df_m_input_converter['Site'] == str_region:
            #print(df_m_input_converter['Site'] , "multi-converter")

            if 'inst-cap' not in df_m_input_converter:
                print('inst-cap not found for multi-converter!', df_m_input_converter['Process'])
                df_m_input_converter['inst-cap'] = 0

            return [['#multi-converter', '#code', df_m_input_converter['Process'].replace(" ", "_")+'_'+df_m_input_converter['Site'].replace(" ", "_")],
                    ['installation', '#type', 'DVP_const', '#data', str(start_date), str(df_m_input_converter['inst-cap']/1000)]]
        else:
           return[]

    def f_get_template_storage(dict_storage, start_date, str_region="test-region"):
        #print(__name__, 'f_get_template_storage')

        # add only if Site matches the current region!
        if dict_storage['Site'] == str_region:
            #print('storage in region ' , dict_storage['Site'] )
            if 'inst-cap-c' not in dict_storage:
                dict_storage['inst-cap-c'] = 0

            return [['#storage', '#code', dict_storage['Storage'].replace(" ", "_")+'_'+dict_storage['Site'].replace(" ", "_")],
                    ['installation', '#type', 'DVP_const', '#data', str(start_date), str(dict_storage['inst-cap-c']/1000)]]
        else:
            return[]


    def f_get_region_demands(str_region, region_demands):
        ####################################################################
        # input parameters
        # - str_region      : string of region code
        # - region_demands  : dict with regional demands for heat and electric
        # output
        # - return      : dict with heat and electric demand for selected region str_region
        # v1.0 - cbu
        ####################################################################
        demands = {"electric": 0, "heat":0}

        for key, val in region_demands.items():
            if key.find('Elec' or 'elec'):
                if not key.find(str_region): # i am not sure why here >if not< is the correct choice! cbu
                    # print(key, val)
                    demands["electric"] = val
            elif key.find('heat' or 'HEAT' or 'Heat'):
                if not key.find(str_region):
                    demands["heat"] = val
            else:
                print("no valid key found")
        return demands

    dict_regions = df_region.to_dict()
    #print("\tDEBUG: ",path)

    for index in dict_regions['Name']:
      region_code = dict_regions['Name'][index]
      try:
          with open(path+'/regions/'+dict_regions['Name'][index]+'.csv', 'w') as writeFile:
            writer = csv.writer(writeFile, delimiter=";", lineterminator='\n')
            #write demand rows in region parametrisation file
            writer.writerows(f_get_template_demand(region_code, f_get_region_demands(region_code, region_demand), start_date=start_date))

            for index, row  in df_commodity.iterrows():
                #print(row.to_dict())
                dict_temp = row.to_dict()
                #print(dict_temp['Commodity'] )
                if dict_temp['Commodity'] != 'Elec':
                    writer.writerows(f_get_template_primary_energy(dict_temp, str_region=region_code))

            for index, row in df_converter.iterrows():
                #print("index,row = ", index, row)
                # print(row.to_dict())
                dict_converter_location = row.to_dict()
                if dict_converter_location['Process'] != 'Curtailment' and (
                        dict_converter_location['Process'] == 'Photovoltaics' or dict_converter_location['Process'] == 'Wind' or dict_converter_location[
                    'Process'] == 'Run of River'):
                    writer.writerows(f_get_template_converter(dict_converter_location, str_region=region_code, start_date=start_date))

            if(b_use_storage):
                for index, row in df_storage.iterrows():
                    # print(row.to_dict())
                    #print('str_region', region_code)
                    writer.writerows(f_get_template_storage_converter(row.to_dict(),str_region=region_code, start_date=start_date))

            for index, row in df_converter.iterrows():
                # print(row.to_dict())
                dict_converter_location = row.to_dict()
                if dict_converter_location['Process'] != 'Curtailment' and dict_converter_location['Process'] != 'Photovoltaics' and dict_converter_location['Process'] != 'Wind' and dict_converter_location['Process'] != 'Run of River':
                    writer.writerows(f_get_template_multiconverter(dict_converter_location, str_region=region_code, start_date=start_date))

            if(b_use_storage):
                #print('storage in region', region_code)
                for index, row in df_storage.iterrows():
                    #print(row.to_dict())
                    writer.writerows(f_get_template_storage(row.to_dict(), start_date=start_date, str_region=region_code))

            writer.writerows([['#endblock']])
      except:
         print("ERROR in ", __name__, " could not open file for writing in ", path)
    writeFile.close()
    # except:
    #     print("No sheet for multiconverters!")
