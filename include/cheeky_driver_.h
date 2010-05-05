/*
  This file is part of cheeky_driver.

  cheeky_driver is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  cheeky_driver is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with cheeky_driver. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CHEEKY_DRIVER_H_
# define CHEEKY_DRIVER_H_

# include <asm/uaccess.h>

# include <linux/kthread.h>
# include <linux/kernel.h>
# include <linux/module.h>
# include <linux/errno.h>
# include <linux/sched.h>
# include <linux/slab.h>
# include <linux/init.h>
# include <linux/usb.h>
# include <linux/fs.h>

# include "cheeky_driver.h"

/*
 * defines
 */
/**
 * @brief
 *	The USB Vendor ID of the device.
 */
# define USB_VID		0x1D34

/**
 * @brief
 *	The USB Product ID of the device.
 */
# define USB_PID		0x0013
# define USB_MINOR_BASE		42

# define BRIGHNESS_MASK		(0x0003)
# define CUSTOM_MASK		(0x0004)
# define FLASH_MASK		(0x0008)
# define SPEED_MASK		(0x00f0)
# define HMOVE_MASK		(0x0300)
# define VMOVE_MASK		(0x0c00)
# define NEGATIVE_MASK		(0x1000)

# define NB_ROWS		7
# define NB_COLUMNS		21

# ifndef MAX_CHARS
/**
 * @brief
 *	The maximum size of the text buffer, this value may be overwritten at
 *	compile time to expand the value.
 */
#  define MAX_CHARS		64
# endif

/*
 * macros
 */
/**
 * @brief
 *	Store the bitfield in way expected by the driver.
 * @param Bf The bitfield to store.
 */
# define COMPLETE_BITFIELD(Bf)			\
	((Bf) << 11)

/**
 * @brief
 *	Returns the 3 bits corresponding to the row.
 * @param Number the row number.
 * @param Bf The bitfield to extract the 3 bits.
 */
# define ROW(Number, Bf)				\
	((Bf)					<<	\
	 ((Number) * 3)			>>		\
	 (29))

/**
 * @brief
 *	Set the 2 bits corresponding to the brighness in the bitfield Params.
 * @param Params The bitfield where to set the brighness
 * @param Value The value of brighness, should one of:
 *		- LED_LOW_BR | 0
 *		- LED_MIDDLE_BR | 1
 *		- LED_HIGH_BR | 2
 */
# define SET_BRIGHNESS(Params, Value)			\
	((Params) = ((Params) & ~BRIGHNESS_MASK) |	\
	 ((Value) & BRIGHNESS_MASK))

/**
 * @brief
 *	Set the custom bit, used when the user specify by itself the usb packets
 *	to send to the led display.
 * @param Params The bitfield where to set the custom bit.
 * @param Value The value of custom, should be 0 or 1.
 */
# define SET_CUSTOM(Params, Value)		\
	((Params) = ((Params) & ~CUSTOM_MASK) |	\
	 (((Value) << 2) & CUSTOM_MASK))

/**
 * @brief
 *	Set the flash bit, used to know if the user wants the text to flash.
 * @param Params The bitfield where to set the flash bit.
 * @param Value The value of flash, should be one of:
 *		- LED_NO_FLASH | 0
 *		- LED_FLASHING | 1
 */
# define SET_FLASH(Params, Value)		\
	((Params) = ((Params) & ~FLASH_MASK) |	\
	 (((Value) << 3) & FLASH_MASK))

/**
 * @brief
 *	Set the speed 4 bits value, used to specify the speed of the text
 *	moving.
 * @param Params The bitfield where to set the speed value
 * @param Value The speed value, should be between [0-15].
 */
# define SET_SPEED(Params, Value)		\
	((Params) = ((Params) & ~SPEED_MASK) |	\
	 (((Value) << 4) & SPEED_MASK))

/**
 * @brief
 *	Set the horizontal move value, used to know in which direction to move
 *	to.
 * @param Params The bitfield where to set the hmove value.
 * @param Value The hmove value, should be one of:
 *		- LED_NO_HMOVE | 0
 *		- LED_RIGHT_TO_LEFT | 1
 *		- LED_LEFT_TO_RIGHT | 0
 */
# define SET_HMOVE(Params, Value)		\
	((Params) = ((Params) & ~HMOVE_MASK) |	\
	 (((Value) << 8) & HMOVE_MASK))

/**
 * @brief
 *	Set the vertical move value, used to know in which direction to move
 *	to.
 * @param Params The bitfield where to set the hmove value.
 * @param Value the vmove value, should be one of:
 *		- LED_NO_VMOVE | 0
 *		- LED_UP_TO_DOWN | 1
 *		- LED_DOWN_TO_UP | 2
 */
