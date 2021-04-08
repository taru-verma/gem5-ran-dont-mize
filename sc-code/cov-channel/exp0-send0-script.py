"""
    Script to run exp0 
    Move to gem5 root dir for executing - fix this format when future experiments are known
    Basic repetition encoding - receiver and sender communicate through stochastic priming
    Script sweeps parameters n and m and captures average access times
"""

import os
import subprocess
import math
import re

current_dir = os.path.dirname(os.path.realpath(__file__))
gem5_binary = os.path.join(current_dir, "build/X86/gem5.opt")
config_script = os.path.join(current_dir, "sc-configs/sysconfig.py")
cov_chan_code = os.path.join(current_dir, "sc-binaries/g5-cc-rep-send0")

num_trials = 500

# stdout log file
out_file = open('out0.log', 'w')
out_file.write('### exp0 gem5 and covert channel outputs\n### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n')
out_file.flush()

# stderr log file
err_file = open('err0.log', 'w')
err_file.write('### exp0 gem5 errors and warnings\n### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n')
err_file.flush()

# results log file
res_file = open('res0.log', 'w')
res_file.write('### exp0 average access times\n### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n')
res_file.flush()

# n = 64, 128, 256, 512
# m = 8, 16, 32, ..., (till m == n)
#for (n, m) in [(128, 32), (256, 32), (512, 8)]:
for n in [64*pow(2, i) for i in range(4)]:
    for m in [pow(2, j) for j in range(3, int(math.log(n,2))+1, 1)]:
        # Prepare command to execute
        cmd = [gem5_binary, config_script, cov_chan_code, str(num_trials), str(n), str(m)]
        cmd_str = "Executing cmd: " + " ".join(cmd) + '\n'
        print(cmd_str)

        # Execute command and capture stdout and stderr
        # stdout should contain gem5 run details and covert channel output
        # stderr should contain x86 cpuid warnings
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = proc.communicate()
        out_str = out.decode()
        err_str = err.decode()

        # Log stdout and stderr
        out_file.write(cmd_str)
        out_file.write(out_str)
        out_file.write('~~~~~~~~~~~~~~~~~~~~\n')
        out_file.flush()
        err_file.write(cmd_str)
        err_file.write(err_str)
        err_file.write('~~~~~~~~~~~~~~~~~~~~\n')
        err_file.flush()

        # Log results
        res_str = re.search('res=(.+?)\n', out_str)
        if res_str:
            res_file.write(res_str.group(1) + '\n')
        else:
            res_file.write('Error: results not available for ' + cmd_str + '\n')
        res_file.flush()
print("Done\n")
