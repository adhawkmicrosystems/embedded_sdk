#pragma once

#include <stdbool.h>
#include <stdint.h>

//! Reads the specified blob from the filesystem. A buffer is allocated for the data which must be freed.
bool ah_blob_storage_readBlob(uint32_t blobId, uint8_t **data, unsigned *len);

//! Writes the specified blob to the filesystem. Returns the id in blobId.
bool ah_blob_storage_writeBlob(uint32_t *blobId, uint8_t *data, unsigned len);

//! Delete the specified blob from the filesystem.
bool ah_blob_storage_deleteBlob(uint32_t blobId);
