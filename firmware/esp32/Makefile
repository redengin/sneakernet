# FIXME on ubuntu make can't find the docker executable
DOCKER = `which docker`

# configure docker user
DOCKER_USER ?= ${shell id -u}:${shell id -g}

.PHONY: build
build:
	@DOCKER_USER=$(DOCKER_USER) \
		docker compose run --rm idf \
			idf.py build

.PHONY: flash
flash:
	@${DOCKER} run --rm -it --privileged \
		--volume ./build:/release \
	 	--workdir /release \
			espressif/idf \
		esptool.py --before=default_reset --after=hard_reset \
			--baud 460800 \
			write_flash \
				0x1000 bootloader/bootloader.bin \
				0x8000 partition_table/partition-table.bin \
				0xD000 ota_data_initial.bin \
				0x10000 SneakerNet.bin


.PHONY: monitor
monitor:
	@${DOCKER} run --rm -it --privileged \
		--volume .:/release \
	 	--workdir /release \
			espressif/idf \
		idf.py monitor
