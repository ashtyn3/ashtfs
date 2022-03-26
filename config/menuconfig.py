from menu import findPath, choices, main
from os.path import exists
import os

os.chdir(os.environ['BUILD_WORKING_DIRECTORY'])
base = findPath("fs")

from pyparsing import *

# define the structure of a macro definition (the empty term is used 
# to advance to the next non-whitespace character)
macroDef = "#define" + Word(alphas+"_",alphanums+"_").setResultsName("macro") + \
            empty + restOfLine.setResultsName("value")


if exists(base+"/fs/config.h"):
    with open(base+"/fs/config.h", 'r') as f:
        res = macroDef.scanString(f.read())
        for macro in res:
            for item in choices:
                define = macro[0]
                if choices[item]["title"] == define[1]:
                    choices[item]["value"] = define[2]
                    choices[item]["args"]["default"] = define[2]

main()
