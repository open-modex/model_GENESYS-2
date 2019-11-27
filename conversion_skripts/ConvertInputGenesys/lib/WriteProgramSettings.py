import csv

def f_write (scn, path, start_date, end_date):


    with open(path + '/ProgramSettings.dat', 'w') as writeFile:
        writer = csv.writer(writeFile, delimiter="=", lineterminator='\n')

        writer.writerows([['/*general settings*/']])
        writer.writerows([['optimisation_algorithm' ,'cma-es']])
        writer.writerows([['simulation_start', str(start_date)]])
        writer.writerows([['simulation_end', str(end_date)]])

        writer.writerows([['interest_rate', str(0.07)]])
        writer.writerows([['use_randomisation', 'yes']])


        writer.writerows([['/*variables for operation simulation*/']])
        writer.writerows([['gridbalance_hop_level', str(0)]])
        writer.writerows([['operation_algorithm', 'hsm_total_cost_min']])
        writer.writerows([['energy2power_ratio', '1h']])
        writer.writerows([['operation_sequence_duration', '100a']])
        writer.writerows([['simulation_step_length', '1h']])
        writer.writerows([['analysis_hsm_output_detail', 'yes']])


    writeFile.close()