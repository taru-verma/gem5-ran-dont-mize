import random
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

system_random = random.SystemRandom()
num_trials = 50

cache_size = 512*1024
associativity = 16
num_sets = int(cache_size/(associativity * 64))
num_lines = int(num_sets * associativity)

acc, bins, sets, results = 0, [], [], {}
bins = [i for i in range(0, num_sets+1)]
for i in range(num_lines):
    results[i] = 0.0
 
for i in range(num_trials):
    sets = [system_random.randint(0, num_sets - 1) for k in range(num_lines)]
    for j in range(0, num_lines, 100):
        acc = 0
        curr_sets = system_random.sample(sets, j)
        hist, edges = np.histogram(curr_sets, bins)

        for k in range(len(hist)):
            if hist[k] >= associativity:
                acc += 1
        value = results[j] + acc/(num_sets*num_trials)
        results.update({j:value})

plt.plot(*zip(*sorted(results.items())))
plt.xlabel('Number of random lines written (n)')
plt.ylabel('Percentage of cache sets primed (p)')
plt.title('p(n)')
#plt.show()
plt.savefig('priming.png', format='png', dpi=1200)


