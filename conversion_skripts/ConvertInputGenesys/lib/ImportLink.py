import csv
import pandas


def f_import(dfs, scn, path, start_date='2030-01-01_00:00'):
    # try:
    #df_m_input = pandas.read_excel('./input/om-threenode-storage-transmission.xlsx', sheet_name='Transmission')

    b_use_transmission = True
    try:
        df_m_input = dfs['Transmission']
        df_m_input = df_m_input.iloc[range(int(df_m_input.shape[0] / 2)),]
    except:
        print("\tWARNING: no Transmission found in scenario", scn)
        b_use_transmission = False



    #df_m_input = dfs['Transmission']



    def f_get_template_transmission(dict_link):

        if 'length' not in dict_link:
            dict_link['length'] = 0

        return [['#code',dict_link['Site In'].replace(" ", "_")+'_'+dict_link['Site Out'].replace(" ", "_").replace(" ", "_"),'#region_A',dict_link['Site In'],'#region_B',dict_link['Site Out']],
                ['length', '#type', 'DVP_const', '#data', str(start_date), str(dict_link['length'])],
                ['#converter', '#code', dict_link['Transmission'].replace(" ", "_")+'_'+dict_link['Site In'].replace(" ", "_")+'_'+dict_link['Site Out'].replace(" ", "_").replace(" ", "_")],
                ['installation', '#type', 'DVP_const', '#data', str(start_date), str(dict_link['inst-cap']/1000)],
                ['#endblock']]

    with open(path+'/Link.csv', 'w') as writeFile:
        writer = csv.writer(writeFile, delimiter=";", lineterminator='\n')
        writer.writerows([['#comment', '===============LINK Parameterisation============================================']])

        if(b_use_transmission):
            writer.writerows([['#blockwise']])
            for index, row in df_m_input.iterrows():
                # print(row.to_dict())
                writer.writerows(f_get_template_transmission(row.to_dict()))
        else:
            #in case there are no links to be defined, the one single line needs to contain an #empty flag.
            writer.writerows([['#empty']])


    writeFile.close()
    # except:
    #     print("No sheet for storages!")
