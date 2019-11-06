#£!/bin/bash

# PATH = ""
arm-none-eabi-gdb "build/project.elf" -ex 'target extended-remote localhost:3333'