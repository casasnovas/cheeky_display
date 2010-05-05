/*
  (c) 2009 Quentin Casasnovas

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

#include "cheeky_driver_.h"

MODULE_DESCRIPTION("USB led display driver");
MODULE_AUTHOR("Quentin Casasnovas");
MODULE_LICENSE("GPL");

static struct usb_device_id cheeky_id_table[] __devinitdata = {
	{USB_DEVICE(USB_VID, USB_PID)},
	{}
};

MODULE_DEVICE_TABLE(usb, cheeky_id_table);

static const character_map_t 	character_map[] = {
	{'A', COMPLETE_BITFIELD(0b111101101111101101101)},
	{'B', COMPLETE_BITFIELD(0b011101101011101101011)},
	{'C', COMPLETE_BITFIELD(0b110001001001001001110)},
	{'D', COMPLETE_BITFIELD(0b011111101101101111011)},
	{'E', COMPLETE_BITFIELD(0b111001001011001001111)},
	{'F', COMPLETE_BITFIELD(0b111001001011001001001)},
	{'G', COMPLETE_BITFIELD(0b110001001111101101110)},
	{'H', COMPLETE_BITFIELD(0b101101101111101101101)},
	{'I', COMPLETE_BITFIELD(0b010000010010010010010)},
	{'J', COMPLETE_BITFIELD(0b110100100101101101010)},
	{'K', COMPLETE_BITFIELD(0b101101111001011101101)},
	{'L', COMPLETE_BITFIELD(0b001001001001001001111)},
	{'M', COMPLETE_BITFIELD(0b101111111101101101101)},
	{'N', COMPLETE_BITFIELD(0b101101111111101101101)},
	{'O', COMPLETE_BITFIELD(0b010101101101101101010)},
	{'P', COMPLETE_BITFIELD(0b011101101011001001001)},
	{'Q', COMPLETE_BITFIELD(0b010101101101101111110)},
	{'R', COMPLETE_BITFIELD(0b011101101011001011101)},
	{'S', COMPLETE_BITFIELD(0b010101001010100101010)},
	{'T', COMPLETE_BITFIELD(0b111010010010010010010)},
	{'U', COMPLETE_BITFIELD(0b101101101101101101010)},
	{'V', COMPLETE_BITFIELD(0b101101101101101010010)},
	{'W', COMPLETE_BITFIELD(0b101101101111111101010)},
	{'X', COMPLETE_BITFIELD(0b101101101010101101101)},
	{'Y', COMPLETE_BITFIELD(0b101101101010010010010)},
	{'Z', COMPLETE_BITFIELD(0b111100010010010001111)},
	{'0', COMPLETE_BITFIELD(0b010101101101101101010)},
	{'1', COMPLETE_BITFIELD(0b010011010010010010111)},
	{'2', COMPLETE_BITFIELD(0b010101101100010011111)},
	{'3', COMPLETE_BITFIELD(0b010101100010100100111)},
	{'4', COMPLETE_BITFIELD(0b001001001101111100100)},
	{'5', COMPLETE_BITFIELD(0b111001001011100100011)},
	{'6', COMPLETE_BITFIELD(0b110001001011101101010)},
	{'7', COMPLETE_BITFIELD(0b111100100010010001001)},
	{'8', COMPLETE_BITFIELD(0b010101101010101101010)},
	{'9', COMPLETE_BITFIELD(0b010101101110100101010)},
	{'-', COMPLETE_BITFIELD(0b000000000111000000000)},
	{'_', COMPLETE_BITFIELD(0b000000000000000000111)},
	{'(', COMPLETE_BITFIELD(0b100010010001010010100)},
	{')', COMPLETE_BITFIELD(0b001010010100010010001)},
	{'\\', COMPLETE_BITFIELD(0b001001010010010100100)},
	{'/', COMPLETE_BITFIELD(0b100100010010010001001)},
	{'|', COMPLETE_BITFIELD(0b010010010010010010010)},
	{'\'', COMPLETE_BITFIELD(0b010010000000000000000)},
	{'<', COMPLETE_BITFIELD(0b100010001001001010100)},
	{'>', COMPLETE_BITFIELD(0b001010100100100010001)},
	{'!', COMPLETE_BITFIELD(0b010010010010010000010)},
	{'?', COMPLETE_BITFIELD(0b010101101100010000010)},
	{'.', COMPLETE_BITFIELD(0b000000000000000000001)},
	{',', COMPLETE_BITFIELD(0b000000000000000100010)},
	{';', COMPLETE_BITFIELD(0b000000010000010001000)},
	{':', COMPLETE_BITFIELD(0b000000010000010000000)},
	{'^', COMPLETE_BITFIELD(0b010101000000000000000)},
	{'=', COMPLETE_BITFIELD(0b000000111000111000000)},
	{'+', COMPLETE_BITFIELD(0b000000010111010000000)},
	{'"', COMPLETE_BITFIELD(0b110011000000000000000)},
	{' ', COMPLETE_BITFIELD(0b000000000000000000000)},
	{'\0',COMPLETE_BITFIELD(0b000000000000000000000)}
};

static struct usb_driver cheeky_driver;

/**
 * @brief
 *	Returns the bitfield associated with the character c.
 * @param c The letter we're searching the bitfield for.
 * @return The bitfield corresponding to the letter c.
 */
