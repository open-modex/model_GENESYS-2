import csv
import pandas
import math

def f_import(dfs, scn, path, start_date='2030-01-01_00:00'):

    df_m_input_commodity = dfs['Commodity']
    # try:
    #df_m_input_commodity = pandas.read_excel('./input/om-threenode-storage-transmission.xlsx', sheet_name='Commodity')

    def f_get_template_primary_energy(df_m_input_commodity):

        if 'max' not in df_m_input_commodity:
            df_m_input_commodity['max'] = 1000000
        else:
            if math.isnan(df_m_input_commodity['max']) or df_m_input_commodity['max'] == math.inf:
                df_m_input_commodity['max'] = 1000000

        if 'price' not in df_m_input_commodity:
            df_m_input_commodity['price'] = 0
        else:
            if math.isnan(df_m_input_commodity['price']):
                df_m_input_commodity['max'] = 0

        if df_m_input_commodity['Commodity'] == 'Solar' or df_m_input_commodity['Commodity'] == 'Wind' or df_m_input_commodity[
            'Commodity'] == 'Run of River':
            return [['#code', df_m_input_commodity['Commodity'].replace(" ", "_") + '_' + df_m_input_commodity['Site'].replace(" ", "_"), '#name',
                     df_m_input_commodity['Commodity'].replace(" ", "_") + '_' + df_m_input_commodity['Site'].replace(" ", "_")],
                    ['cost_table', '#type', 'TBD_lookupTable'],
                    ['base', '#type', 'DVP_const', '#data', str(start_date), str(1)],
                    ['value', '#type', 'DVP_const', '#data', str(start_date), str(0)],
                    ['#endtable'],
                    ['#endblock']]
        else:
            return [['#code', df_m_input_commodity['Commodity'].replace(" ", "_") + '_' + df_m_input_commodity['Site'].replace(" ", "_"), '#name',
                     df_m_input_commodity['Commodity'].replace(" ", "_") + '_' + df_m_input_commodity['Site'].replace(" ", "_")],
                    ['cost_table', '#type', 'TBD_lookupTable'],
                    ['base', '#type', 'DVP_const', '#data',str(start_date), str(df_m_input_commodity['max'])],
                    ['value', '#type', 'DVP_const', '#data', str(start_date), str(df_m_input_commodity['price']*1000)],
                    ['#endtable'],
                    ['#endblock']]

    with open(path+'/PrimaryEnergy.csv', 'w') as writeFile:
        writer = csv.writer(writeFile, delimiter=";", lineterminator='\n')
        writer.writerows([['#comment', '===============Primary Energy============================================']])
        writer.writerows([['#blockwise']])

        for index, row in df_m_input_commodity.iterrows():
            # print(row.to_dict())
            df_m_input_commodity = row.to_dict()
            if df_m_input_commodity['Commodity'] != 'Elec':
                writer.writerows(f_get_template_primary_energy(df_m_input_commodity))

    writeFile.close()
    # except:
    #     print("No sheet for primary_energys!")
