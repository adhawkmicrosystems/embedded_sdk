#pragma once

#include "ah_types.h"

#include <stdint.h>

ah_result ah_blob_api_handleLoadBlob(ah_blobType blobType, uint32_t blobId);

ah_result ah_blob_api_handleSaveBlob(ah_blobType blobType, uint32_t *blobId);

ah_result ah_blob_api_handleDeleteBlob(uint32_t blobId);

ah_result ah_blob_api_loadBlob(ah_blobType blobType, uint32_t blobId);

ah_result ah_blob_api_saveBlob(ah_blobType blobType, uint32_t *blobId);

ah_result ah_blob_api_deleteBlob(uint32_t blobId);

ah_result ah_blob_api_readBlob(ah_blobType blobType, uint8_t **blobData, uint16_t *blobSize);

ah_result ah_blob_api_writeBlob(ah_blobType blobType, const uint8_t *blobData, uint16_t blobSize);

ah_result ah_blob_api_clearBlob(ah_blobType blobType);
