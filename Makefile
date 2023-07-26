SHELL:=/bin/bash
PWD=$(shell pwd)
PORT=/dev/ttyACM0
FQBN=esp32:esp32:esp32c3:FlashMode=dio
FILENAME=esp32-shutter-speed-tester

compile:
	arduino-cli compile \
		--fqbn $(FQBN) \
		--log-level=info \
		--libraries $(PWD)/libraries/ \
		--build-path=$(PWD)/build \
		--build-property 'compiler.warning_level=all' \
		--warnings all \
		.

upload:
	arduino-cli upload \
		-p $(PORT) \
		--fqbn $(FQBN) \
		--log-level=debug \
		--input-dir=$(PWD)/build

monitor:
	arduino-cli monitor \
		-p $(PORT) \
		--config Baudrate=9600

clean:
	rm -rf ./build
