from csv import list_dialects
import sys
import os
from typing import List
from os import listdir
from os.path import isfile, join

COMMAND = "otool -L "
SUFF_LIB = "Contents/Resources/lib/"
SUFF = "Contents/MacOS/writernote"
COPY = "cp "

COMMAND_TOOL = "install_name_tool "
COMMAND_SUFF = " -change "
COMMAND_ID = " -id "
COMMAND_BEFORE = " @executable_path/../Resources/lib"
COMMAND_ID_SUFF = " @executable_path/../Resources"

def file_in_folder(path: str) -> list[str]:
    return [f for f in listdir(path) if isfile(join(path, f))]


def remove_double(list_dep: list[str]) -> list[str]:
    newlist = []
    for i in list_dep:
        if i not in newlist:
            newlist.append(i)

    return newlist

def get_dep(pos_binary: str, dest_list: str, binary: bool) -> list[str]:
    os.system("rm {}".format(dest_list))
    os.system(COMMAND + pos_binary + " >> " + dest_list)
    list = []

    with open(dest_list, "r") as file:
        lines = file.readlines()
        for line in lines:
            list.append(line)

    if binary:
        list = list[1:]
    else:
        list = list[2:]

    list_sec = []
    i = 0
    for line in list:
        list_sec.append(line)

        try:
            ind = list_sec[i].index('(')
            list_sec[i] = list_sec[i][:ind]
        except ValueError:
            pass

        list_sec[i].replace(" ", "")
        list_sec[i].replace("\n", "")
        list_sec[i].replace("\t", "")

        if not("/usr/local" in list_sec[i] or "poppler" in list_sec[i]):
            print("INFO: Delete dependency: {}".format(list_sec[i]))
            del list_sec[i]
            i -= 1

        i += 1

    list = list_sec

    return remove_double(list_sec)

def get_name_lib(lib: str) -> str:
    list = lib.split('/')
    name = list[-1]
    print("get_name_lib: position lib: {} name: {}".format(lib, name))
    return name

# copy all the dependency to SUFF_LIB
def copy_dep(app_path: str, list_dep: list[str]) -> bool:
    print("copy_dep: app_path: {}".format(app_path))

    try:
        tmp = "{}/{}".format(app_path, SUFF_LIB)
        os.mkdir(tmp)
    except:
        print("Directory already exist")

    for dep in list_dep:
        name = get_name_lib(dep)
        to = app_path + "/" + SUFF_LIB + name
        if os.system("{} {} {}".format(COPY, dep, to )) != 0:
            print("copy_dep: error copy from: {} to: {}".format(dep, to))
            return False

    return True


"""
example:
path = /home/user/writernote-qt/hello.txt

return /home/user/writernote-qt/
"""
def get_pos(path: str) -> str:
    list = path.split('/')
    
    string_ret = ""
    for a in list[:-1]:
        string_ret += '/' + a

    if string_ret[-1] != '/':
        string_ret += "/"
    
    return string_ret


def print_dep(list: list[str]) -> None:
    for line in list:
        print(line)

def change_dep(pos_bin: str, list_dep: list[str]) -> bool:
    pos_lib = pos_bin + "/" + SUFF_LIB
    list_file = file_in_folder(pos_lib)
    
    print("List of all dep in lib: ")
    print_dep(list_file)
    
    for real_dep in list_dep:
        for dep_exe in list_file:
            command = "{}{} {}{}/{} {}{}".format(COMMAND_TOOL, COMMAND_SUFF, real_dep, COMMAND_BEFORE, get_name_lib(real_dep), pos_lib, dep_exe)
            print("First command: ", command)
            os.system(command)
    
    return change_id(pos_bin)

def change_id(pos_bin: str) -> bool:
    pos_lib = pos_bin + "/" + SUFF_LIB
    list_file = file_in_folder(pos_lib)

    for dep_exe in list_file:
        command = "{}{} {}/ {}{}".format(COMMAND_TOOL, COMMAND_ID, COMMAND_ID_SUFF,pos_lib, dep_exe)
        print("Second command: ", command)

        os.system(command)

    return True

pos_bin = sys.argv[1]
ind = pos_bin.index("writernote.app")
pos_dest = pos_bin[:ind] + "lib_list.txt"


def main(list: list[str]) -> list[str]:
    #print("call to main")
    for dep in list:
        #print("dep: ", dep)
        list_new_dep = get_dep(dep, pos_dest, binary=False)

        list_new_dep = main(list_new_dep)
        #print("list_new_dep ", list_new_dep)
        for dep_sec in list_new_dep:
            if dep_sec not in list:
                #print("add ", dep_sec)
                list.append(dep_sec)

    return remove_double(list)

if __name__ == "__main__":
    
    pos_exe = pos_bin + "/" + SUFF

    # get dependency of writernote (binary)
    list = get_dep(pos_exe, pos_dest, binary=True)

    lista = main(list)

    print("List for all dep: ")
    print_dep(lista)

    if not copy_dep(pos_bin, list):
        print("Error copy")
        exit(1)

    if not change_dep(pos_bin, list):
        print("Error change dep")
        exit(2)

