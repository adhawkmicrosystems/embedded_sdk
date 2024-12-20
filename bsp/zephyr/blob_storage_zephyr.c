#include "bsp_blob_storage.h"

#include "bsp_alloc.h"
#include "bsp_logging.h"
#include "bsp_random.h"

#include <stdio.h>
#include <string.h>

#include <zephyr/fs/fs.h>

#define BLOB_PATH "/ah_sdk/blobs"

// FILENAME_MAX is defined as 1024 which blows stacks
#define AH_FILENAME_MAX (128)

static void initializeBlobDirectory(void)
{
    struct fs_dirent dirent;
    int err = fs_stat(BLOB_PATH, &dirent);
    if (err == -ENOENT)
    {
        fs_mkdir(BLOB_PATH);
    }
}

static void getBlobFilePath(uint32_t blobId, char *filePath)
{
    initializeBlobDirectory();  // Create the directory if it doesn't exist

    snprintf(filePath, AH_FILENAME_MAX, "%s/blob_%u.bin", BLOB_PATH, blobId);
}

bool ah_blob_storage_readBlob(uint32_t blobId, uint8_t **data, unsigned *len)
{
    char filePath[AH_FILENAME_MAX];
    getBlobFilePath(blobId, filePath);

    struct fs_dirent dirent;
    if (fs_stat(filePath, &dirent) == -ENOENT)
    {
        return false;
    }
    *len = dirent.size;

    struct fs_file_t fp;
    fs_file_t_init(&fp);

    if (fs_open(&fp, filePath, FS_O_READ) != 0)
    {
        ah_log_error("Failed to open %s", filePath);
        return false;
    }

    uint8_t *buffer = ah_malloc(*len);
    if (!buffer)
    {
        fs_close(&fp);
        return false;
    }

    if (fs_read(&fp, buffer, *len) != *len)
    {
        ah_log_error("Failed to read %s", filePath);
        ah_free(buffer);
        fs_close(&fp);
    }

    fs_close(&fp);
    *data = buffer;
    return true;
}

bool ah_blob_storage_writeBlob(uint32_t *blobId, uint8_t *data, unsigned len)
{
    ah_random(blobId, sizeof(*blobId));

    char filePath[AH_FILENAME_MAX];
    getBlobFilePath(*blobId, filePath);

    struct fs_file_t fp;
    fs_file_t_init(&fp);

    if (fs_open(&fp, filePath, FS_O_CREATE | FS_O_WRITE) != 0)
    {
        ah_log_error("Failed to create %s", filePath);
        return false;
    }

    if (fs_write(&fp, data, len) != len)
    {
        ah_log_error("Failed to write %s", filePath);
        fs_close(&fp);
        return false;
    }

    fs_close(&fp);
    return true;
}

bool ah_blob_storage_deleteBlob(uint32_t blobId)
{
    char filePath[AH_FILENAME_MAX];
    getBlobFilePath(blobId, filePath);

    return fs_unlink(filePath) == 0;
}
