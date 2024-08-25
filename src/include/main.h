#ifndef MAIN_H_
#define MAIN_H_

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <soc.h>
#include <assert.h>
#include <zephyr/spinlock.h>

#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include <zephyr/bluetooth/services/bas.h>
#include <bluetooth/services/hids.h>
#include <zephyr/bluetooth/services/dis.h>
#include <dk_buttons_and_leds.h>

#include <zephyr/input/input_kbd_matrix.h>

#include "config/define_configuration.h"

// DATA STRUCTURES

/* HIDS instance. */
BT_HIDS_DEF(hids_obj,
	    OUTPUT_REPORT_MAX_LEN,
	    INPUT_REPORT_KEYS_MAX_LEN);

static volatile bool is_adv;

static const struct bt_data ad[] = {
    // Declear this device is an HID-Keyboard device
	BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE,
		      (CONFIG_BT_DEVICE_APPEARANCE >> 0) & 0xff,
		      (CONFIG_BT_DEVICE_APPEARANCE >> 8) & 0xff),
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_HIDS_VAL),
					  BT_UUID_16_ENCODE(BT_UUID_BAS_VAL)),
};

static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static struct conn_mode {
	struct bt_conn *conn;
	bool in_boot_mode;
} conn_mode[CONFIG_BT_HIDS_MAX_CLIENT_COUNT];

static const uint8_t hello_world_str[] = {
	0x0b,	/* Key h */
	0x08,	/* Key e */
	0x0f,	/* Key l */
	0x0f,	/* Key l */
	0x12,	/* Key o */
	0x28,	/* Key Return */
};

static const uint8_t shift_key[] = { 225 };

/* Current report status
 */
static struct keyboard_state {
	uint8_t ctrl_keys_state; /* Current keys state */
	uint8_t keys_state[KEY_PRESS_MAX];
} hid_keyboard_state;

#if CONFIG_NFC_OOB_PAIRING
static struct k_work adv_work;
#endif

static struct k_work pairing_work;
struct pairing_data_mitm {
	struct bt_conn *conn;
	unsigned int passkey;
};

K_MSGQ_DEFINE(mitm_queue,
	      sizeof(struct pairing_data_mitm),
	      CONFIG_BT_HIDS_MAX_CLIENT_COUNT,
	      4);

#endif