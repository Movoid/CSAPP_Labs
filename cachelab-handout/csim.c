#include "cachelab.h"
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_help() {
  puts("Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>");
  puts("\t-h: Optional help flag that prints usage info");
  puts("\t-v: Optional verbose flag that displays trace info");
  puts("\t-s <s>: Number of set index bits (S = 2^s is the number of sets)");
  puts("\t-E <E>: Associativity (number of lines per set)");
  puts("\t-b <b>: Number of block bits (B = 2^b is the block size)");
  puts("\t-t <tracefile>: Name of the valgrind trace to replay");
}

// in this example config,
// there are 16 lines.
// 64bits
static size_t opt_verbose_trace = 0;
static size_t opt_sindex_bitcnt = 4; // 1<<4 == 16 sets totally
static size_t opt_set_linecnt = 1;   // 1 line per set

// `opt_block_bitcnt` is only used to calc addr bitfield.
// there is no READ/WRITE simulations.
static size_t opt_block_bitcnt = 4; // 1<<4 == 16 bytes per block
static FILE *opt_tracefile = NULL;

// *** THERE IS NO NEED TO DESIGN CACHE BLOCK
// BECUZ THERE IS ONLY LOAD/STORE.
typedef struct Line {
  int used;
  int tag;
  int last_used_time;
} Line;

static Line *base = NULL;

Line *malloc_cache() {
  size_t setcnt = 1 << opt_sindex_bitcnt;
  size_t linecnt = setcnt * opt_set_linecnt;
  Line *base = (Line *)malloc(sizeof(Line) * linecnt);
  memset(base, 0, sizeof(Line) * linecnt);
  return base;
}

static size_t time = 0;
static size_t blkoff_mask = 0;
static size_t sindex_mask = 0;
static size_t tag_mask = 0;

static int hits = 0;
static int misses = 0;
static int evictions = 0;
void visit_cacheline(size_t tag, size_t sindex) {

  Line *set_base = base + sindex * opt_set_linecnt;
  Line *cur = set_base;
  // check if hit
  Line *line_hit = NULL;
  Line *line_free = NULL;
  for (int i = 0; i < opt_set_linecnt; i++) {
    if (!cur->used) {
      // if this line is not used.
      line_free = cur;
      continue;
    }
    if (cur->tag == tag) {
      line_hit = cur;
    }
    cur++;
  }

  if (line_hit != NULL) {
    line_hit->last_used_time = time;
    hits++;
    printf("hit\n");
  } else if (line_free != NULL) {
    // not full
    line_free->last_used_time = time;
    line_free->used = 1;
    line_free->tag = tag;
    misses++;
    printf("miss\n");
  } else {
    // full, need to evict
    cur = set_base;
    Line *line_evict = cur;
    for (int i = 0; i < opt_set_linecnt; i++) {
      if (cur->last_used_time < line_evict->last_used_time) {
        line_evict = cur;
      }
      cur++;
    }
    line_evict->last_used_time = time;
    line_evict->used = 1;
    line_evict->tag = tag;
    misses++;
    evictions++;
    printf("miss eviction\n");
  }
}

int main(int argc, char *argv[]) {
  int opt;

  while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) { // NOLINT
    switch (opt) {
      case 'h':
        print_help();
        return 0;
      case 'v':
        opt_verbose_trace = 1;
        break;
      case 's':
        opt_sindex_bitcnt = atoi(optarg);
        break;
      case 'E':
        opt_set_linecnt = atoi(optarg);
        break;
      case 'b':
        opt_block_bitcnt = atoi(optarg);
        break;
      case 't':
        opt_tracefile = fopen(optarg, "r"); // readonly
        if (!opt_tracefile) {
          fprintf(stderr, "Cannot open %s. Set to STDIN.\n", optarg);
          opt_tracefile = stdin;
        }
        break;
      case '?':
        [[fallthrough]];
      default:
        fprintf(stderr, "Unknow option: %s\n", argv[optind]);
        print_help();
        return 0;
    }
  }

  printf("Set index bitcnt == %zu. Totally %zu sets.\n", opt_sindex_bitcnt, (size_t)1 << opt_sindex_bitcnt);
  printf("Line cnt per Set == %zu.\n", opt_set_linecnt);
  printf("Block offset bitcnt == %zu. Totally %zu bytes per Block.\n", opt_block_bitcnt, (size_t)1 << opt_block_bitcnt);

  // gen mask
  blkoff_mask = sindex_mask = ((size_t)1 << (opt_block_bitcnt + opt_sindex_bitcnt)) - ((size_t)1 << opt_block_bitcnt);
  tag_mask = ~(((size_t)1 << (opt_block_bitcnt + opt_sindex_bitcnt)) - 1);

  // memory access:
  // [space]operation addr,size

  printf("Init Cache...");
  base = malloc_cache();
  printf("Done.\n");

  printf("Start simulation...\n");
  if (!opt_tracefile) {
    fprintf(stderr, "Trace file is invalid. Set to STDIN.\n");
    opt_tracefile = stdin;
  }

  time = 0;
  char buf[MAX_INPUT];
  char op = 0;
  size_t addr = 0;
  size_t sz = 0;
  while (1) {
    fgets(buf, MAX_INPUT, opt_tracefile);
    if (feof(opt_tracefile)) break;
    sscanf(buf, " %c %lx,%zu\n", &op, &addr, &sz);
    printf("\n%s", buf);
    size_t end_addr = addr + sz;
    time++;
    switch (op) {
      case 'I':
        continue;
      case 'M':
        while (addr < end_addr) {
          size_t sindex = (addr & sindex_mask) >> opt_block_bitcnt;
          size_t tag = (addr & tag_mask) >> (opt_block_bitcnt + opt_sindex_bitcnt);
          visit_cacheline(tag, sindex);
          visit_cacheline(tag, sindex);
          addr += (1 << opt_block_bitcnt);
        }
        break;
      case 'L':
      case 'S':
        while (addr < end_addr) {
          size_t sindex = (addr & sindex_mask) >> opt_block_bitcnt;
          size_t tag = (addr & tag_mask) >> (opt_block_bitcnt + opt_sindex_bitcnt);
          visit_cacheline(tag, sindex);
          addr += (1 << opt_block_bitcnt);
        }
        break;
      default:
        fprintf(stderr, "Unexpected operation: %c\n", op);
        return 0;
    }
  }

  free(base);

  printf("\n");
  printSummary(hits, misses, evictions);

  printf("Exiting...\n");
  return 0;
}
