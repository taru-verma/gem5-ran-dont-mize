import matplotlib.pyplot as plt
import seaborn as sns
import statistics

send0_data = []
send1_data = []
with open('256-256-send0.log') as send0_inp:
    for line in send0_inp:
        send0_data.append(int(line))
with open('256-256-send1.log') as send1_inp:
    for line in send1_inp:
        send1_data.append(int(line))
print(set(send0_data))
print(set(send1_data))

#send0_mean, send0_pvariance, send0_stdev = statistics.mean(send0_data), statistics.pvariance(send0_data), statistics.pstdev(send0_data)
#send1_mean, send1_pvariance, send1_stdev = statistics.mean(send1_data), statistics.pvariance(send1_data), statistics.pstdev(send1_data)
#print(send0_mean, send0_pvariance, send0_stdev, send1_mean, send1_pvariance, send1_stdev)

######################################################################################

bins_list = list(set(send0_data))+list(set(send1_data))

sns.set_style('darkgrid')
sns.histplot([send1_data, send0_data], discrete=True, stat='probability', bins=bins_list, kde=True, kde_kws=dict(bw_adjust=4), legend=True, shrink=0.8)

plt.xticks(bins_list)
plt.xlabel('Number of CPU cycles')
plt.ylabel('Probability')
plt.legend(title='Legend', labels=['Sending 0', 'Sending 1'])
plt.title('PDFs for communicating (n=256, m=256)')
######################################################################################

#plt.show()
plt.savefig('exp0-pdf-256-256.png', format='png', dpi=1200)
