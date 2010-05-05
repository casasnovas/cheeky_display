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

#include <fcntl.h>
#include <stropts.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "cheeky_driver.h"

static struct option long_options[] = {
	{"brighness", required_argument, 0, 'b'},
	{"speed", required_argument, 0, 's'},
	{"horizontal_move", required_argument, 0, 'm'},
	{"vertical_move", required_argument, 0, 'v'},
	{"flashing", required_argument, 0, 'f'},
	{"text", required_argument, 0, 't'},
	{"negative", required_argument, 0, 'n'},
	{"help", 0, 0, 'h'},
	{0, 0, 0, 0}
};

/**
 * @brief
 *	Prints a résumé of all options supported.
 */
static void	usage(void)
{
	printf("Usage: cheeky_control [option value]*\n"
	       "Here is a list of all options:\n"
	       "\t--brighness/-b: LED_LOW_BR (or 0), LED_MIDDLE_BR (or 1), LED_HIGH_BR (or 2)\n"
	       "\t--speed/-s: A number between 0 and 15\n"
	       "\t--horizontal_move/-m: LED_NO_HMOVE (or 0), LED_RIGHT_TO_LEFT (or 1), LED_LEFT_TO_RIGHT (or 2)\n"
	       "\t--vertical_move/-v: LED_NO_VMOVE (or 0), LED_UP_TO_DOWN (or 1), LED_DOWN_TO_UP (or 2)\n"
	       "\t--flash/-f: LED_NO_FLASH (or 0), LED_FLASHING (or 1)\n"
	       "\t--negative/-n: LED_NOEGATIVE_OFF (or 0), LED_NEGATIVE_ON (or 1)\n"
	       "\t--help/-h: Print this message\n");
}

/**
 * @brief
 *	Check if the string is a numeric string.
 * @param string The string to check if it's numeric.
 * @return 1 if the string is composed of digits, 0 if not.
 */
static int	is_numeric(char*	string)
{
	while (*string) {
		if (*string < '0' ||
		    *string > '9')
			return (0);
		++string;
	}
	return (1);
}

/**
 * @brief
 *	Change the brighness of the led display.
 * @param arg The new level of brighness, could be:
 *		- LED_LOW_BR or 0
 *		- LED_MIDDLE_BR or 1
 *		- LED_HIGH_BR or 2
 * @param cheeky_device A file descriptor to the cheeky device.
 * @return 0 on success, -1 on error.
 */
static int	set_brighness(char*	arg,
			      int	cheeky_device)
{
	if (is_numeric(arg))
		ioctl(cheeky_device,
		      IOCTL_CMD_BRIGHNESS,
		      atoi(arg));
	else if (strcmp(arg, "LED_LOW_BR") == 0)
		ioctl(cheeky_device,
		      IOCTL_CMD_BRIGHNESS,
		      LED_LOW_BR);
	else if (strcmp(arg, "LED_MIDDLE_BR") == 0)
		ioctl(cheeky_device,
		      IOCTL_CMD_BRIGHNESS,
		      LED_MIDDLE_BR);
	else if (strcmp(arg, "LED_HIGH_BR") == 0)
		ioctl(cheeky_device,
		      IOCTL_CMD_BRIGHNESS,
		      LED_HIGH_BR);
	else {
		printf("cheeky_display: Wrong argument to --brighness!\n");
		usage();
		return (-1);
	}
	return (0);
}

/**
 * @brief
 *	Change the flash [on/off] of the led display.
 * @param arg The new value for the flash option, could be:
 *		- LED_NO_FLASH or 0
 *		- LED_FLASHING or 1
 * @param cheeky_device A file descriptor to te led device.
 * @return 0 on success, -1 on error.
 */
static int	set_flashing(char*	arg,
			     int	cheeky_device)
{
	if (is_numeric(arg))
		ioctl(cheeky_device,
		      IOCTL_CMD_FLASH,
		      atoi(arg));
	else if (strcmp(arg, "LED_FLASHING") == 0)
		ioctl(cheeky_device,
		      IOCTL_CMD_FLASH,
		      LED_FLASHING);
	else if (strcmp(arg, "LED_NO_FLASH") == 0)
		ioctl(cheeky_device,
		      IOCTL_CMD_FLASH,
		      LED_NO_FLASH);
	else {
		printf("cheeky_display: Wrong argument to --flash!\n");
		usage();
		return (-1);
	}
	return (0);
}

/**
 * @brief
 *	Change the negative [on/off] of the led display.
 * @param arg The new negative value for the negative option, could be:
 *		- LED_NEGATIVE_OFF or 0
 *		- LED_NEGATIVE_ON or 1
 * @param cheeky_device A file descriptor to the led device.
 * @return 0 on success, -1 on error.
 */
static int	set_negative(char*	arg,
			     int	cheeky_device)
{
	if (is_numeric(arg))
		ioctl(cheeky_device,
		      IOCTL_CMD_NEGATIVE,
		      atoi(arg));
	else if (strcmp(arg, "LED_NEGATIVE_ON") == 0)
		ioctl(cheeky_device,
		      IOCTL_CMD_NEGATIVE,
		      LED_NEGATIVE_ON);
	else if (strcmp(arg, "LED_NEGATIVE_OFF") == 0)
		ioctl(cheeky_device,
		      IOCTL_CMD_NEGATIVE,
		      LED_NEGATIVE_OFF);
	else {
		printf("cheeky_display: Wrong argument to --negative!\n");
		usage();
		return (-1);
	}
	return (0);
}

