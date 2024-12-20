#include "bsp_blob_storage.h"

#include "bsp_alloc.h"
#include "bsp_logging.h"
#include "bsp_random.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static void initializeBlobDirectory(const char *blobFolder)
{
    char tmp[FILENAME_MAX];
    strcpy(tmp, blobFolder);

    for (char *p = strchr(tmp + 1, '/'); p; p = strchr(p + 1, '/'))
    {
        *p = '\0';
        if (mkdir(tmp, 0774) == -1)
        {
            if (errno != EEXIST)
            {
                *p = '/';
                return;
            }
        }
        *p = '/';
    }
    return;
}

static bool getBlobFilePath(uint32_t blobId, char *filePath)
{
    char *dataDir = getenv("AH_DATA_DIR");
    if (!dataDir)
    {
        dataDir = ".";
    }

    char blobFolder[FILENAME_MAX];
    snprintf(blobFolder, FILENAME_MAX, "%s/blobs/", dataDir);
    initializeBlobDirectory(blobFolder);  // Create the directory if it doesn't exist

    snprintf(filePath, FILENAME_MAX, "%s/blob_%u.bin", blobFolder, blobId);
}

bool ah_blob_storage_readBlob(uint32_t blobId, uint8_t **data, unsigned *len)
{
    char filePath[FILENAME_MAX];
    getBlobFilePath(blobId, filePath);

    struct stat sb;
    if (stat(filePath, &sb) != 0)
    {
        return false;
    }
    *len = sb.st_size;

    FILE *blobFile = fopen(filePath, "rb");
    if (!blobFile)
    {
        ah_log_error("Failed to open %s", filePath);
        return false;
    }

    uint8_t *buffer = malloc(*len);
    if (!buffer)
    {
        fclose(blobFile);
        return false;
    }
    if (fread(buffer, *len, 1, blobFile) != 1)
    {
        ah_log_error("Failed to read %s", filePath);
        free(buffer);
        fclose(blobFile);
        return false;
    }

    *data = buffer;
    fclose(blobFile);
    return true;
}

bool ah_blob_storage_writeBlob(uint32_t *blobId, uint8_t *data, unsigned len)
{
    ah_random(blobId, sizeof(*blobId));

    char filePath[FILENAME_MAX];
    getBlobFilePath(*blobId, filePath);

    FILE *blobFile = fopen(filePath, "wb");
    if (!blobFile)
    {
        ah_log_error("Failed to create %s", filePath);
        return false;
    }

    if (fwrite(data, len, 1, blobFile) != 1)
    {
        ah_log_error("Failed to write %s", filePath);
        fclose(blobFile);
        return false;
    }

    fclose(blobFile);
    return true;
}

bool ah_blob_storage_deleteBlob(uint32_t blobId)
{
    char filePath[FILENAME_MAX];
    getBlobFilePath(blobId, filePath);

    return remove(filePath) == 0;
}
