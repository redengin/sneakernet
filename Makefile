# FIXME on ubuntu make can't find the docker executable
DOCKER = `which docker`

#--------------------------------------------------------------------------------
# Use to initialize new ESP32 SneakerNet node from released binaries
#--------------------------------------------------------------------------------
RELEASE_VERSION ?= 1.0.0
RELEASE_URL ?= https://github.com/redengin/sneakernet/releases/download/${RELEASE_VERSION}

RELEASE_DIR ?= release/$(RELEASE_VERSION)
$(RELEASE_DIR)/%.bin:
	@mkdir -p $(RELEASE_DIR)
	@wget $(RELEASE_URL)/$(@F) \
		--show-progress -O $@

.PHONY: sneakernet.esp32
sneakernet.esp32: $(RELEASE_DIR)/bootloader.bin \
				  $(RELEASE_DIR)/partition-table.bin \
				  $(RELEASE_DIR)/ota_data_initial.bin \
				  $(RELEASE_DIR)/SneakerNet.bin

# clean off any stored data
	@${DOCKER} run --rm -it --privileged \
			espressif/idf \
		esptool.py erase_flash
# load the target image
	@${DOCKER} run --rm -it --privileged \
		--volume $(abspath $(RELEASE_DIR)):/tmp/release \
		--workdir /tmp/release \
			espressif/idf \
		esptool.py --before=default_reset --after=hard_reset \
			--baud 460800 \
			write_flash \
				0x1000 bootloader.bin \
				0x8000 partition-table.bin \
				0xd000 ota_data_initial.bin \
				0x10000 SneakerNet.bin

.PHONY: sneakernet.esp32.create_release
sneakernet.esp32.create_release:
	mkdir -p $(RELEASE_DIR)
	cp firmware/esp32/build/bootloader/bootloader.bin $(RELEASE_DIR)
	cp firmware/esp32/build/partition_table/partition-table.bin $(RELEASE_DIR)
	cp firmware/esp32/build/ota_data_initial.bin $(RELEASE_DIR)
	cp firmware/esp32/build/SneakerNet.bin $(RELEASE_DIR)