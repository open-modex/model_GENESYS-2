import csv
import pandas as pd

def f_import(dfs, scn, path, start_date='2030-01-01_00:00'):

    b_use_transmission = True
    try:
        df_m_input = dfs['Transmission']
        df_m_input = df_m_input.iloc[range(int(df_m_input.shape[0] / 2)),]
    except:
        print("\tWARNING: no Transmission found in scenario", scn)
        b_use_transmission = False


    # try:
    #df_m_input = pandas.read_excel('./input/om-threenode-storage-transmission.xlsx', sheet_name='Transmission')



    def f_get_template_transmission(dict_transmissionconverter):

        # if 'length_dep_loss' not in dict_transmissionconverter:
        dict_transmissionconverter['length_dep_loss'] = 0
        # if 'length_dep_cost' not in dict_transmissionconverter:
        dict_transmissionconverter['length_dep_cost'] = 0

        return [['#code',dict_transmissionconverter['Transmission'].replace(" ", "_")+'_'+dict_transmissionconverter['Site In'].replace(" ", "_")+'_'+dict_transmissionconverter['Site Out'].replace(" ", "_").replace(" ", "_"),'#name',dict_transmissionconverter['Transmission'].replace(" ", "_")+'_'+dict_transmissionconverter['Commodity'].replace(" ", "_").replace(" ", "_"),'#input',dict_transmissionconverter['Commodity'],'#output',dict_transmissionconverter['Commodity'],'#bidirectional','true'],
                ['efficiency_new', '#type', 'DVP_linear', '#data', str(start_date), str(dict_transmissionconverter['eff'])],
                ['cost', '#type', 'DVP_linear', '#data', str(start_date), str(dict_transmissionconverter['inv-cost']*1000)],
                ['lifetime', '#type', 'DVP_linear', '#data', str(start_date), str(dict_transmissionconverter['depreciation'])],
                ['OaM_rate', '#type', 'DVP_linear', '#data', str(start_date), str(dict_transmissionconverter['fix-cost']/float(dict_transmissionconverter['inv-cost']))],
                ['length_dep_loss', '#type', 'DVP_linear', '#data', str(start_date), str(dict_transmissionconverter['length_dep_loss']*1000)],
                ['length_dep_cost', '#type', 'DVP_linear', '#data', str(start_date), str(dict_transmissionconverter['length_dep_cost']*1000)],
                ['#endblock']]

    with open(path+'/TransmissionConverter.csv', 'w') as writeFile:
        writer = csv.writer(writeFile, delimiter=";", lineterminator='\n')
        writer.writerows([['#comment', '===============Transmission Converters============================================']])

        if(b_use_transmission):
            writer.writerows([['#blockwise']])
            for index, row in df_m_input.iterrows():
                # print(row.to_dict())
                writer.writerows(f_get_template_transmission(row.to_dict()))
        else:
            writer.writerows([['#empty']])

    writeFile.close()
    # except:
    #     print("No sheet for storages!")
