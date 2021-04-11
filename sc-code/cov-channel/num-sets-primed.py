import random
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

system_random = random.SystemRandom()
num_trials = 20

cache_size = 512*1024
associativity = [4, 8, 12, 16, 20, 32]

acc, bins, sets, results = 0, [], [], {}
 
for assoc in associativity:
    num_sets = int(cache_size/(assoc * 64))
    num_lines = int(num_sets * assoc)
    bins = [i for i in range(0, num_sets+1)]
    for i in range(num_lines):
        results[i] = 0.0

    for trial in range(num_trials):
        sets = [system_random.randint(0, num_sets - 1) for i in range(num_lines)]
        for lines in range(0, num_lines):
            acc = 0
            curr_sets = system_random.sample(sets, lines)
            hist, edges = np.histogram(curr_sets, bins)

            for this_set in range(len(hist)):
                if hist[this_set] >= assoc:
                    acc += 1
            value = results[lines] + acc/(num_sets*num_trials)
            results.update({lines:value})

    plt.plot(*zip(*sorted(results.items())))

plt.xlabel('Number of random lines written (n)')
plt.ylabel('Percentage of cache sets primed (p)')
plt.title('p(n), 512kB randomized LLC, 8192 lines')
plt.legend([str(assoc) for assoc in associativity], title='Associativity of\nrandomized cache')
#plt.show()
plt.savefig('priming.png', format='png', dpi=1200)
