#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

struct Node
{
    double diff_access_time;
    double carrier_access_time;
    double signal_access_time;
    struct Node *next;
};
typedef struct Node nodes;

// Reading cache line - mfence, then read
static inline void read_cache_line(void *p) {
    asm volatile (
        "mfence\n\t"
        "movq (%0), %%r10\n\t"
        :: "r" (p)
        : "r10"
    );
}

// Reading and timing cache line - cpuid -> rdtsc -> read -> rdtsc -> cpuid
static inline uint32_t read_and_time_cache_line(void *line) {
    uint32_t tsc_low_bits = 0;

    asm volatile (
        "rdtsc\n\t"
        "mov %%eax, %%r8d\n\t"
        "mfence\n\t"
        "movq (%1), %%r10\n\t"
        "rdtscp\n\t"
        "mov %%eax, %%r9d\n\t"
        "sub %%r8d, %%r9d\n\t"
        "mov %%r9d, %0\n\t"
        : "=r" (tsc_low_bits)
        : "r" (line)
        : "rax", "rbx", "rcx", "rdx", "r8", "r9", "r10"
    );

    return tsc_low_bits;
}

void usage(const char *prog);
void generate_addresses(uintptr_t **, int, uintptr_t);

int main(int argc, char **argv) {
    uint32_t sample_cnt = -1, n = -1, m = -1;

    if (argc == 4)
    {
        sample_cnt = atoi(argv[1]);
        n = atoi(argv[2]);
        m = atoi(argv[3]);
    }
    else if (argc != 4 || sample_cnt < 0)
        usage(argv[0]);

    void *receiver_ds = aligned_alloc(4096, 0x8000);
    void *sender_ds = aligned_alloc(4096, 0x8000);

    srand(42);
    uintptr_t *addr_ptr_recv[n], *addr_ptr_send[m];
    generate_addresses(addr_ptr_recv, n, (uintptr_t)receiver_ds);
    generate_addresses(addr_ptr_send, m, (uintptr_t)sender_ds);

    uint32_t i = 0, j = 0, carrier_access_time = 0, signal_access_time = 0;

    // Generate linked list to store probe timings
    nodes *probe_head = (struct Node *)malloc(sizeof(struct Node));
    nodes *curr = probe_head;
    for (int i = 0; i < sample_cnt - 1; ++i)
    {
        curr->diff_access_time = 0;
        curr->next = (struct Node *)malloc(sizeof(struct Node));
        curr = curr->next;
    }
    curr->diff_access_time = 0;
    curr->next = NULL;
    curr = probe_head;

    for (i = 0; i < sample_cnt; ++i) {
        carrier_access_time = 0;
        signal_access_time = 0;

        // Carrier Signal
        for (j = 0; j < n; ++j)
            read_cache_line(addr_ptr_recv[j]);

        for (j = 0; j < n ; ++j)
            carrier_access_time += read_and_time_cache_line(addr_ptr_recv[j]);

        // Actual Signal
        for (j = 0; j < n; ++j)
            read_cache_line(addr_ptr_recv[j]);

        for (j = 0; j < n; ++j)
            signal_access_time += read_and_time_cache_line(addr_ptr_recv[j]);

        curr->carrier_access_time = carrier_access_time/n;
        curr->signal_access_time = signal_access_time/n;
        curr->diff_access_time = fabs(curr->signal_access_time - curr->carrier_access_time);
        curr = curr->next;
    }

    curr = probe_head;
    while(curr != NULL)
    {
        printf("res=%f,%f,%f\n", curr->carrier_access_time, curr->signal_access_time, curr->diff_access_time);
        curr = curr->next;
    }

    return EXIT_SUCCESS;
}

void usage(const char *prog) {
    fprintf(stderr, "Usage: %s <samples> <n> <m>\n", prog);
    exit(EXIT_FAILURE);
}

void generate_addresses(uintptr_t **array, int num_elements, uintptr_t base_addr)
{
    uintptr_t addr, set, tag;
    for (int i = 0; i < num_elements; ++i)
    {
        set = rand();
        tag = rand();
        addr = base_addr + ((tag % 8) << 12) + ((set % 64) << 6);
        array[i] = (uintptr_t *)addr;
    }
}
