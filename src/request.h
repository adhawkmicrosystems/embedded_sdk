#pragma once

#include "ah_types.h"

#include <stdbool.h>
#include <stdint.h>

bool ah_request_init(void);

bool ah_request_deinit(void);

bool ah_request_waitingForResponse(uint16_t requestId);

//! Should called for each non streaming packet that is received
void ah_request_handleResponse(const uint8_t *data, uint32_t len);

//! This function blocks until a response is received or a timeout triggers
ah_result ah_request_sendRequest(const void *requestData, uint32_t requestLen,
                                 void **response, uint32_t *responseLen);

//! Non-blocking function, responses must be handled manually
void ah_request_forwardRequest(const uint8_t *request, uint32_t requestLen);

void ah_request_sendRequestNoResponse(const void *request, uint32_t requestLen);
