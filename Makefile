#--------------------------------------------------------------------------------
# Use to initialize new ESP32 SneakerNet node from released binaries
#--------------------------------------------------------------------------------
RELEASE_DIR ?= build/release
RELEASE_URL ?= https://github.com/redengin/sneakernet/releases/download/Release1

$(RELEASE_DIR)/%.bin:
	@mkdir -p $(RELEASE_DIR)
	@wget $(RELEASE_URL)/$(@F) \
		--quiet --show-progress -O $@

.PHONY: sneakernet.esp32
sneakernet.esp32: $(RELEASE_DIR)/bootloader.bin \
				  $(RELEASE_DIR)/partition-table.bin \
				  $(RELEASE_DIR)/ota_data_initial.bin \
				  $(RELEASE_DIR)/esp32-sneakernet.bin
	docker run --rm -it --privileged \
		--volume $(abspath $(RELEASE_DIR)):/tmp/release \
		--workdir /tmp/release \
			espressif/idf \
		esptool.py -b 460800 --before=default_reset --after=hard_reset \
			write_flash --flash_mode dio --flash_freq 40m \
				0x1000 bootloader.bin \
				0x9000 partition-table.bin \
				0xf000 ota_data_initial.bin \
				0x120000 esp32-sneakernet.bin

#--------------------------------------------------------------------------------
# Use to develop SneakerNet
#--------------------------------------------------------------------------------
# setup user identification
UID ?= $(shell id -u)
GID ?= $(shell id -g)
DOCKER_USER = ${UID}:${GID}
DOCKER_COMPOSE ?= docker compose

.PHONY: angular-serve
angular-serve:
	@DOCKER_USER=$(DOCKER_USER) $(DOCKER_COMPOSE) run --rm \
		--service-ports \
		--volume $(abspath angular/sneakernet):/angular \
		--env \
			HOME=/angular \
		--workdir /angular \
			angular \
	  ng serve

.PHONY: angular-build
angular-build:
	@DOCKER_USER=$(DOCKER_USER) $(DOCKER_COMPOSE) run --rm \
		--volume $(abspath angular/sneakernet):/angular \
		--env \
			HOME=/angular \
		--workdir /angular \
			angular \
      ng build --base-href "/app/"

.PHONY: angular-shell
angular-shell:
	DOCKER_USER=$(DOCKER_USER) $(DOCKER_COMPOSE) run --rm \
		--volume $(abspath angular/sneakernet):/angular \
		--env \
			HOME=/angular \
		--workdir /angular \
			angular \
	  sh


.PHONY: firmware/esp32.build
firmware/esp32.build: angular-build
	@DOCKER_USER=$(DOCKER_USER) $(DOCKER_COMPOSE) run --rm \
			espressif-idf \
	  idf.py build

.PHONY: firmware/esp32.flash
# firmware/esp32.flash: firmware/esp32.build
firmware/esp32.flash:
	@DOCKER_USER=$(DOCKER_USER) $(DOCKER_COMPOSE) run --rm \
		--volume /dev:/dev \
			espressif-idf \
		idf.py flash

.PHONY: firmware/esp32.monitor
firmware/esp32.monitor:
	@DOCKER_USER=$(DOCKER_USER) $(DOCKER_COMPOSE) run --rm \
		--volume /dev:/dev \
			espressif-idf \
		idf.py monitor
