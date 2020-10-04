CC = gcc
CCFLAGS = -W -Wall

BINARY = boiler-controller-web-server
SYSTEMD_SERVICE = boiler-controller-web-server.service

all:
	$(CC) $(CCFLAGS) -IIncludes Sources/Boiler.c Sources/Main.c Sources/Page_Index.c -lmicrohttpd -o $(BINARY)

clean:
	rm -f $(BINARY)

install: all
	@# Make sure this is executed as root
	@if [ $(shell id -u) -ne 0 ]; then printf "\033[31mThis rule must be executed as root.\033[0m\n"; false; fi
	
	@# Install binary
	cp $(BINARY) /usr/bin
	
	@# Create init script
	echo "[Unit]" > /lib/systemd/system/$(SYSTEMD_SERVICE)
	echo "Wants=network-online.target" >> /lib/systemd/system/$(SYSTEMD_SERVICE)
	echo "After=network-online.target" >> /lib/systemd/system/$(SYSTEMD_SERVICE)
	echo "Description=Boiler controller web server" >> /lib/systemd/system/$(SYSTEMD_SERVICE)
	echo "[Service]" >> /lib/systemd/system/$(SYSTEMD_SERVICE)
	echo "Type=simple" >> /lib/systemd/system/$(SYSTEMD_SERVICE)
	echo "ExecStart=/usr/bin/$(BINARY) 8888" >> /lib/systemd/system/$(SYSTEMD_SERVICE)
	echo "[Install]" >> /lib/systemd/system/$(SYSTEMD_SERVICE)
	echo "WantedBy=multi-user.target" >> /lib/systemd/system/$(SYSTEMD_SERVICE)
	systemctl enable $(SYSTEMD_SERVICE)
	systemctl start $(SYSTEMD_SERVICE)

uninstall:
	@# Make sure this is executed as root
	@if [ $(shell id -u) -ne 0 ]; then printf "\033[31mThis rule must be executed as root.\033[0m\n"; false; fi
	
	@# Remove binary
	rm -f /usr/bin/$(BINARY)
	
	@# Remove init script
	systemctl stop $(SYSTEMD_SERVICE)
	systemctl disable $(SYSTEMD_SERVICE)
	rm -f /lib/systemd/system/$(SYSTEMD_SERVICE)
