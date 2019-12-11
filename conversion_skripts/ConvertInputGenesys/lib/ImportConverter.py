import csv

def f_import(dfs, scn, path, start_date='2030-01-01_00:00'):
    df_m_input_converter_location = dfs['Process']
    df_m_input_converter_process = dfs['Process-Commodity']
    b_use_storage = True
    try:
        df_m_input_storage = dfs['Storage']
    except:
        print("\tWARNING: no Storage found in scenario", scn)
        b_use_storage = False

    def f_get_template_converter(dict_converter_location, dict_converter_process):

        if 'efficiency_new' not in dict_converter_location:
            dict_converter_location['efficiency_new'] = 1

        str_input = list()
        str_input.append('#input')
        str_output = list()
        str_output.append('#output')
        for index in dict_converter_process['Direction']:

            if dict_converter_process['Commodity'][index] == 'Elec':
                dict_converter_process['Commodity'][index] = 'electric_energy'

            if dict_converter_process['Process'][index] == dict_converter_location['Process']:
                if dict_converter_process['Direction'][index] == 'In':
                    str_input.append(dict_converter_process['Commodity'][index]+'_'+dict_converter_location['Site'].replace(" ", "_"))
                if dict_converter_process['Direction'][index] == 'Out':
                    str_output.append(dict_converter_process['Commodity'][index])

        list_start = ['#code',dict_converter_location['Process'].replace(" ", "_")+'_'+dict_converter_location['Site'].replace(" ", "_"),
                      '#name',dict_converter_location['Process'].replace(" ", "_")+'_'+dict_converter_location['Site'].replace(" ", "_")]
        list_start = list_start.__add__(str_input)
        list_start = list_start.__add__(str_output)
        list_start = list_start.__add__(['#bidirectional', 'false'])

        #calculate the operation and maintenance cost
        try:
            dict_converter_location['d_oam_rate'] = dict_converter_location['fix-cost'] / dict_converter_location['inv-cost']
        except ZeroDivisionError:
            dict_converter_location['d_oam_rate'] = 0.01*dict_converter_location['inv-cost']

        return [list_start,
                ['efficiency_new', '#type', 'DVP_linear', '#data', str(start_date), str(dict_converter_location['efficiency_new'])],
                ['cost', '#type', 'DVP_linear', '#data', str(start_date), str(dict_converter_location['inv-cost']*1e3)],
                ['lifetime', '#type', 'DVP_linear', '#data', str(start_date), str(dict_converter_location['depreciation'])],
               # ['OaM_rate', '#type', 'DVP_linear', '#data', str(start_date), str(dict_converter_location['fix-cost']/float(dict_converter_location['inv-cost']))],
                ['OaM_rate', '#type', 'DVP_linear', '#data', str(start_date),str(dict_converter_location['d_oam_rate'])],
                ['#endblock']]

    def f_get_template_storage(dict_storage, index, site):
        if 'depreciation' not in dict_storage:
            dict_storage['d_lifetime'] = 20

        try:
            dict_storage['oam-rate'] = dict_storage['fix-cost-p'] / dict_storage['inv-cost-p']
        except ZeroDivisionError:
            dict_storage['oam-rate'] = 0.133* dict_storage['inv-cost-p']

        storage = dict_storage['Storage'].replace(" ", "_")
        region = dict_storage['Site'].replace(" ", "_")

        return [['#code','converter'+'_'+storage+'_'+region,
                 '#name','converter'+'_'+storage+'_'+region,
                 '#input','electric_energy',
                 '#output', storage+'_'+region+'_energy',
                 '#bidirectional', 'true'],
                ['efficiency_new', '#type', 'DVP_linear', '#data', str(start_date), str(dict_storage['eff-in'])],
                ['cost', '#type', 'DVP_linear', '#data', str(start_date), str(dict_storage['inv-cost-p']*1e3)],
                ['lifetime', '#type', 'DVP_linear', '#data', str(start_date), str(dict_storage['depreciation'])],
                ['OaM_rate', '#type', 'DVP_linear', '#data', str(start_date), str(dict_storage['oam-rate'])],
                ['#endblock']]

    with open(path+'/Converter.csv', 'w') as writeFile:
        writer = csv.writer(writeFile, delimiter=";", lineterminator='\n')
        writer.writerows([['#comment', '===============converter-technologies============================================']])
        writer.writerows([['#blockwise']])

        for index, row in df_m_input_converter_location.iterrows():
            dict_converter_location = row.to_dict()

            if dict_converter_location['Process'] != 'Curtailment' and dict_converter_location['Process'] in ['Wind', 'Photovoltaics', 'Run of River', 'Wind power plant', 'Solar power plant', 'Hydro power plant']:
                writer.writerows(f_get_template_converter(dict_converter_location, df_m_input_converter_process.to_dict()))
        if (b_use_storage):
            for index, row in df_m_input_storage.iterrows():
                # print(row.to_dict())
                writer.writerows(f_get_template_storage(row.to_dict(), index, site=dict_converter_location['Site']))

    writeFile.close()
