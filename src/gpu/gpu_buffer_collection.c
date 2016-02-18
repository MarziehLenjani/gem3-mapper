/*
 * PROJECT: GEMMapper
 * FILE: gpu_buffer_collection.c
 * DATE: 04/09/2014
 * AUTHOR(S): Alejandro Chacon <alejandro.chacon@uab.es>
 *            Santiago Marco-Sola <santiagomsola@gmail.com>
 */

#include "gpu/gpu_buffer_collection.h"
#include "resources/gpu_modules/gpu_interface.h"

/*
 * Profile
 */
#define PROFILE_LEVEL PHIGH

/*
 * CUDA Supported
 */
#ifdef HAVE_CUDA
/*
 * Setup
 */
gpu_buffer_collection_t* gpu_buffer_collection_new(
    char* const gpu_index_name,
    const uint64_t num_buffers,
    const uint64_t buffer_size,
    const bool verbose) {
  PROFILE_START(GP_GPU_BUFFER_COLLECTION_INIT,PROFILE_LEVEL);
  // Allocate Buffer Collection
  gpu_buffer_collection_t* const buffer_collection = mm_alloc(gpu_buffer_collection_t);
  buffer_collection->num_buffers = num_buffers;
  // Initialize GPU Runtime
  gpu_buffers_dto_t* const gpu_buffers_dto = mm_alloc(gpu_buffers_dto_t);
  gpu_buffers_dto->buffer = NULL;
  gpu_buffers_dto->numBuffers = num_buffers;
  gpu_buffers_dto->maxMbPerBuffer = CONVERT_B_TO_MB(buffer_size);
  gpu_buffers_dto->activeModules = GPU_ALL_MODULES;
  buffer_collection->gpu_buffers_dto = gpu_buffers_dto;
  gpu_index_dto_t gpu_index_dto = {
      .filename         = gpu_index_name,
      //Initialize FM-Index
      .fmi.h_fmi        = NULL,
      .fmi.bwtSize      = 0,
      .fmi.indexCoding  = GPU_INDEX_GEM_FILE,
      //Initialize Suffix-Array
      .sa.h_sa          = NULL,
      .sa.numEntries    = 0,
      .sa.samplingRate  = 0,
      .sa.indexCoding   = GPU_INDEX_GEM_FILE,
  };
  gpu_reference_dto_t gpu_reference_dto = {
      .reference = gpu_index_name,
      .refCoding = GPU_REF_GEM_FILE,
      .refSize   = 0,
  };
  gpu_info_dto_t gpu_info_dto = {
      .selectedArchitectures = GPU_ARCH_SUPPORTED,
      .userAllocOption = GPU_LOCAL_OR_REMOTE_DATA,
  };
  gpu_init_buffers_(gpu_buffers_dto,&gpu_index_dto,&gpu_reference_dto,&gpu_info_dto,verbose);
  buffer_collection->internal_buffers = gpu_buffers_dto->buffer;
  // Return
  PROFILE_STOP(GP_GPU_BUFFER_COLLECTION_INIT,PROFILE_LEVEL);
  return buffer_collection;
}
void gpu_buffer_collection_delete(gpu_buffer_collection_t* const gpu_buffer_collection) {
  gpu_destroy_buffers_((gpu_buffers_dto_t*)gpu_buffer_collection->gpu_buffers_dto); // Destroy buffers
  mm_free(gpu_buffer_collection->gpu_buffers_dto); // Free DTO
  mm_free(gpu_buffer_collection); // Free Handler
}
/*
 * Accessors
 */
void* gpu_buffer_collection_get_buffer(
    const gpu_buffer_collection_t* const gpu_buffer_collection,
    const uint64_t buffer_no) {
  return gpu_buffer_collection->internal_buffers[buffer_no];
}
/*
 * CUDA NOT-Supported
 */
#else
/*
 * Setup
 */
gpu_buffer_collection_t* gpu_buffer_collection_new(
    char* const gpu_index_name,
    const uint64_t num_buffers,
    const uint64_t buffer_size,
    const bool verbose) {
  GEM_CUDA_NOT_SUPPORTED();
  return NULL;
}
void gpu_buffer_collection_delete(
    gpu_buffer_collection_t* const gpu_buffer_collection) { GEM_CUDA_NOT_SUPPORTED(); }
/*
 * Accessors
 */
void* gpu_buffer_collection_get_buffer(
    const gpu_buffer_collection_t* const gpu_buffer_collection,
    const uint64_t buffer_no) { GEM_CUDA_NOT_SUPPORTED(); return NULL; }
#endif /* HAVE_CUDA */