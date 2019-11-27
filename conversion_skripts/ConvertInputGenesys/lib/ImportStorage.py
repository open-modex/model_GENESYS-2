import csv
import pandas


def f_import(dfs, scn, path, start_date='2030-01-01_00:00'):

    b_use_storage = True
    try:
        #df_m_input_storage = dfs['Storage']
        df_m_input = dfs['Storage']
    except:
        print("\tWARNING: no Storage found in scenario", scn)
        b_use_storage = False
        #return
    # try:
    #df_m_input = pandas.read_excel('./input/om-threenode-storage-transmission.xlsx', sheet_name='Storage')
    #df_m_input_storage = pandas.read_excel('./input/om-threenode-storage-transmission.xlsx', sheet_name='Storage')

    def f_get_template_storage(dict_storage, index):

        # print(dict_storage['fix-cost-p'])
        # print(dict_storage['inv-cost-p'])
        #
        # om_cost_rate = dict_storage['fix-cost-p'] / dict_storage['inv-cost-p']
        # print (om_cost_rate, "%")

        if 'discharge' not in dict_storage:
            dict_storage['discharge'] = 0
        if 'inv-cost-p' not in dict_storage:
            dict_storage['inv-cost-p'] = 0
        if 'depreciation' not in dict_storage:
            dict_storage['d_lifetime'] = 20
            dict_storage['depreciation'] = 20
        if 'd_oam_rate' not in dict_storage:
            try:
                dict_storage['d_oam_rate'] = dict_storage['fix-cost-c'] / dict_storage['inv-cost-c']
                #print('storage fix cost', dict_storage['fix-cost-c'])
            except ZeroDivisionError:
                dict_storage['d_oam_rate'] = 0.01*dict_storage['inv-cost-c']
                #print('storage inv cost', dict_storage['inv-cost-c'])
        #else:
            #print('****already found OM Rate:', dict_storage['d_oam_rate'])

        storage = dict_storage['Storage'].replace(" ", "_")
        region = dict_storage['Site'].replace(" ", "_")

        #print ('**** OM Rate:', dict_storage['d_oam_rate'])

        return [['#code',storage+'_'+region,
                 '#name',storage+'_'+region,
                 '#input',storage+'_'+region+'_energy',
                 '#output',storage+'_'+region+'_energy',
                 ],
                ['efficiency_new', '#type', 'DVP_linear', '#data', str(start_date), str(1-dict_storage['discharge'])],
                ['cost', '#type', 'DVP_linear', '#data', str(start_date), str(dict_storage['inv-cost-p']*1e3)],
                ['lifetime', '#type', 'DVP_linear', '#data', str(start_date), str(dict_storage['depreciation'])],
                #['OaM_rate', '#type', 'DVP_linear', '#data', str(start_date), str(dict_storage['fix-cost-p']/float(dict_storage['inv-cost-p']))],
                ['OaM_rate', '#type', 'DVP_linear', '#data', str(start_date), str(dict_storage['d_oam_rate'] )],
                ['#endblock']]

    with open(path+'/Storage.csv', 'w') as writeFile:
        writer = csv.writer(writeFile, delimiter=";", lineterminator='\n')
        writer.writerows([['#comment', '===============storage-technologies============================================']])
        if b_use_storage:
            writer.writerows([['#blockwise']])
            for index, row in df_m_input.iterrows():
                # print(row.to_dict())
                writer.writerows(f_get_template_storage(row.to_dict(), index))
        else:
            writer.writerows([['#empty']])


    writeFile.close()
