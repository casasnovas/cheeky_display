DESTDIR := /lib/modules/$(shell uname -r)

all: cheeky_driver cheeky_control

cheeky_driver:
	make -C src/cheeky_driver/
	cp src/cheeky_driver/cheeky_driver.ko ./

cheeky_control:
	make -C src/cheeky_control/
	cp src/cheeky_control/cheeky_control ./

.PHONY: doc

doc:
	doxygen Doxyfile

install: all
	groupadd cheeky
	echo "KERNEL==\"cheeky[0-9]*\", GROUP=\"cheeky\"" > /etc/udev/rules.d/10-cheeky.rules
	cp cheeky_control /usr/bin/cheeky_control	&& \
	chown root:cheeky /usr/bin/cheeky_control	&& \
	chmod 750 /usr/bin/cheeky_control		&& \
	mkdir -p $(DESTDIR)/misc
	install cheeky_driver.ko $(DESTDIR)/misc/
	depmod -a
	modprobe cheeky_driver
	@echo "*****"
	@echo "To use cheeky_control to control your newly installed led display, your user must be a member of the cheeky group."
	@echo "The driver won't be loaded at next boot, follow the instructions for your distribution to make it automatically loading at next boot."
	@echo "*****"

uninstall:
	rm /etc/udev/rules.d/10-cheeky.rules
	groupdel cheeky
	rmmod cheeky_driver
	rm -f /usr/bin/cheeky_control
	rm -f $(DESTDIR)/misc/cheeky_driver.ko

clean:
	make -C src/cheeky_driver/ clean
	make -C src/cheeky_control/ clean
	rm -f cheeky_control
	rm -f cheeky_driver.ko
	rm -Rf doc/*