# define SET_VMOVE(Params, Value)		\
	((Params) = ((Params) & ~VMOVE_MASK) |	\
	 (((Value) << 10) & VMOVE_MASK))

/**
 * @brief
 *	Set the negative bit value, used to know if the driver should reverse
 *	the led (on -> off && off -> on).
 * @param Params The bitfield where to set the negative bit.
 * @param Value The negative value, should be one of:
 ⎋ *		- LED_NEGATIVE_OFF | 0
 *		- LED_NEGATIVE_ON | 1
 */
# define SET_NEGATIVE(Params, Value)			\
	((Params) = ((Params) & ~NEGATIVE_MASK) |	\
	 (((Value) << 12) & NEGATIVE_MASK))

/**
 * @brief
 *	Extract the brighness value from the bitfield Params.
 * @param Params The bitfield to extract the brighness value from
 */
# define GET_BRIGHNESS(Params)			\
	((Params) & BRIGHNESS_MASK)

/**
 * @brief
 *	Extract the custom value from the bitfield Params.
 * @param Params The bitfield to extract the custom value from
 */
# define GET_CUSTOM(Params)			\
	(((Params) & CUSTOM_MASK) >> 2)

/**
 * @brief
 *	Extract the flash value from the bitfield Params.
 * @param Params The bitfield to extract the flash value from
 */
# define GET_FLASH(Params)			\
	(((Params) & FLASH_MASK) >> 3)

/**
 * @brief
 *	Extract the speed value from the bitfield Params.
 * @param Params The bitfield to extract the speed value from
 */
# define GET_SPEED(Params)			\
	(((Params) & SPEED_MASK) >> 4)

/**
 * @brief
 *	Extract the hmove value from the bitfield Params.
 * @param Params The bitfield to extract the hmove value from
 */
# define GET_HMOVE(Params)			\
	(((Params) & HMOVE_MASK) >> 8)

/**
 * @brief
 *	Extract the vmove value from the bitfield Params.
 * @param Params The bitfield to extract the vmove value from
 */
# define GET_VMOVE(Params)			\
	(((Params) & VMOVE_MASK) >> 10)

/**
 * @brief
 *	Extract the negative value from the bitfield Params.
 * @param Params The bitfield to extract the negative value from
 */
# define GET_NEGATIVE(Params)			\
	(((Params) & NEGATIVE_MASK) >> 12)

/**
 * @brief
 *	Represents a usb packet in the form expected by the led display.
 */
typedef struct usb_packet_t {
	__u8 brighness;
	/*!<
	 * The message brigness,  range from 0 to 2 (2 is the max.).
	 */
	__u8 row_number;
	/*!<
	 * The first row number which is being set (0, 2, 4 or 6).
	 */
	unsigned int first_row:24;
	/*!<
	 * Bitfields which represent the firs row of 21 LED (the last 3 bits are set
	 * to 1) bigendian.
	 */
	unsigned int second_row:24;
	/*!<
	 * Same as first_row but for the second row.
	 */
} __attribute__ ((packed))	usb_packet_t;

/**
 * @brief
 *	Represents the driver internally data that are used to
 *	display a text on the led display.
 */
typedef struct data_t {
	struct usb_interface* interface;
	/*!<
	 * The usb interface for the device.
	 */
	struct task_struct* kthread;
	/*!<
	 * A pointer to the thread which refresh the led display.
	 */
	struct usb_device* udev;
	/*!<
	 * The usb device registered with this driver.
	 */
	struct semaphore sem_buffer;
	/*!<
	 * A semaphore used during the write from a user, used not to mess the text.
	 */
	usb_packet_t* display_packets;
	/*!<
	 * The representation of the 8bytes message we send to the usb device.
	 */
	char* buffer;
	/*!<
	 * Contains the text message to be written on the display.  No more than
	 * MAX_CHARS are supported, you may change it during the compilation.
	 */
	__u16 params;
	/*!<
	 * A bitfield of all parameters associated with the device.
	 */
	__u8 length;
	/*!<
	 * The length if the text kept in the buffer.
	 */
	__u8 start_character;
	/*!<
	 * Keep the index of the first character printed on the display.
	 */
} data_t;

/**
 * @brief
 *	This structure is used to keep a correspondance between a character
 *	(letter) and the data (bitfield) we need to send to the led display.
 */
typedef struct character_map_t {
	char letter;
	/*!<
	 * The character to print.
	 */
	unsigned int bitfield;
	/*!<
	 * The bitfield representing the character.
	 */
} character_map_t;

#endif /* !CHEEKY_DRIVER_H_ */