static unsigned int		cheeky_get_bitfield(char		c)
{
	const character_map_t*	chars_map = character_map;

	while (chars_map->letter != '\0'	&&
	       chars_map->letter != c) {
		if (c >= 'a'	&&
		    c <= 'z'	&&
		    c - 32 == chars_map->letter)
			break;
		else
			++chars_map;
	}

	return (chars_map->bitfield);
}

/**
 * @brief
 *	Refresh rows row_number AND (row_number + 1) in the usb packet
 *	that we'll send to the led device. This function is also in charge
 *	to make the horizontal move and the negative display.
 * @param data Our private structure where all params are located.
 * @param row_number The first row this function is updating.
 * @param decale The number of LED, between 0 and 2, to shift.
 */
static void		cheeky_refresh_row(data_t*		data,
					__u8		row_number,
					__u8		decale)
{
	unsigned int		bitfield;
	char			char_to_print;
	__be32		first_row = 0;
	__be32		second_row = 0;
	__u8			i;
	__s8			hmove;

	data->display_packets[row_number].brighness = GET_BRIGHNESS(data->params);
	data->display_packets[row_number].row_number = row_number * 2;

	/* Updating the usb packet depending on the text buffer */
	for (i = 0; i < 8; ++i)	{
		up(&data->sem_buffer);
		char_to_print =
			data->buffer[(i + data->start_character) % data->length];
		down(&data->sem_buffer);

		bitfield = cheeky_get_bitfield(char_to_print);

		first_row |= ROW(row_number * 2, bitfield) << (3 * (i + 1));
		second_row |= ROW(row_number * 2 + 1, bitfield) << (3 * (i + 1));
	}

	/* If there is a horizontal move, just shift the bits :) */
	hmove = GET_HMOVE(data->params);
	if (hmove & LED_RIGHT_TO_LEFT) {
		first_row >>= decale;
		second_row >>= decale;
	}
	else if (hmove & LED_LEFT_TO_RIGHT) {
		up(&data->sem_buffer);
		char_to_print =
			data->buffer[(i - 2 + data->start_character) % data->length];
		down(&data->sem_buffer);

		bitfield = cheeky_get_bitfield(char_to_print);
		first_row |= ROW(row_number * 2, bitfield);
		second_row |= ROW(row_number * 2 + 1, bitfield);

		first_row <<= decale;
		second_row <<= decale;
	}

	/*
	 * Reverse the byte order of the usb packet, the led device is excepting
	 * bigendian bytesx
	 */
	if (GET_NEGATIVE(data->params))	{
		data->display_packets[row_number].first_row =
			cpu_to_be32((first_row << 5));
		data->display_packets[row_number].second_row =
			cpu_to_be32((second_row << 5));
	}
	else {
		data->display_packets[row_number].first_row =
			cpu_to_be32(~(first_row << 5));
		data->display_packets[row_number].second_row =
			cpu_to_be32(~(second_row << 5));
	}
}

