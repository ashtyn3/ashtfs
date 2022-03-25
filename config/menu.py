from rich.prompt import Prompt
from rich.console import Console
from time import sleep
from rich.pretty import Pretty
import sys
import os
from os.path import exists
import mmap

def clear_last_line():
    sys.stdout.write("\033[F")
    sys.stdout.write("\033[K")
choices =	{
  "1": {
      "title": "FS_SIZE",
      "type": "int",
      "args":{
          "prompt": "Size"
          },
      "set": False,
      "rec": True
      },
  "2": {
      "title": "IMG_PATH",
      "type": "str",
      "args":{
            "prompt": "Image"
          },
      "set": False,
      "rec": True
      },
}

answers = ["\n"]

console = Console()

def findPath(targetFile):
    exist = exists(targetFile)
    if not exist:
        os.chdir("..")
        findPath(targetFile)
    return os.path.abspath(".")

def remove_dup():
    res = []
    names = []
    lines = []
    for i,item in enumerate(answers):
        line = str.split(" ")
        if len(line) >= 2:
            if line[1] not in names:
                names.append(line[1])
                res.append(i)
    for index in res:
        lines.append(res[index])

    return lines

def exit_save():
    clear_last_line()
    base = findPath("WORKSPACE")
    answers = remove_dup()
    f = open(base+"/fs/config.h", "a")
    f.write("\n".join(answers))
    f.close()
    with open(base+"/fs/config.h") as f2:
        s = mmap.mmap(f2.fileno(), 0, access=mmap.ACCESS_READ)
        for key in choices:
            if key == "0":
                continue
            if s.find(bytes(choices[key]["title"], "UTF-8")) == -1 and choices[key]["rec"] == True:
                console.print(" [red bold]ERROR:[reset] "+choices[key]["title"]+" is possibly not set")
                return
    quit()
with console.screen():
    console.print("[bold]Ashtfs configuration tool", justify="center")
    console.print("Choose value to set:")
    console.print("(0): save & exit")
    console.print("(1): Filesystem size in KB")
    console.print("(2): Image path")
    while True:
        num = Prompt.ask(choices=["0",*choices.keys()])
        if num == "0":
            exit_save()
            continue
        clear_last_line()
        choice = choices[num]
        value = Prompt.ask(**choice["args"])
        clear_last_line()
        console.print("[green bold] "+choice["title"]+"[reset] = " + value)
        if choice["type"] == "int":
            answers.append("#define "+choice["title"]+" "+value)
        if choice["type"] == "str":
            answers.append("#define "+choice["title"]+" "+"\""+value+"\"")
