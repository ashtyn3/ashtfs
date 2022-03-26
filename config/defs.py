from menu import findPath, choices, main

base = findPath("WORKSPACE")

from pyparsing import *

# define the structure of a macro definition (the empty term is used 
# to advance to the next non-whitespace character)
macroDef = "#define" + Word(alphas+"_",alphanums+"_").setResultsName("macro") + \
            empty + restOfLine.setResultsName("value")
with open(base+"/fs/config.h", 'r') as f:
    res = macroDef.scanString(f.read())
    for macro in res:
        for item in choices:
            define = macro[0]
            if choices[item]["title"] == define[1]:
                choices[item]["value"] = define[2]
                choices[item]["args"]["default"] = define[2]

main()
