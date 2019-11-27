import csv
import pandas
import os

def f_import(dfs, scn, path, start_date='2030-01-01_00:00'):
    #print(scn, path)
    if not os.path.exists(path+'/TimeSeries/'):
        os.mkdir(path+'/TimeSeries/')

    # try:
    #df_m_input = pandas.read_excel('./input/'+scn+'.xlsx', sheet_name='SupIm', index_col=0)
    df_m_input = dfs['SupIm']

    for col in df_m_input:
        with open(path+'/TimeSeries/' + col.replace(".", "_") + '.csv', 'w') as writeFile:
            writer = csv.writer(writeFile, delimiter=";", lineterminator='\n')

            writer.writerows([['#comment', '===============generation time series============================================']])
            writer.writerows([['base', '#type', 'DVP_const', '#data' , str(start_date), str(1000000)]])

            list_temp = ['value', '#type', 'TS_repeat_const', '#interval', '1h', '#start', str(start_date), '#data']
            writer.writerow(list_temp.__add__((df_m_input[col]).to_list()))
            writer.writerows([['#endtable']])

        writeFile.close()
        # except:
        #     print("No sheet for storages!")