/**
 * @brief
 *	This is a helper function used to update all parameters
 *	at each loop turn. Sexyer than having this code in the loop...
 * @param hdecale The number of horizontal LED to shift (if hmove is activated).
 * @param vdecale The number of vertical LED to shift (if vmove is activated).
 * @param flash A on/off switch.
 * @param data Our private data.
 */
static void		cheeky_update_params(__u8*		hdecale,
					  __u8*		vdecale,
					  __u8*		flash,
					  data_t	*data)
{
	__s8			hmove;
	__s8			vmove;
	__u8			speed;

	hmove = GET_HMOVE(data->params);
	vmove = GET_VMOVE(data->params);
	speed = GET_SPEED(data->params);

	if (hmove) {
		if (*hdecale == 2) {
			*hdecale = 0;
			if (hmove & LED_RIGHT_TO_LEFT)
				++(data->start_character);
			else
				--(data->start_character);
			if (data->start_character == data->length)
				data->start_character = 0;
			else if (data->start_character == 0)
				data->start_character = data->length - 1;
		}
		else
			++(*hdecale);
	}
	if (vmove) {
		if (*vdecale == 6)
			*vdecale = 0;
		else
			++(*vdecale);
	}

	/* Turn of the LED if we flash this turn */
	if (GET_FLASH(data->params))
		*flash = !(*flash);

	/* Release the CPU until time has expired */
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(HZ / (4 + 2 * abs(speed)));
}

/**
 * @brief
 *	Makes a vertical switch with the rows if choosen by the user.
 * @param data Our private structure
 * @param vdecale Number of line to shift.
 */
static void		cheeky_vertical_move(data_t*	data,
					  __u8		vdecale)
{
	unsigned int		tmp_row;
	__s8			direction = 0;
	__u8			i = 0;
	__u8			j = 0;

	direction = GET_VMOVE(data->params);

	if (direction & LED_DOWN_TO_UP)	{
		for (j = 0; j < vdecale; ++j) {
			tmp_row = data->display_packets[0].first_row;
			for (i = 0; i < 3; ++i)	{
				data->display_packets[i].first_row =
					data->display_packets[i].second_row;
				data->display_packets[i].second_row =
					data->display_packets[i + 1].first_row;
			}
			data->display_packets[i].first_row =
				data->display_packets[i].second_row;
			data->display_packets[i].second_row = tmp_row;
		}
	}
	else if (direction & LED_UP_TO_DOWN) {
		for (j = 0; j < vdecale; ++j) {
			tmp_row = data->display_packets[3].second_row;
			for (i = 3; i > 0; --i)	{
				data->display_packets[i].second_row =
					data->display_packets[i].first_row;
				data->display_packets[i].first_row =
					data->display_packets[i - 1].second_row;
			}
			data->display_packets[i].second_row =
				data->display_packets[i].first_row;
			data->display_packets[i].first_row = tmp_row;
		}
	}
}

/**
 * @brief
 *	This function runs into a separate thread than usuals functions (open,
 *	read, write, etc...) and refresh all led rows from the the text the
 *	user has entered (or from the bitfield he gave thanks to ioctl
 *	function). A semaphore is used not to print altered text due to
 *	concurent writting in the buffer by different threads.
 * @param vdata Private data associated with the usb device.
 * @return Always 0.
 */
