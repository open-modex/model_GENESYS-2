import csv

def f_write(path):
 
  with open(path+'/InstallationListResult.csv', 'w') as writeFile:
        writer = csv.writer(writeFile, delimiter=";", lineterminator='\n')
        writer.writerows([['#comment', 'MOUNTING-CODE.TECH-CODE;DATA-TYPE;DATA(may contain placeholders varxy);(if applicable) next lines:;#varxy;INIT-POINT;lBOUND;uBOUND;']])
        writer.writerows([['#empty']])

