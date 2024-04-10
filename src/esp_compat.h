/*
 * This file is part of the Pico Keys SDK distribution (https://github.com/polhenarejos/pico-keys-sdk).
 * Copyright (c) 2022 Pol Henarejos.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ESP_COMPAT_H_
#define __ESP_COMPAT_H_

#ifdef ESP_PLATFORM

#include "freertos/freertos.h"
#include "freertos/queue.h"
typedef QueueHandle_t queue_t;
#define queue_init(a,b,c) do { *(a) = xQueueCreate(c, b); } while(0)
#define queue_add_blocking(a,b) xQueueSend(*(a), b, portMAX_DELAY)
#define queue_remove_blocking(a,b) xQueueReceive(*(a), b, portMAX_DELAY)
#define queue_try_add(a,b) xQueueSend(*(a), b, 0)
#define queue_is_empty(a) (uxQueueMessagesWaiting(*(a)) == 0)
#define queue_try_remove(a,b) xQueueReceive(*(a), b, 0)
extern TaskHandle_t hcore0, hcore1;
#define multicore_launch_core1(a) xTaskCreate((void(*)(void *))a, "core1", 4096, NULL, CONFIG_TINYUSB_TASK_PRIORITY + 2, &hcore1)
#define multicore_reset_core1() do { if (hcore1) { eTaskState e = eTaskGetState(hcore1); if (e == eRunning) { vTaskDelete(hcore1); }} }while(0)
#define sleep_ms(a) vTaskDelay(a / portTICK_PERIOD_MS)
static inline uint32_t board_millis(void) {
    return ( ( ((uint64_t) xTaskGetTickCount()) * 1000) / configTICK_RATE_HZ );
}
#define PICO_UNIQUE_BOARD_ID_SIZE_BYTES 8
typedef struct { uint8_t id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES]; } pico_unique_board_id_t;
#define pico_get_unique_board_id(a) memset(a, 0, sizeof(pico_unique_board_id_t))
#define pico_get_unique_board_id_string(a, b) \
    do {                                      \
        pico_unique_board_id_t t;             \
        pico_get_unique_board_id(&t);          \
        snprintf(a, b, "%02X%02X%02X%02X%02X%02X%02X%02X", t.id[0], t.id[1], t.id[2], t.id[3], t.id[4], t.id[5], t.id[6], t.id[7]); } while(0)
typedef SemaphoreHandle_t mutex_t;
typedef SemaphoreHandle_t semaphore_t;
#define mutex_init(a) do { *(a) = xSemaphoreCreateMutex();} while(0)
#define mutex_try_enter(a,b) xSemaphoreTake(*(a), 0)
#define mutex_enter_blocking(a) xSemaphoreTake(*(a), portMAX_DELAY)
#define mutex_exit(a) xSemaphoreGive(*(a))
#define sem_init(a,b,c) do { *(a) = xSemaphoreCreateCounting(c, b); } while(0)
#define sem_release(a) xSemaphoreGive(*(a))
#define sem_acquire_blocking(a) xSemaphoreTake(*(a), portMAX_DELAY)
#define multicore_lockout_victim_init() (void)0
static inline bool multicore_lockout_start_timeout_us(int a) {
    vTaskSuspend(hcore1); return true; }
static inline bool multicore_lockout_end_timeout_us(int a) {
    vTaskResume(hcore1); return true; }

#endif

#endif
