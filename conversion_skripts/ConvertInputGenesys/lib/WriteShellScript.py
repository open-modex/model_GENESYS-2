import csv, os

def f_write (path):


    with open(path + '/start_genesys.sh', 'w') as writeFile:
        writer = csv.writer(writeFile, delimiter=" ", lineterminator='\n')

        writer.writerows([['echo', 'Run genesys2 script v2']])

        writer.writerows([['#getting the scenario name from the current folder']])
        writer.writerows([['MY_DIR=','${PWD##*/}']])
        writer.writerows([['echo', "Scenario:", '$MY_DIR']])
        writer.writerows([['DATE="$(date +%Y-%m-%d_%H-%M)"']])
        writer.writerows([['#current path']])
        writer.writerows([['CPATH="$(pwd | xargs basename)"']])
        writer.writerows([['#generate filename from time and path']])
        writer.writerows([['''log_filename=$DATE"_genesys_log_"$CPATH".txt"''']])
        writer.writerows([['''echo "Starting Genesys 2 with logging to "$log_filename''']])
        writer.writerows([['''#execute the optimisation as bg job''']])
        writer.writerows([['''./genesys_2 --mode=analysis --scenario="$MY_DIR" -j=1>$log_filename&''']])

    writeFile.close()

    os.chmod(path + '/start_genesys.sh', 0o777)