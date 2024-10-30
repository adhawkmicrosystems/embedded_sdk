#pragma once

#include <stdbool.h>

// The tracker info module caches core information about the tracker so it can be queried easily

bool ah_tracker_info_init(void);

void ah_tracker_info_deinit(void);

bool ah_tracker_info_reload(void);

unsigned ah_tracker_info_getEyeMask(void);

unsigned ah_tracker_info_getCameraType(void);

unsigned ah_tracker_info_getProductId(void);

char *ah_tracker_info_getSerialNumber(void);

char *ah_tracker_info_getFirmwareVersion(void);

char *ah_tracker_info_getAPIVersion(void);

bool ah_tracker_info_isUpdater(void);