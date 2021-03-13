
import os
import subprocess
import math
import re

current_dir = os.path.dirname(os.path.realpath(__file__))
processes = []

commands = [['python', os.path.join(current_dir, 'exp0-send' + str(i) + '-script.py')] for i in range(2)]
processes = [subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE) for cmd in commands]

for proc in processes:
    proc.wait()