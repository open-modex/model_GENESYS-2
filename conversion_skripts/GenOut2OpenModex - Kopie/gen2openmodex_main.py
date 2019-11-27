'''
Created on Sep 24, 2019

@author: jou
'''

#import all generation-files
import gen_constraints
import gen_investment
import gen_memory
import gen_objective
import gen_production
import gen_timebuild
import gen_variables

# Ergebnisfile von Genesys als String übergeben
read_file = 'AnalysedResult.xml'

def main():
    #generate the constraints-file
    gen_constraints.get_constraints()
    #generate the investment-file
    #gen_investment.get_investment(read_file)
    #generate the memory-file
    gen_memory.get_memory()
    #generate the objective-file
    gen_objective.get_objective(read_file)
    #generate the production-file
    gen_production.get_production(read_file)
    #generate the timebuild-file
    gen_timebuild.get_timebuild(read_file)
    #generate the variables-file
    gen_variables.get_variables(read_file)



if __name__ == '__main__':
    main()