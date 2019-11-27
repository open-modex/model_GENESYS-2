import csv


def f_write (path, start_date):

    with open(path + '/TimeSeries/PrimaryEnergyUnlimited_minusOne.csv', 'w') as writeFile:
        writer = csv.writer(writeFile, delimiter=";", lineterminator='\n')
        writer.writerows([['#comment','unlimited -1 primary ressource']])
        writer.writerows([['base','#type','DVP_const','#data',start_date,'1e15']])
        writer.writerows([['value','#type','DVP_linear','#data',start_date,'-1']])
        writer.writerows([['#endtable']])

    writeFile.close()