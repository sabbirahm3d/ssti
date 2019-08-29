#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

if __name__ == "__main__":

    with open(sys.argv[-1]) as mem_dump_file:

        data = mem_dump_file.read().split("\n")[:-1]

        try:
            data_str = ''.join([chr(int(x, 2)) for x in data])
        except ValueError:
            data_str = ''.join([chr(int(x)) for x in data])

        if data_str == "HELLO WORLD":
            print("\x1b[1;32mOutputs match\x1b[0m")
        else:
            raise AssertionError("\x1b[1;31mOutputs don't match\x1b[0m")
