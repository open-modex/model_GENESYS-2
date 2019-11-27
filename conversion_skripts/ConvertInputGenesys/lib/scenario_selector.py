import os

def generate_scenario_list(str_input_folder):
    l_input = os.listdir(str_input_folder)
    l_scenarios = []

    for entry in l_input:
        #find excel files and ommit office file-locks (.~***filename***)
        if(entry.find("xlsx")>1 and entry.find('.~') < 0):
                #print(entry)
                l_scenarios.append(entry.strip('.xlsx'))
    return l_scenarios


