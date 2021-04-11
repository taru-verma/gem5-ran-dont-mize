import random
import numpy as np

system_random = random.SystemRandom()
num_trials = 20

cache_size = 512*1024
associativity = [4, 8, 12, 16, 20, 32]

acc, bins, sets, results = 0, [], [], {}
 
f = open("data.log", "w")

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

    f.write(str(sorted(results.items())))
    f.write('\n')

f.close()