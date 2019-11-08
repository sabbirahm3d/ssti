#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

import os
import {ipc}
import sys

{module_dir}
import {module}

# Connect the PyRTL simulation to SST through {ipc}
{driver_bind}
_sock.{connect}(sys.argv[1])

_sock.{send}(str(os.getpid()).encode())

while True:
    signal = str(_sock.recv({buf_size}).decode("utf-8"))
    alive = int(signal[0])
    signal = signal[1:]
    if not alive:
        break
    {module}.sim.step({{
        {inputs}
    }})
    _outputs = {outputs}
    _sock.{send}(_outputs)
