/*
 * Architecture Lab: Part A
 *
 * High level specs for the functions that the students will rewrite
 * in Y86-64 assembly language
 */

/* $begin examples */
/* linked list element */
typedef struct ELE {
  long val;
  struct ELE *next;
} *list_ptr;

/* sum_list - Sum the elements of a linked list */
long sum_list(list_ptr ls) {
  long val = 0; // get sum of all nodes
  while (ls) {
    val += ls->val;
    ls = ls->next;
  }
  return val;
}

/* rsum_list - Recursive version of sum_list */
long rsum_list(list_ptr ls) {
  if (!ls)
    return 0;
  else {
    long val = ls->val;
    long rest = rsum_list(ls->next);
    return val + rest; // recursive getSum, same to `sum_list`
  }
}

/* copy_block - Copy src to dest and return xor checksum of src */
long copy_block(long *src, long *dest, long len) {
  long result = 0;
  while (len > 0) {
    long val = *src++; // get val, src move to node.next
    *dest++ = val;     // change val, dest move to node.next
    result ^= val;     // get XOR sum
    len--;
  }
  return result;
}
/* $end examples */
