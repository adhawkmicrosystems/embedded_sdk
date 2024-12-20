#include "blob_api.h"

#include "ah_types.h"
#include "bsp_alloc.h"
#include "bsp_blob_storage.h"
#include "bsp_random.h"
#include "packet.h"
#include "request.h"
#include "util.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define BLOB_PACKET_OVERHEAD 4  // 1 byte packet type, 1 byte blob type, 2 bytes offset
#define BLOB_CHUNK_SIZE (API2_MAX_PACKET_SIZE - API2_PKT_OVERHEAD - API2_REQUEST_OVERHEAD - BLOB_PACKET_OVERHEAD)

ah_result ah_blob_api_handleLoadBlob(ah_blobType blobType, uint32_t blobId)
{
    uint8_t *blobData;
    unsigned blobSize;
    if (!ah_blob_storage_readBlob(blobId, &blobData, &blobSize))
    {
        return ah_result_Failure;
    }

    ah_result rv = ah_blob_api_writeBlob(blobType, blobData, blobSize);
    ah_free(blobData);
    return rv;
}

ah_result ah_blob_api_handleSaveBlob(ah_blobType blobType, uint32_t *blobId)
{
    uint8_t *blobData;
    uint16_t blobSize;
    ah_result rv = ah_blob_api_readBlob(blobType, &blobData, &blobSize);
    if (rv != ah_result_Success)
    {
        return rv;
    }

    if (!ah_blob_storage_writeBlob(blobId, blobData, blobSize))
    {
        rv = ah_result_Failure;
    }
    ah_free(blobData);
    return rv;
}

ah_result ah_blob_api_handleDeleteBlob(uint32_t blobId)
{
    if (!ah_blob_storage_deleteBlob(blobId))
    {
        return ah_result_Failure;
    }
    return ah_result_Success;
}

ah_result ah_blob_api_loadBlob(ah_blobType blobType, uint32_t blobId)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t blobType;
        uint32_t blobId;
    } request = {
        .type = ah_packetType_LoadBlob,
        .blobType = blobType,
        .blobId = blobId,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_blob_api_saveBlob(ah_blobType blobType, uint32_t *blobId)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t blobType;
    } request = {
        .type = ah_packetType_LoadBlob,
        .blobType = blobType,
    };
    struct __attribute__((__packed__)) responseData
    {
        uint32_t blobId;
    };

    void *responsePtr = NULL;
    ah_result rv = ah_request_sendRequest(&request, sizeof(request), &responsePtr, NULL);
    if (rv == ah_result_Success)
    {
        struct responseData *response = responsePtr;
        *blobId = response->blobId;
    }
    ah_free(responsePtr);
    return rv;
}

ah_result ah_blob_api_deleteBlob(uint32_t blobId)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint32_t blobId;
    } request = {
        .type = ah_packetType_DeleteBlob,
        .blobId = blobId,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_blob_api_readBlob(ah_blobType blobType, uint8_t **blobData, uint16_t *blobSize)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t blobType;
    } sizeRequest = {
        .type = ah_packetType_BlobSize,
        .blobType = blobType,
    };
    struct __attribute__((__packed__)) sizeResponseData
    {
        uint16_t blobSize;
    };

    void *sizeResponsePtr = NULL;
    ah_result rv = ah_request_sendRequest(&sizeRequest, sizeof(sizeRequest), &sizeResponsePtr, NULL);
    if (rv != ah_result_Success)
    {
        ah_free(sizeResponsePtr);
        return rv;
    }
    struct sizeResponseData *sizeResponse = sizeResponsePtr;
    *blobSize = sizeResponse->blobSize;
    ah_free(sizeResponsePtr);
    if (!*blobSize)
    {
        *blobData = NULL;
        return rv;
    }

    *blobData = ah_malloc(*blobSize);
    if (!blobData)
    {
        return ah_result_Failure;
    }

    unsigned offset = 0;
    while (offset < *blobSize)
    {
        struct __attribute__((__packed__))
        {
            uint8_t type;
            uint8_t blobType;
            uint16_t offset;
        } dataRequest = {
            .type = ah_packetType_BlobData,
            .blobType = blobType,
            .offset = offset,
        };

        void *responseData = NULL;
        unsigned responseLen;
        rv = ah_request_sendRequest(&dataRequest, sizeof(dataRequest), &responseData, &responseLen);
        if (rv != ah_result_Success)
        {
            ah_free(responseData);
            ah_free(*blobData);
            return rv;
        }

        memcpy(*blobData + offset, (uint8_t *)responseData, responseLen);
        offset += responseLen;
        ah_free(responseData);
    }

    return ah_result_Success;
}

ah_result ah_blob_api_writeBlob(ah_blobType blobType, const uint8_t *blobData, uint16_t blobSize)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t blobType;
        uint16_t blobSize;
    } sizeRequest = {
        .type = ah_packetType_BlobSize,
        .blobType = blobType,
        .blobSize = blobSize,
    };

    ah_result rv = ah_request_sendRequest(&sizeRequest, sizeof(sizeRequest), NULL, NULL);
    if (rv != ah_result_Success)
    {
        return rv;
    }

    unsigned offset = 0;
    while (offset < blobSize)
    {
        struct __attribute__((__packed__))
        {
            uint8_t type;
            uint8_t blobType;
            uint16_t offset;
            uint8_t data[BLOB_CHUNK_SIZE];
        } dataRequest = {
            .type = ah_packetType_BlobData,
            .blobType = blobType,
            .offset = offset,
        };

        unsigned dataLen = AH_MIN(BLOB_CHUNK_SIZE, blobSize - offset);
        memcpy(dataRequest.data, blobData + offset, dataLen);
        unsigned dataRequestLen = BLOB_PACKET_OVERHEAD + dataLen;

        rv = ah_request_sendRequest(&dataRequest, dataRequestLen, NULL, NULL);
        if (rv != ah_result_Success)
        {
            return rv;
        }

        offset += dataLen;
    }

    return ah_result_Success;
}

ah_result ah_blob_api_clearBlob(ah_blobType blobType)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t blobType;
        uint16_t blobSize;
    } sizeRequest = {
        .type = ah_packetType_BlobSize,
        .blobType = blobType,
        .blobSize = 0,
    };

    ah_result rv = ah_request_sendRequest(&sizeRequest, sizeof(sizeRequest), NULL, NULL);
    if (rv != ah_result_Success)
    {
        return rv;
    }

    return ah_result_Success;
}
