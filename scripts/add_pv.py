# This is a script that allows for rapid adding of PVs to the plugin, which
# avoids having to edit 3 different files in a specific manner.
#
# Author: Jakub Wlodek
# Created on: 27-Mar-2019
#

import os
import argparse


# Arrays containing valid PV and data types
datatypes = ["Int32", "Float64", "Octet"]
pvtypes = [['binary', 'bo', 'bi'], ['multibit', 'mbbo', 'mbbi'], ['analog', 'ao', 'ai'], ['string', 'stringout', 'stringin'], ['waveform', 'waveform', 'waveform']]


# Path names to necessary files. These depend on the plugin and are fixed when the update_names.py script is run
path_to_template = "../timelapseApp/Db/NDPluginTimelapse.template"
path_to_header = "../timelapseApp/src/NDPluginTimelapse.h"
path_to_source = "../timelapseApp/src/NDPluginTimelapse.cpp"
name_of_driver = "NDPluginTimelapse"

# Main function that writes necessary PV info to header and source files. Note that formatting may not be correct.
def write_init_pv(pv_base_name, pv_string, driver_name, first_pv, dtype):
    os.rename(path_to_header, path_to_header+"_OLD")
    os.rename(path_to_source, path_to_source+"_OLD")
    header_file_old = open(path_to_header+"_OLD", "r+")
    source_file_old = open(path_to_source+"_OLD", "r+")
    header_file = open(path_to_header, "w+")
    source_file = open(path_to_source, "w+")

    pvIndexWritten = False

    line = header_file_old.readline()
    while line:
        if "MODIFICATION" in line:
            header_file.write(line)
            header_file.write("\n\n")
            header_file.write("#define "+driver_name+pv_base_name+"String "+'"'+pv_string+'"'+" //asynParam"+dtype+"\n")
        elif "FIRST_PARAM" in line:
            if first_pv == True and not pvIndexWritten:
                header_file.write("int "+driver_name+pv_base_name+";\n")
                line = line.strip()
                line = line.split(' ')
                header_file.write(line[0]+" "+line[1]+ " "+driver_name+pv_base_name+"\n")
                pvIndexWritten = True
            else:
                header_file.write(line)
        elif "LAST_PARAM" in line and first_pv == False and not pvIndexWritten:
            header_file.write("int " + driver_name+pv_base_name+";\n")
            line = line.strip()
            line = line.split(' ')
            header_file.write(line[0]+" "+line[1]+ " "+driver_name+pv_base_name+"\n")
            pvIndexWritten = True
        else:
            header_file.write(line)
        line = header_file_old.readline()
    
    isConstructor = False
    line_src = source_file_old.readline()
    while line_src:
        if driver_name+"::"+driver_name in line_src:
            isConstructor = True
            source_file.write(line_src)
        elif "{" in line_src and isConstructor:
            source_file.write(line_src)
            source_file.write("createParam("+driver_name+pv_base_name+"String, asynParam"+dtype+", &"+driver_name+pv_base_name+");\n")
            isConstructor = False
        else:
            source_file.write(line_src)
        line_src = source_file_old.readline()
    header_file_old.close()
    source_file_old.close()
    os.remove(path_to_header+"_OLD")
    os.remove(path_to_source+"_OLD")
    header_file.close()
    source_file.close()



# Parses input PV string into PV names
def parse_pv_string(pv_string):
    parts = pv_string.split('_')
    pv_base_name = ""
    for i in range(0, len(parts)):
        temp = parts[i].lower().capitalize()
        pv_base_name += temp
    pv_readback_name = pv_base_name+"_RBV"
    return pv_base_name, pv_readback_name



