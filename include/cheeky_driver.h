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

#ifndef CHEEKY_DISPLAY_H_
# define CHEEKY_DISPLAY_H_

# define IOCTL_CMD_BRIGHNESS	(1 << 1)
# define IOCTL_CMD_SPEED	(1 << 2)
# define IOCTL_CMD_HMOVE	(1 << 3)
# define IOCTL_CMD_VMOVE	(1 << 4)
# define IOCTL_CMD_FLASH	(1 << 5)
# define IOCTL_CMD_NEGATIVE	(1 << 6)
# define IOCTL_CMD_CUSTOM	(1 << 7)

# define LED_NO_VMOVE		0
# define LED_UP_TO_DOWN		1
# define LED_DOWN_TO_UP		2
# define LED_NO_HMOVE		0
# define LED_RIGHT_TO_LEFT	1
# define LED_LEFT_TO_RIGHT	2
# define LED_NO_FLASH		0
# define LED_FLASHING		1
# define LED_LOW_BR		0
# define LED_MIDDLE_BR		1
# define LED_HIGH_BR		2
# define LED_NEGATIVE_OFF	0
# define LED_NEGATIVE_ON	1

#endif /* !CHEEKY_DISPLAY_H_ */
