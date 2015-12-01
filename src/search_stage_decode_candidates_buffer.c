/*
 * PROJECT: GEMMapper
 * FILE: search_stage_decode_candidates_buffer.c
 * DATE: 06/06/2012
 * AUTHOR(S): Santiago Marco-Sola <santiagomsola@gmail.com>
 */

#include "search_stage_decode_candidates_buffer.h"

/*
 * Setup
 */
search_stage_decode_candidates_buffer_t* search_stage_decode_candidates_buffer_new(
    const gpu_buffer_collection_t* const gpu_buffer_collection,const uint64_t buffer_no,
    fm_index_t* const fm_index,const bool cpu_emulated) {
  // Alloc
  search_stage_decode_candidates_buffer_t* const decode_candidates_buffer =
      mm_alloc(search_stage_decode_candidates_buffer_t);
  // Init
  decode_candidates_buffer->gpu_buffer_fmi_decode =
      gpu_buffer_fmi_decode_new(gpu_buffer_collection,buffer_no,fm_index);
  if (cpu_emulated) gpu_buffer_fmi_decode_set_device_cpu(decode_candidates_buffer->gpu_buffer_fmi_decode);
  const uint64_t max_queries = gpu_buffer_fmi_decode_get_max_queries(decode_candidates_buffer->gpu_buffer_fmi_decode);
  decode_candidates_buffer->archive_searches = vector_new(max_queries,archive_search_t*);
  // Return
  return decode_candidates_buffer;
}
void search_stage_decode_candidates_buffer_clear(
    search_stage_decode_candidates_buffer_t* const decode_candidates_buffer,
    archive_search_cache_t* const archive_search_cache) {
  gpu_buffer_fmi_decode_clear(decode_candidates_buffer->gpu_buffer_fmi_decode);
  // Return searches to the cache
  if (archive_search_cache!=NULL) {
    VECTOR_ITERATE(decode_candidates_buffer->archive_searches,archive_search,n,archive_search_t*) {
      archive_search_cache_free(archive_search_cache,*archive_search);
    }
  }
  // Clear searches vector
  vector_clear(decode_candidates_buffer->archive_searches);
}
void search_stage_decode_candidates_buffer_delete(
    search_stage_decode_candidates_buffer_t* const decode_candidates_buffer,
    archive_search_cache_t* const archive_search_cache) {
  gpu_buffer_fmi_decode_delete(decode_candidates_buffer->gpu_buffer_fmi_decode);
  // Return searches to the cache
  VECTOR_ITERATE(decode_candidates_buffer->archive_searches,archive_search,n,archive_search_t*) {
    archive_search_cache_free(archive_search_cache,*archive_search);
  }
  // Delete searches vector
  vector_delete(decode_candidates_buffer->archive_searches);
}
/*
 * Occupancy
 */
bool search_stage_decode_candidates_buffer_fits(
    search_stage_decode_candidates_buffer_t* const decode_candidates_buffer,
    archive_search_t* const archive_search_end1,archive_search_t* const archive_search_end2) {
  // Get buffer limits
  gpu_buffer_fmi_decode_t* const gpu_buffer_fmi_decode = decode_candidates_buffer->gpu_buffer_fmi_decode;
  const uint64_t max_queries = gpu_buffer_fmi_decode_get_max_queries(gpu_buffer_fmi_decode);
  const uint64_t num_queries = gpu_buffer_fmi_decode_get_num_queries(gpu_buffer_fmi_decode);
  // Get number of candidates to decode
  uint64_t num_decode_candidates = archive_search_get_num_decode_candidates(archive_search_end1);
  if (archive_search_end2 != NULL) {
    num_decode_candidates += archive_search_get_num_decode_candidates(archive_search_end2);
  }
  // Return
  return num_queries + num_decode_candidates <= max_queries;
}
/*
 * Send/Receive
 */
void search_stage_decode_candidates_buffer_send(
    search_stage_decode_candidates_buffer_t* const decode_candidates_buffer) {
  gpu_buffer_fmi_decode_send(decode_candidates_buffer->gpu_buffer_fmi_decode);
}
void search_stage_decode_candidates_buffer_receive(
    search_stage_decode_candidates_buffer_t* const decode_candidates_buffer) {
  gpu_buffer_fmi_decode_receive(decode_candidates_buffer->gpu_buffer_fmi_decode);
}
/*
 * Accessors
 */
void search_stage_decode_candidates_buffer_add(
    search_stage_decode_candidates_buffer_t* const decode_candidates_buffer,
    archive_search_t* const archive_search) {
  // Add archive-search
  vector_insert(decode_candidates_buffer->archive_searches,archive_search,archive_search_t*);
}
void search_stage_decode_candidates_buffer_retrieve(
    search_stage_decode_candidates_buffer_t* const decode_candidates_buffer,
    const uint64_t search_idx,archive_search_t** const archive_search) {
  // Retrieve archive-search
  *archive_search = *vector_get_elm(decode_candidates_buffer->archive_searches,search_idx,archive_search_t*);
}

