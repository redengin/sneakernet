# introspect the path of the project
mkfile_path := $(lastword $(MAKEFILE_LIST))
SOURCE_DIR := $(dir $(mkfile_path))

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

