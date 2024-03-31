# introspect the path of the project
mkfile_path := $(lastword $(MAKEFILE_LIST))
SOURCE_DIR := $(dir $(mkfile_path))

# setup user identification
UID ?= $(shell id -u)
GID ?= $(shell id -g)
DOCKER_USER = ${UID}:${GID}
DOCKER_COMPOSE ?= docker compose

.PHONY: angular-shell
angular-shell:
	DOCKER_USER=$(DOCKER_USER) $(DOCKER_COMPOSE) run --rm angular \
		sh

.PHONY: angular-serve
angular-serve:
	DOCKER_USER=$(DOCKER_USER) $(DOCKER_COMPOSE) run --rm --service-ports --workdir /angular/sneakernet angular \
		ng serve

.PHONY: angular-build
angular-build:
	DOCKER_USER=$(DOCKER_USER) $(DOCKER_COMPOSE) run --rm --service-ports --workdir /angular/sneakernet angular \
    ng build --deploy-url "/app" --output-hashing none
