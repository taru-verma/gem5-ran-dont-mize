#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#define CACHE_SIZE 0x8000
#define PAGE_SIZE 0x1000
#define ASSOC 8

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

    printf("Number of primed lines for receiver: %d\n", n);
    printf("Number of primed lines for sender: %d\n", m);

    // Allocate 32 KB of memory (L1 cache for now)
    void *cache_mem = malloc(PAGE_SIZE * ASSOC);
    uintptr_t *curr_head = cache_mem;

    uint32_t i, j;
    uintptr_t *addr_ptr_recv[n], *addr_ptr_send[m];
    //uint32_t access_time[sample_cnt * n];
    //memset(access_time, 0, sample_cnt * n);
    double access_time = 0;

    uintptr_t base_addr = (uintptr_t)curr_head;
    //printf("DEBUG: Base address: %" PRIxPTR "\n", base_addr);

    srandom(42);

    // Generate random receiver and sender addresses
    generate_addresses(addr_ptr_recv, n, base_addr, 0);
    generate_addresses(addr_ptr_send, m, base_addr, CACHE_SIZE + PAGE_SIZE);

    //printf("DEBUG: Receiver addresses and sets:\n");
    //for (i = 0; i < n; ++i)
    //    //printf("%d\n", (int)(((uintptr_t)addr_ptr_recv[i] >> 6) & 63));
    //    printf("\t%" PRIxPTR ", %d\n", (uintptr_t)addr_ptr_recv[i], (int)(((uintptr_t)addr_ptr_recv[i] >> 6) & 63));
    //printf("DEBUG: Sender addresses and sets:\n");
    //for (i = 0; i < m; ++i)
    //    //printf("%d\n", (int)(((uintptr_t)addr_ptr_send[i] >> 6) & 63));
    //    printf("\t%" PRIxPTR ", %d\n", (uintptr_t)addr_ptr_send[i], (int)(((uintptr_t)addr_ptr_send[i] >> 6) & 63));

    //printf("\n~~~~~~~~~~~~~~~~~~~~\nStart cache communication\n");
    for (i = 0; i < sample_cnt; ++i)
    {
        access_time = 0;
        // Receiver Prime - read cache lines
        for (j = 0; j < n; ++j)
            read_cache_line(addr_ptr_recv[j]);
        // Sender Access - read cache lines
        for (j = 0; j < m; ++j)
            read_cache_line(addr_ptr_send[j]);
        // Receiver Probe - read and time access, update access_time variable
        for (j = 0; j < n; ++j)
            //access_time[i*n + j] = read_and_time_cache_line(addr_ptr_recv[j]);
            access_time += read_and_time_cache_line(addr_ptr_recv[j]);
    }
    //printf("Stop cache communication\n~~~~~~~~~~~~~~~~~~~~\n\n");

    printf("Average access time: %f\n", (double)access_time/n);
    //for (i = 0; i < sample_cnt; ++i)
    //{
    //    for (j = 0; j < n; ++j)
    //        printf("%d, ", access_time[i*n + j]);
    //    printf("\n");
    //}

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