# Writes a waveform pv to template file (waveforms have somewhat different fields)
def write_pv_waveform(pv_string):
    pv_base_name, pv_readback_name = parse_pv_string(pv_string)
    template_file = open(path_to_template, "a+")
    template_file.write("\n\n")
    template_file.write('record(waveform, "$(P)$(R)'+pv_base_name+'"){\n')
    template_file.write('   field(DTYP, "asynOctetWrite")\n')
    template_file.write('   field(OUT, "@asyn($(PORT),$(ADDR), $(TIMEOUT))'+pv_string+'")\n')
    template_file.write('   field(FTVL, "CHAR")\n')
    template_file.write('   field(NELM, "256")\n')
    template_file.write('   info(autosaveFields, "VAL")\n')
    template_file.write('}\n\n')
    template_file.write('record(waveform, "$(P)$(R)'+pv_readback_name+'"){\n')
    template_file.write('   field(DTYP, "asynOctetRead")\n')
    template_file.write('   field(INP, "@asyn($(PORT),$(ADDR), $(TIMEOUT))'+pv_string+'")\n')
    template_file.write('   field(FTVL, "CHAR")\n')
    template_file.write('   field(NELM, "256")\n')
    template_file.write('   field(SCAN, "I/O Intr")\n')
    template_file.write('}\n')



# writes a basic pv to the template file (note any PV specific info needs to be added after)
def write_pv_basic(pv_string, pv_type, dtype):
    pv_base_name, pv_readback_name = parse_pv_string(pv_string)
    template_file = open(path_to_template, "a+")
    template_file.write("\n\n")
    template_file.write('record('+pv_type[1]+', "$(P)$(R)'+pv_base_name+'"){\n')
    template_file.write('    field(DTYP, "asyn'+dtype+'")\n')
    template_file.write('    field(OUT, "@asyn($(PORT),$(ADDR), $(TIMEOUT))'+pv_string+'")\n')
    template_file.write('    field(VAL, "0")\n')
    template_file.write('    info(autosaveFields, "VAL")\n')
    template_file.write('}\n\n')
    template_file.write('record('+pv_type[2]+', "$(P)$(R)'+pv_readback_name+'"){\n')
    template_file.write('    field(DTYP, "asyn'+dtype+'")\n')
    template_file.write('    field(INP, "@asyn($(PORT),$(ADDR), $(TIMEOUT))'+pv_string+'")\n')
    template_file.write('    field(VAL, "0")\n')
    template_file.write('    field(SCAN, "I/O Intr")\n')
    template_file.write('}\n')



# checks if data format is valid
def check_valid_dform(data_format):
    for dform in datatypes:
        if data_format==dform:
            return True
    return False



# checks if pv type is valid
def check_valid_type(pv_type):
    for ptype in pvtypes:
        if ptype[0] == pv_type:
            return True
    return False



# gets the type of the PV
def get_type(pv_type):
    for ptype in pvtypes:
        if ptype[0] == pv_type:
            return ptype



# Parses user command line input
def parse_args():
    parser = argparse.ArgumentParser(description = "PV boilerplate code generator")
    parser.add_argument('-n', '--name', help='PV String name to be used. Should be all caps with underscores for spaces. ex: EXPOSURE_TIME')
    parser.add_argument('-t', '--type', help='Record type for the PV (binary, multibit, analog, string, waveform)')
    parser.add_argument('-d', '--data-format', help='Data type for the record (Int32, Float64, Octet)')
    parser.add_argument('-f', '--first', action='store_true', help='used to tag as the first pv')
    arguments = vars(parser.parse_args())
    if arguments["name"] is None:
        print("No name specified, pv not being added")
        return
    if arguments["type"] is None:
        print("No type specified, pv not being added")
        return
    if arguments["data_format"] is None:
        print("No data format specified, pv not being added")
        return
    if check_valid_type(arguments["type"]) == False or check_valid_dform(arguments["data_format"]) == False:
        print("Illegal value for data format or type")
        return
    pv_string = arguments["name"]
    pv_type = get_type(arguments["type"])
    data_format = arguments["data_format"]
    pv_base_name, pv_readback_name = parse_pv_string(pv_string)
    if(pv_type == "waveform"):
        write_pv_waveform(pv_string)
    else:
        write_pv_basic(pv_string, pv_type, data_format)

    write_init_pv(pv_base_name, pv_string, name_of_driver, arguments["first"], data_format)


parse_args()

    