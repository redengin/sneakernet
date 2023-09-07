# introspect the path of the project
mkfile_path := $(lastword $(MAKEFILE_LIST))
SOURCE_DIR := $(dir $(mkfile_path))

# setup user identification
UID ?= $(shell id -u)
GID ?= $(shell id -g)

.PHONY: angular-init
angular-init:
	@docker run --rm -it \
		--user $(UID):$(GID) \
		--volume $(abspath $(SOURCE_DIR)):/tmp/ng \
		--env HOME=/tmp/ng \
		--workdir /tmp/ng/angular \
		$(NG_DOCKER_IMAGE) \
			npm install

.PHONY: build-webapp
build-webapp: angular-init
	@docker run --rm -it \
		--user $(UID):$(GID) \
		--volume $(abspath $(SOURCE_DIR)):/tmp/ng \
		--env HOME=/tmp/ng \
		--workdir /tmp/ng/angular \
		$(NG_DOCKER_IMAGE) \
			ng build --deploy-url "/app/"

.PHONY: view-webapp
#view-webapp: build-webapp
view-webapp: 
	@docker run --rm -it \
		--network host \
		--user $(UID):$(GID) \
		--volume $(abspath $(SOURCE_DIR)):/tmp/ng \
		--workdir /tmp/ng/angular \
		$(NG_DOCKER_IMAGE) \
			ng serve

#-------------------------------------------------------------------------------
# Docker Images
#-------------------------------------------------------------------------------
NG_DOCKER_IMAGE ?= angular/alpine
.PHONY: $(NG_DOCKER_IMAGE)
$(NG_DOCKER_IMAGE):
	@docker build -t $@ \
		tools/Docker/angular/