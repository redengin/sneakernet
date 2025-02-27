# configure docker user
DOCKER_USER ?= ${shell id -u}:${shell id -g}

~/.npm:
	@mkdir -p $@

.PHONY: npm-shell
npm-shell: ~/.npm
	@DOCKER_USER=$(DOCKER_USER) \
		docker compose run --rm npm

.PHONY: ng-shell
ng-shell: ~/.npm
	@DOCKER_USER=$(DOCKER_USER) \
		docker compose run --rm ng

.PHONY: ng-serve
ng-serve: ~/.npm
	@DOCKER_USER=$(DOCKER_USER) \
		docker compose up ng-serve
	@DOCKER_USER=$(DOCKER_USER) \
		docker compose down

.PHONY: ng-build
ng-build: ~/.npm
	@DOCKER_USER=$(DOCKER_USER) \
		docker compose run --rm ng \
			ng build --deploy-url=app/