static int		cheeky_refresh(void*	vdata)
{
	data_t*		data = vdata;
	__u8			hdecale = 0;
	__u8			vdecale = 0;
	__u8			flash = 0;
	__u8			i = 0;

	while (!kthread_should_stop()) {
		/* Clear the display when flashing		*/
		if (GET_FLASH(data->params) && flash)
			for (i = 0; i < 4; ++i)	{
				data->display_packets[i].first_row = ~0;
				data->display_packets[i].second_row = ~0;
			}
		/* Update all 8 rows depending on the text buffer */
		else if (!GET_CUSTOM(data->params))
			for (i = 0; i < 4; ++i)
				cheeky_refresh_row(data, i, hdecale);
		cheeky_vertical_move(data, vdecale);
		/* Send the 4 packets to the device		*/
		for (i = 0; i < 4; ++i)
			usb_control_msg(data->udev,
					usb_sndctrlpipe(data->udev, 0),
					0x09,		/* Reverse engeenered it under windows using usbsnoop	*/
					0x22,		/* Idem...						*/
					0x02,		/* Idem...						*/
					0,
					&(data->display_packets[i]),
					sizeof(usb_packet_t),
					HZ / 4);
		/* Update all parameters and wait			*/
		cheeky_update_params(&hdecale,
				  &vdecale,
				  &flash,
				  data);
	}

	return (0);
}

/**
 * @brief
 *	(Not implemented yet). This function will read the text (if any) that
 *	is displayed on the led display, and copy it to buf.
 * @param file Used to retreive our priavte data.
 * @param buf Buffer where we would copy the text printed on the cheeky display.
 * @param count The maximum size of buf.
 * @param pos An offset to copy to from the buf address.
 * @return The number of character red.
 */
static ssize_t		cheeky_read(struct file*	file,
				 char*		buf,
				 size_t		count,
				 loff_t*	pos)
{
	return (0);
}

/**
 * @brief
 *	Changes the text to be displayed ont the led display by the ascii
 *	string in buf (not all ascii characters are supported yet).
 * @param file Used to retreive our private data.
 * @param buf A pointer to the text that will be printed on the led display.
 * @param count The number of character to be displayed on the led display. If
 * this number is greater than MAX_CHARS, the text will be truncated.
 * @param ppos An offset to copy from buf.
 * @return The number of character written.
 */
static ssize_t		cheeky_write(struct file*	file,
				  const char*	buf,
				  size_t	count,
				  loff_t*	ppos)
{
	__u8			i;
	size_t		real;
	data_t*		data;

	data = file->private_data;
	if (!data) {
		printk(KERN_WARNING "cheeky_display: Cannot find private data.\n");
		return (-ENODEV);
	}

	/* Copying buffer from user */
	real = min((size_t) MAX_CHARS, count);
	up(&data->sem_buffer);
	if (copy_from_user(data->buffer, buf, real)) {
		printk(KERN_WARNING "cheeky_display: Cannot copy from user.\n");
		return (-EFAULT);
	}
	/* If the buffer is lower than 7 bytes, we fill it with whitespaces */
	for (i = real; i < 7; ++i)
		data->buffer[i] = ' ';

	data->start_character = 0;
	data->length = max((size_t) 7, real);
	down(&data->sem_buffer);

	SET_CUSTOM(data->params, 0);

	return (real);
}

/**
 * @brief
 *	Extends features of this driver. Here are the 7 comands that are
 *	supported yet:
 *	- IOCTL_CMD_BRIGHNESS
 *	- IOCTL_CMD_SPEED
 *	- IOCTL_CMD_HMOVE
 *	- IOCTL_CMD_VMOVE
 *	- IOCTL_CMD_NEGATIVE
 *	- IOCTL_CMD_CUSTOM
 * @param inode Used to retreive the minor for this device.
 * @param file
 * @param cmd One of the seven comands above.
 * @param arg The parameter for each comand, authorized values, depending on
 * the cmd argument are :
 *	- cmd = IOCTL_CMD_BRIGHNESS: should be one of LED_LOW_BR, LED_MIDDLE_BR
 *	or LED_HIGH_BR
 *	- cmd = IOCTL_CMD_SPEED: should be a number between 0 and 15, 15 is the
 *	fastest.
 *	- cmd = IOCTL_CMD_HMOVE: should be one of LED_NO_HMOVE,
 *	LED_RIGHT_TO_LEFT or LED_LEFT_TO_RIGHT
 *	- cmd = IOCTL_CMD_VMOVE: should be one of LED_NO_VMOVE, LED_UP_TO_DOWN
 *	or LED_DOWN_TO_UP.
 *	- cmd = IOCTL_CMD_FLASH: should be one of LED_FLASHING or LED_NO_FLASH.
 *	- cmd = IOCTL_CMD_NEGATIVE: should be one of LED_NEGATIVE_ON or
 *	LED_NEGATIVE_OFF.
 *	- cmd = IOCTL_CMD_CUSTOM: arg is a pointer to a 32bytes memory area
 *	corresponsding to the 4 usb packets that will be sent to the
 *	device. This is used to give the ability to a user to write whatever he
 *	wants to the device and not juste ascii text.
 * @return 0 on success, a negative number on failure.
 */