/**
 * @brief
 *	Change the speed value of the led display.
 * @param arg The new speed value, should be a number between 0 and 15.
 * @param cheeky_device A file descriptor to the led device.
 * @return 0 on success, -1 on error.
 */
static int	set_speed(char*	arg,
			  int	cheeky_device)
{
	if (is_numeric(arg))
		ioctl(cheeky_device,
		      IOCTL_CMD_SPEED,
		      atoi(arg));
	else {
		printf("cheeky_display: Wrong argument to --speed!\n");
		usage();
		return (-1);
	}
	return (0);
}

/**
 * @brief
 *	Change the text displayed on the screen.
 * @param arg The new text to display.
 * @param cheeky_device A file descriptor to the led device.
 * @return 0 on success, -1 on error.
 */
static int	set_text(char*	arg,
			 int	cheeky_device)
{
	if (write(cheeky_device, arg, strlen(arg)) == -1) {
		printf("cheeky_display; Cannot write to device /dev/cheeky0. Is the display plugged ? "
		       "Is you user a member of the group cheeky ?\n");
		return (-1);
	}
	return (0);
}

/**
 * @brief
 *	Change the horizontal move value.
 * @param arg The new value of the hmove option, could be:
 *		- LED_NO_HMOVE or 0
 *		- LED_RIGHT_TO_LEFT or 1
 *		- LED_LEFT_TO_RIGHT or 2
 * @param cheeky_device A file descriptor to the led device.
 * @return 0 on success, -1 on error.
 */
static int	set_hmove(char*		arg,
			  int		cheeky_device)
{
	if (is_numeric(arg))
		ioctl(cheeky_device,
		      IOCTL_CMD_HMOVE,
		      atoi(arg));
	else if (strcmp(arg, "LED_RIGHT_TO_LEFT") == 0)
		ioctl(cheeky_device,
		      IOCTL_CMD_HMOVE,
		      LED_RIGHT_TO_LEFT);
	else if (strcmp(arg, "LED_LEFT_TO_RIGHT") == 0)
		ioctl(cheeky_device,
		      IOCTL_CMD_HMOVE,
		      LED_LEFT_TO_RIGHT);
	else if (strcmp(arg, "LED_NO_HMOVE") == 0)
		ioctl(cheeky_device,
		      IOCTL_CMD_HMOVE,
		      LED_NO_HMOVE);
	else {
		printf("cheeky_display: Wrong argument to --hmove!\n");
		usage();
		return (-1);
	}
	return (0);
}

/**
 * @brief
 *	Change the vertical move value of the led display.
 * @param arg The new value of the vertical move, could be:
 *		- LED_NO_VMOVE or 0
 *		- LED_UP_TO_DOWN or 1
 *		- LED_DOWN_TO_UP or 2
 * @param cheeky_device A file descriptor to the led device.
 * @return 0 on success, -1 on error.
 */
static int	set_vmove(char*		arg,
			  int		cheeky_device)
{
	if (is_numeric(arg))
		ioctl(cheeky_device,
		      IOCTL_CMD_VMOVE,
		      atoi(arg));
	else if (strcmp(arg, "LED_UP_TO_DOWN") == 0)
		ioctl(cheeky_device,
		      IOCTL_CMD_VMOVE,
		      LED_UP_TO_DOWN);
	else if (strcmp(arg, "LED_DOWN_TO_UP") == 0)
		ioctl(cheeky_device,
		      IOCTL_CMD_VMOVE,
		      LED_DOWN_TO_UP);
	else if (strcmp(arg, "LED_NO_VMOVE") == 0)
		ioctl(cheeky_device,
		      IOCTL_CMD_VMOVE,
		      LED_NO_VMOVE);
	else {
		printf("cheeky_display: Wrong argument to --vmove!\n");
		usage();
		return (-1);
	}
	return (0);
}

/**
 * @brief
 *	 Parses all options given to the programm and call the appropritates
 *	 functions.
 * @param argc The number of arguments.
 * @param argv The array of arguments.
 * @return 0 on success, 1 on error.
 */
int		main(int	argc,
		     char**	argv)
{
	int		cheeky_device;
	int		option_index = 0;
	int		c = 0;


	if (argc < 2) {
		usage();
		return (0);
	}

	cheeky_device = open("/dev/cheeky0", O_RDWR);
	if (cheeky_device == -1) {
		printf("cheeky_device: Unable to open the file /dev/cheeky0. Is your user a member of the cheeky group ?\n");
		return (-1);
	}

	while (1) {
		c = getopt_long(argc,
				argv,
				"b:f:m:n:s:t:v:h",
				long_options,
				&option_index);

		if (c == -1)
			break;

		switch (c) {
		case 'b':
			if (set_brighness(optarg, cheeky_device) == -1)
				return (-1);
			break;
		case 'f':
			if (set_flashing(optarg, cheeky_device) == -1)
				return (-1);
			break;
		case 'm':
			if (set_hmove(optarg, cheeky_device) == -1)
				return (-1);
			break;
		case 'n':
			if (set_negative(optarg, cheeky_device) == -1)
				return (-1);
			break;
		case 's':
			if (set_speed(optarg, cheeky_device) == -1)
				return (-1);
			break;
		case 't':
			if (set_text(optarg, cheeky_device) == -1)
				return (-1);
			break;
		case 'v':
			if (set_vmove(optarg, cheeky_device) == -1)
				return (-1);
			break;
		case 'h':
			usage();
			return (0);
			break;
		default:
			usage();
			return (-1);
			break;
		}
	}

	close(cheeky_device);

	return (0);
}
