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

#include "pico_keys.h"
#include "file.h"

#ifndef ENABLE_EMULATION

phy_data_t phy_data;

int phy_serialize_data(const phy_data_t *phy, uint8_t *data, uint16_t *len) {
    if (!phy || !data || !len) {
        return PICOKEY_ERR_NULL_PARAM;
    }
    uint8_t *p = data;
    if (phy->vidpid_present) {
        *p++ = PHY_VIDPID;
        *p++ = phy->vidpid[1];
        *p++ = phy->vidpid[0];
        *p++ = phy->vidpid[3];
        *p++ = phy->vidpid[2];
    }
    if (phy->led_gpio_present) {
        *p++ = PHY_LED_GPIO;
        *p++ = phy->led_gpio;
    }
    if (phy->led_brightness_present) {
        *p++ = PHY_LED_BTNESS;
        *p++ = phy->led_brightness;
    }
    *p++ = PHY_OPTS;
    *p++ = phy->opts >> 8;
    *p++ = phy->opts & 0xff;
    *len = p - data;
    return PICOKEY_OK;
}
#include <stdio.h>
int phy_unserialize_data(const uint8_t *data, uint16_t len, phy_data_t *phy) {
    if (!phy || !data || !len) {
        return PICOKEY_ERR_NULL_PARAM;
    }
    memset(phy, 0, sizeof(phy_data_t));
    const uint8_t *p = data;
    while (p < data + len) {
        printf("TAG %x\n",*p);
        switch (*p++) {
            case PHY_VIDPID:
                memcpy(phy->vidpid, p, 4);
                phy->vidpid[1] = *p++;
                phy->vidpid[0] = *p++;
                phy->vidpid[3] = *p++;
                phy->vidpid[2] = *p++;
                phy->vidpid_present = true;
                break;
            case PHY_LED_GPIO:
                phy->led_gpio = *p++;
                phy->led_gpio_present = true;
                break;
            case PHY_LED_BTNESS:
                phy->led_brightness = *p++;
                phy->led_brightness_present = true;
                break;
            case PHY_OPTS:
                phy->opts = (*p << 8) | *(p + 1);
                p += 2;
                break;
        }
    }
    return PICOKEY_OK;
}

int phy_init() {
    memset(&phy_data, 0, sizeof(phy_data_t));
    if (file_has_data(ef_phy)) {
        const uint8_t *data = file_get_data(ef_phy);
        int ret = phy_unserialize_data(data, file_get_size(ef_phy), &phy_data);
        if (ret != PICOKEY_OK) {
            return ret;
        }
    }

    return PICOKEY_OK;
}
#endif
