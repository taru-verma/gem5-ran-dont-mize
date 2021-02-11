# Generate a bunch of random addresses to see how many sets are primed
# Useful for offline analysis

import sys
from random import seed
from random import random

def generate_addresses(num_elements, base_addr, offset):
    array = []
    for i in range(num_elements):
        set_bits = int(random()*10e12)
        tag_bits = int(random()*10e12)
        array.append(base_addr + offset + ((tag_bits % 8) << 12) + ((set_bits % 64) << 6));
    return array

def main():
    seed(42)
    
    base_addr = 0x87000
    
    n = int(sys.argv[1])
    m = int(sys.argv[2])
    
    print ("Number of primed lines for receiver: ", n)
    print ("Number of primed lines for sender: ", m)

    addr_ptr_recv = generate_addresses(n, base_addr, 0)
    addr_ptr_send = generate_addresses(m, base_addr, 0x8000 + 0x1000)

    print('[{}]'.format(', '.join(hex(elem) for elem in addr_ptr_recv)));
    print('[{}]'.format(', '.join(hex(elem) for elem in addr_ptr_send)));


if __name__ == "__main__":
    main()
