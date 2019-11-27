import csv
import pandas as pd


def f_import(scn, path, dfs, start_date='2030-01-01_00:00'):
    # try:
    #     # read excel file once!
    #     xlsx = pd.ExcelFile('./input/' + scn + '.xlsx')
    #     # print(xlsx)
    #     # print(xlsx.sheet_names)
    #     dfs = {sheet_name: xlsx.parse(sheet_name) for sheet_name in xlsx.sheet_names}
    # except:
    #     print("ERROR could not read input excel file for scenario ", scn)
    #     sys.exit()

    # define input-sheet dataframes from global df
    df_m_input_location = dfs['Process']
    df_m_input_process = dfs['Process-Commodity']

    def f_get_template_multiconverter(dict_multiconverter_location, dict_multiconverter_process):
        #if 'efficiency_new' not in dict_multiconverter_location:
        #dict_multiconverter_location['efficiency_new'] = 1

        str_input = list()
        str_input.append('#input')
        str_output = list()
        str_output.append('#output')
        str_conversion = list()
        str_conversion.append('#conversion')
        for index in dict_multiconverter_process['Direction']:

            if dict_multiconverter_process['Process'][index] == dict_multiconverter_location['Process']:
                if dict_multiconverter_process['Direction'][index] == 'In':
                    if dict_multiconverter_process['Commodity'][index] == 'Elec':
                        str_input.append('electric_energy')
                    else:
                        str_input.append(dict_multiconverter_process['Commodity'][index]+'_'+dict_multiconverter_location['Site'])
                if dict_multiconverter_process['Direction'][index] == 'Out':
                    if dict_multiconverter_process['Commodity'][index] == 'Elec':
                        str_output.append('electric_energy')
                        str_conversion.append(str(-1))
                        dict_multiconverter_location['efficiency_new'] = dict_multiconverter_process['ratio'][index]

                    else:
                        str_output.append(dict_multiconverter_process['Commodity'][index]+'_'+dict_multiconverter_location['Site'])
                        str_conversion.append(str(dict_multiconverter_process['ratio'][index]*1000))

        list_start = ['#code',dict_multiconverter_location['Process'].replace(" ", "_")+'_'+dict_multiconverter_location['Site'].replace(" ", "_"),'#name',dict_multiconverter_location['Process'].replace(" ", "_")+'_'+dict_multiconverter_location['Site'].replace(" ", "_")]
        list_start = list_start.__add__(str_input)
        # list_start = list_start.__add__(str_output)
        # list_start = list_start.__add__(str_conversion)
        return [list_start,
                str_output,
                str_conversion,
                ['efficiency_new', '#type', 'DVP_linear', '#data', str(start_date), str(dict_multiconverter_location['efficiency_new'])],
                ['cost', '#type', 'DVP_linear', '#data', str(start_date), str(dict_multiconverter_location['inv-cost']*1000)],
                ['lifetime', '#type', 'DVP_linear', '#data', str(start_date), str(dict_multiconverter_location['depreciation'])],
                ['OaM_rate', '#type', 'DVP_linear', '#data', str(start_date), str(dict_multiconverter_location['fix-cost']/float(dict_multiconverter_location['inv-cost']))],
                ['#endblock']]
        #END-OF f_get_template_multiconverter

    try:
        with open(path+'/MultiConverter.csv', 'w') as writeFile:

            writer = csv.writer(writeFile, delimiter=";", lineterminator='\n')
            writer.writerows([['#comment',
                               '===============multiconverter-technologies============================================']])
            writer.writerows([['#blockwise']])

            for index, row in df_m_input_location.iterrows():
                # print(row.to_dict())
                dict_multiconverter_location = row.to_dict()
                if dict_multiconverter_location['Process'] != 'Curtailment' and dict_multiconverter_location[
                    'Process'] != 'Photovoltaics' and dict_multiconverter_location['Process'] != 'Wind':
                    writer.writerows(
                        f_get_template_multiconverter(dict_multiconverter_location, df_m_input_process.to_dict()))

        writeFile.close()
    except:
        print("ERROR could write to file", path, scn, "MultiConverter.csv")

