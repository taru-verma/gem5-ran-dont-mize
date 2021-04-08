#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#define CACHE_SIZE 0x8000
#define PAGE_SIZE 0x1000
#define ASSOC 8

struct Node
{
    uint32_t line_access_time;
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
        "cpuid\n\t"
        "rdtsc\n\t"
        "mov %%eax, %%r8d\n\t"
        "mov (%1), %%r10\n\t"
        "rdtscp\n\t"
        "mov %%eax, %%r9d\n\t"
        "cpuid\n\t"
        "sub %%r8d, %%r9d\n\t"
        "mov %%r9d, %0\n\t"
        : "=r" (tsc_low_bits)
        : "r" (line)
        : "rax", "rbx", "rcx", "rdx", "r8", "r9", "r10"
    );

    return tsc_low_bits;
}

void usage(const char *prog);
void generate_addresses(uintptr_t **, int, uintptr_t, int);

int main(int argc, char **argv) {
    uint32_t sample_cnt = -1;
    int n = -1, m = -1;

    if (argc == 4)
    {
        sample_cnt = atoi(argv[1]);
        n = atoi(argv[2]);
        m = atoi(argv[3]);
    }
    if (argc != 4 || sample_cnt < 0)
        usage(argv[0]);

    // Allocate 32 KB of memory (L1 cache for now)
    void *cache_mem = malloc(PAGE_SIZE * ASSOC);
    uintptr_t *curr_head = cache_mem;

    uint32_t i, j;
    uintptr_t *addr_ptr_recv[n], *addr_ptr_send[m];
    double total_access_time = 0, mean = 0, squared_diff = 0, variance = 0, std_dev = 0;

    uintptr_t base_addr = (uintptr_t)curr_head;

    srandom(42);

    // Generate random receiver and sender addresses
    generate_addresses(addr_ptr_recv, n, base_addr, 0);
    //generate_addresses(addr_ptr_send, m, base_addr, CACHE_SIZE + PAGE_SIZE);

    // Generate linked list to store probe timings
    nodes *probe_head = (struct Node *)malloc(sizeof(struct Node));
    nodes *curr = probe_head;
    for (int i = 0; i < sample_cnt - 1; ++i)
    {
        curr->line_access_time = 0;
        curr->next = (struct Node *)malloc(sizeof(struct Node));
        curr = curr->next;
    }
    curr->line_access_time = 0;
    curr->next = NULL;
    curr = probe_head;

    for (i = 0; i < sample_cnt; ++i)
    {
        // Receiver Prime - read cache lines
        for (j = 0; j < n; ++j)
            read_cache_line(addr_ptr_recv[j]);
        // Sender Access - read cache lines
        //for (j = 0; j < m; ++j)
        //    read_cache_line(addr_ptr_send[j]);
        // Receiver Probe - read and time access, update access_time variable
        for (j = 0; j < n; ++j)
            curr->line_access_time += read_and_time_cache_line(addr_ptr_recv[j]);
        curr->line_access_time /= n;
        curr = curr->next;
    }

    // Add divide by zero error checking just in case
    curr = probe_head;
    while(curr != NULL)
    {
        total_access_time += curr->line_access_time;
        curr = curr->next;
    }
    mean = total_access_time/(sample_cnt);

    curr = probe_head;
    while(curr != NULL)
    {
        squared_diff += pow((curr->line_access_time - mean), 2);
        curr = curr->next;
    }
    variance = squared_diff/(sample_cnt);
    std_dev = sqrt(variance);

    printf("res=%d,%d,%f,%f,%f\n", n, m, mean, variance, std_dev);

    curr = probe_head;
    while(curr != NULL)
    {
        printf("%d\n", curr->line_access_time);
        curr = curr->next;
    }

    return EXIT_SUCCESS;
}

void usage(const char *prog) {
    fprintf(stderr, "Usage: %s <samples> <n> <m>\n", prog);
    exit(EXIT_FAILURE);
}

void generate_addresses(uintptr_t **array, int num_elements, uintptr_t base_addr, int offset)
{
    uintptr_t addr, set, tag;
    for (int i = 0; i < num_elements; ++i)
    {
        set = random();
        tag = random();
        addr = base_addr + offset + ((tag % 8) << 12) + ((set % 64) << 6);
        array[i] = (uintptr_t *)addr;
    }
}
