/*
 * PROJECT: GEMMapper
 * FILE: search_interval_set.c
 * DATE: 06/06/2013
 * AUTHOR(S): Santiago Marco-Sola <santiagomsola@gmail.com>
 * DESCRIPTION:
 */

#include "search_interval_set.h"

/*
 * Constants
 */
#define INTERVAL_SET_NUN_INITIAL_INTERVALS 50

/*
 * Setup
 */
void search_interval_set_init(search_interval_set_t* const search_interval_set) {
  search_interval_set->intervals = vector_new(INTERVAL_SET_NUN_INITIAL_INTERVALS,search_interval_t);
}
void search_interval_set_clear(search_interval_set_t* const search_interval_set) {
  vector_clear(search_interval_set->intervals);
}
void search_interval_set_destroy(search_interval_set_t* const search_interval_set) {
  vector_delete(search_interval_set->intervals);
}
/*
 * Counting
 */
uint64_t search_interval_set_count_intervals(search_interval_set_t* const search_interval_set) {
  return vector_get_used(search_interval_set->intervals);
}
uint64_t search_interval_set_count_intervals_length(search_interval_set_t* const search_interval_set) {
  uint64_t count = 0;
  SEARCH_INTERVAL_SET_ITERATE(search_interval_set,interval) {
    count += interval->hi - interval->lo;
  }
  return count;
}
uint64_t search_interval_set_count_intervals_length_thresholded(
    search_interval_set_t* const search_interval_set,const uint64_t max_error) {
  uint64_t count = 0;
  SEARCH_INTERVAL_SET_ITERATE(search_interval_set,interval) {
    if (interval->distance <= max_error) count += interval->hi - interval->lo;
  }
  return count;
}
/*
 * Adding
 */
void search_interval_set_add(
    search_interval_set_t* const search_interval_set,const uint64_t lo,const uint64_t hi,
    const uint64_t distance,const uint64_t length) {
  // Allocate
  search_interval_t* interval;
  vector_alloc_new(search_interval_set->intervals,search_interval_t,interval);
  // Add
  interval->lo = lo;
  interval->hi = hi;
  interval->distance = distance;
  interval->length = length;
}
/*
 * Set Operators
 */
void search_interval_set_union(search_interval_set_t* const search_interval_set_a,search_interval_set_t* const search_interval_set_b) {
  // Appends to @search_interval_set_a the intervals contained into @search_interval_set_b (union set)
  const uint64_t total_size = vector_get_used(search_interval_set_a->intervals) + vector_get_used(search_interval_set_b->intervals);
  vector_reserve(search_interval_set_a->intervals,total_size,false);
  search_interval_t* int_set_a = vector_get_free_elm(search_interval_set_a->intervals,search_interval_t);
  SEARCH_INTERVAL_SET_ITERATE(search_interval_set_b,int_set_b) {
    int_set_a->lo = int_set_b->lo;
    int_set_a->hi = int_set_b->hi;
    int_set_a->distance = int_set_b->distance;
    int_set_a->length = int_set_b->length;
    ++int_set_a;
  }
  vector_set_used(search_interval_set_a->intervals,total_size);
}
void search_interval_set_subtract(search_interval_set_t* const result_set,search_interval_set_t* const exclusion_set) {
  // Subtracts to @result_set the intervals contained in @exclusion_set (difference set)
  const uint64_t exclusion_set_size = vector_get_used(exclusion_set->intervals);
  uint64_t result_set_size = vector_get_used(result_set->intervals);
  uint64_t i, j;
  for (i=0;i<result_set_size;++i) {
    search_interval_t* int_res = vector_get_mem(result_set->intervals,search_interval_t) + i;
    search_interval_t* int_excl = vector_get_mem(exclusion_set->intervals,search_interval_t);
    for (j=0;j<exclusion_set_size;++j,++int_excl) {
      const uint64_t hi1 = int_res->hi;
      const uint64_t lo1 = int_res->lo;
      const uint64_t hi2 = int_excl->hi;
      const uint64_t lo2 = int_excl->lo;
      if (hi1 <= lo2 || hi2 <= lo1) { // Disjoint intervals
        continue;
      } else {
        if (lo2 <= lo1 && hi1 <= hi2) { // Full subtraction
          int_res->lo = int_res->hi; // Close the interval
        } else if (lo1 < lo2 && hi2 < hi1) { // Exclusion inside result
          // Add the chunk to the end
          vector_reserve(result_set->intervals,result_set_size+1,false);
          search_interval_t* const int_res_chunk = vector_get_mem(result_set->intervals,search_interval_t) + result_set_size;
          int_res_chunk->lo = hi2;
          int_res_chunk->hi = hi1;
          int_res_chunk->distance = int_res->distance;
          int_res_chunk->length = int_res->length;
          // Shrink the interval
          int_res->hi = lo2;
          ++result_set_size;
        } else if (lo2 == lo1 && lo1 < hi2) { // Exclusion overlaps left side of result
          int_res->lo = hi2;
        } else /* if (lo2 < hi1 && hi1 == hi2) */ { // Exclusion overlaps right side of result
          int_res->hi = lo2;
        }
      }
    }
  }
  vector_set_used(result_set->intervals,result_set_size);
}
