from mpl_toolkits import mplot3d
import numpy as np
import matplotlib.pyplot as plt
import csv

n, m, send0_avat, send1_avat = [], [], [], []
i = 0

# Open results files, remove first two lines
with open('send0-res.log') as send0_results:
    send0_lines = send0_results.read().splitlines()
send0_lines.pop(0)
send0_lines.pop(0)

with open('send1-res.log') as send1_results:
    send1_lines = send1_results.read().splitlines()
send1_lines.pop(0)
send1_lines.pop(0)

# Extract CSVs 
csv_reader = csv.reader(send0_lines, delimiter=',')
send0_data = [row for row in csv_reader]
send0_data = np.array(send0_data).flatten()

csv_reader = csv.reader(send1_lines, delimiter=',')
send1_data = [row for row in csv_reader]
send1_data = np.array(send1_data).flatten()

# Create x, y, and z axes
while i < len(send0_data):
    n.append(int(send0_data[i]))
    m.append(int(send0_data[i+1]))
    send0_avat.append(float(send0_data[i+2]))
    send1_avat.append(float(send1_data[i+2]))
    i += 3

# Plot
fig = plt.figure(dpi=2000)
fig.suptitle('Stochastic Priming Covert Channel', fontsize=16)

ax = plt.gca(projection='3d')
#ax.set_title("")
ax.set_xlabel("$n$ (num lines primed by receiver)")
ax.set_ylabel("$m$ (num lines primed by sender)")
ax.set_zlabel("Average access time")

ax.plot3D(n, m, send0_avat, 'red', label='Sending 0')
ax.plot3D(n, m, send1_avat, 'blue', label='Sending 1')
ax.scatter3D(n, m, send0_avat, c=send0_avat, cmap='Reds');
ax.scatter3D(n, m, send1_avat, c=send1_avat, cmap='Blues');

ax.legend(loc='best')

#for x, y, z0, z1 in zip(n, m, send0_avat, send1_avat):
#    ax.text(x, y, z0, "{:.2f}".format(z0), fontsize='xx-small', multialignment='center', color='red')
#    ax.text(x, y, z1, "{:.2f}".format(z1), fontsize='xx-small', multialignment='center', color='blue')

plt.savefig("exp0_plot.png")

for j in range(len(send0_avat)):
    print(str(n[j]) + '\t' + str(m[j]) + '\t' + "{:.6f}".format(send0_avat[j]) + '\t' + "{:.6f}".format(send1_avat[j]))