static int		cheeky_ioctl(struct inode*	inode,
				  struct file*	file,
				  unsigned int	cmd,
				  unsigned long	arg)
{
	struct usb_interface*	interface;
	data_t*		data;
	int			minor;

	minor = MINOR(inode->i_rdev);
	interface = usb_find_interface(&cheeky_driver, minor);
	if (!interface)	{
		printk(KERN_WARNING "cheeky_display: cannot find device for minor.\n");
		return (-ENODEV);
	}

	data = usb_get_intfdata(interface);
	if (!data)
		return (-ENODEV);

	switch (cmd) {
	case IOCTL_CMD_BRIGHNESS:
		SET_BRIGHNESS(data->params, arg);
		break;
	case IOCTL_CMD_CUSTOM:
		SET_CUSTOM(data->params, 1);
		if (copy_from_user(data->display_packets, (void*) arg,
				   sizeof(usb_packet_t) * 4) != 0)
			SET_CUSTOM(data->params, 0);
		break;
	case IOCTL_CMD_FLASH:
		SET_FLASH(data->params, arg);
		break;
	case IOCTL_CMD_SPEED:
		SET_SPEED(data->params, arg);
		break;
	case IOCTL_CMD_HMOVE:
		SET_HMOVE(data->params, arg);
		break;
	case IOCTL_CMD_VMOVE:
		SET_VMOVE(data->params, arg);
		break;
	case IOCTL_CMD_NEGATIVE:
		SET_NEGATIVE(data->params, arg);
		break;
	default:
		printk(KERN_WARNING "cheeky_display: 0x%x unsupported ioctl command.\n",
		       cmd);
		return (-EINVAL);
		break;
	}

	return (0);
}

/**
 * @brief
 *	This function registers privates datas to the file
 *	structure, as to be able to use those datas in
 *	read/write/ioctl functions.
 * @param inode Used to retreive the minor.
 * @param file Used to attach private data to the char device.
 * @return 0 on success, a negative number on failure.
 */
static int		cheeky_open(struct inode*	inode,
				 struct file*	file)
{
	struct usb_interface*	interface;
	data_t*		data;
	int			minor;

	/* Minor retreiving */
	minor = MINOR(inode->i_rdev);
	interface = usb_find_interface(&cheeky_driver, minor);
	if (!interface)	{
		printk(KERN_WARNING "cheeky_display: cannot find device for minor.\n");
		return (-ENODEV);
	}

	/* Attaching our data to the usb device */
	data = usb_get_intfdata(interface);
	if (!data)
		return (-ENODEV);

	file->private_data = data;

	return (0);
}

/**
 * @brief
 *	This function releases the memory attached to the file descriptor.
 * @return Always 0.
 */
static int		cheeky_release(struct inode*	inode,
				    struct file*	file)
{
	file->private_data = NULL;

	return (0);
}

/**
 * @brief
 *	This structure tells the kernel which function we register with the
 *	char device.
 */
static struct file_operations	cheeky_fops = {
	.owner	= THIS_MODULE,
	.open	= cheeky_open,
	.release	= cheeky_release,
	.read	= cheeky_read,
	.write	= cheeky_write,
	.ioctl	= cheeky_ioctl,
};

/**
 * @brief
 *	This structure tells the kernel which char device we will use for this
 *	driver.
 */
