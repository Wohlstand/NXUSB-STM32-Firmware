/*
 * switch.h
 *
 *  Created on: 10 сент. 2026 г.
 *      Author: vitaly
 */

#ifndef SOURCES_SWITCH_SWITCH_H_
#define SOURCES_SWITCH_SWITCH_H_

#include <stdbool.h>

extern void switch_pro_handle_output(const uint8_t *data, uint16_t len);

extern bool switch_pro_send_queued(void);

extern void switch_hit(char key);

extern void switch_update_state(void);

#endif /* SOURCES_SWITCH_SWITCH_H_ */
