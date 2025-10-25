#ifndef WS_BRIDGE_H
#define WS_BRIDGE_H

#include <stddef.h>

/** 
 * @brief Thread-safe enqueue of a websocket text frame to the active client.
 * This can be called from any thread. Delivery is performed on the
 * Mongoose event loop via MG_EV_WAKEUP.
 * @param json The JSON string to send.
 * @param len The length of the JSON string.
 */
void ws_bridge_send_json_from_any_thread(const char *json, size_t len);

#endif // WS_BRIDGE_H