static struct usb_class_driver	cheeky_class = {
	.name	= "cheeky%d",
	.fops	= &cheeky_fops,
	.minor_base	= USB_MINOR_BASE,
};

/**
 * @brief
 *	This function is called when we plug a led display. We allocate
 *	all private data we need and discover the endpoint which will be
 *	used to communicate with the device.
 * @param interface
 * @param entity
 * @return 0 on success, a negative number on failure.
 */
static int __devinit		cheeky_probe(struct usb_interface*		interface,
					  const struct usb_device_id*	entity)
{
	int				ret = 0;
	data_t*			data;

	printk(KERN_INFO "cheeky_display: %x:%x device plugged.\n",
	       entity->idVendor,
	       entity->idProduct);

	/* Allocating private structure		*/
	data = kmalloc(sizeof(data_t), GFP_KERNEL);
	if (!data) {
		printk(KERN_WARNING "cheeky_display: unable to allocate private structure.\n");
		ret = -ENOMEM;
		goto error;
	}
	memset(data, 0x0, sizeof(data_t));
	data->buffer = kmalloc(MAX_CHARS, GFP_KERNEL);
	if (!(data->buffer)) {
		printk(KERN_WARNING "cheeky_display: unable to allocate private buffer.\n");
		ret = -ENOMEM;
		goto error;
	}

	/* Initialize default values			*/
	usb_set_intfdata(interface, data);

	data->udev = usb_get_dev(interface_to_usbdev(interface));
	data->interface = interface;

	init_MUTEX(&data->sem_buffer);
	strncpy(data->buffer, "WORKING ", 8);
	data->length = 8;
	data->start_character = 0;

	SET_BRIGHNESS(data->params, LED_HIGH_BR);
	SET_SPEED(data->params, 5);

	/* Attach private structure to the usb device	*/
	data->display_packets = kmalloc(sizeof(usb_packet_t) * 4, GFP_KERNEL);
	if (!data->display_packets) {
		printk(KERN_WARNING "cheeky_display: Cannot allocate DMA buffer.\n");
		ret = -ENOMEM;
		goto error;
	}

	/* Register the usb device and get a minor	*/
	ret = usb_register_dev(interface, &cheeky_class);
	if (ret) {
		printk(KERN_WARNING "cheeky_display: Unable to get a minor.\n");
		goto error;
	}

	data->kthread = kthread_run(cheeky_refresh, data, "cheeky_refresh");

	return (0);

 error:
	usb_set_intfdata(interface, NULL);
	return (ret);
}

/**
 * @brief
 *	This function is called when someone unplug the device. Its work is
 *	to remove the /dev/cheeky device and to remove all allocated data.
 */
static void __devexit		cheeky_disconnect(struct usb_interface* interface)
{
	data_t*			data;

	data = usb_get_intfdata(interface);

	/*
	 * Stopping the kthread which is in charge to send usb packets to the device
	 */
	kthread_stop(data->kthread);

	/* Freeing private data */
	kfree(data->buffer);
	usb_set_intfdata(interface, NULL);

	/* Deregister the char device in /dev */
	usb_deregister_dev(interface, &cheeky_class);

	printk(KERN_INFO "cheeky_display: device unplugged.\n");
}

static struct usb_driver	cheeky_driver = {
	.name	= "cheeky_display",
	.id_table	= cheeky_id_table,
	.probe	= cheeky_probe,
	.disconnect	= cheeky_disconnect,
};

/**
 * @brief
 *	This function is called when we register this driver (modprobe/insmode).
 *	It tells the kernel which devices this driver will be supporting.
 * @return 0 on success, a negative number on failure.
 */
static int __init		cheeky_init(void)
{
	int			ret = 0;

	ret = usb_register(&cheeky_driver);
	if (ret)
		printk(KERN_WARNING "cheeky_display: Unable to register led display driver.\n");

	return (ret);
}

/**
 * @brief
 *	This function unregister (rmmode) the led display driver.
 */
static void __exit		cheeky_exit(void)
{
	usb_deregister(&cheeky_driver);
}

module_init(cheeky_init);
module_exit(cheeky_exit);
