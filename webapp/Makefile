# configure docker user
DOCKER_USER ?= ${shell id -u}:${shell id -g}

~/.npm:
	@mkdir -p $@

.PHONY: ng-shell
ng-shell: ~/.npm
	@DOCKER_USER=$(DOCKER_USER) \
	docker compose run --rm angular

.PHONY: ng-serve
ng-serve: ~/.npm
	@DOCKER_USER=$(DOCKER_USER) \
	docker compose up ng-serve
	docker compose down

.PHONY: ng-build
ng-build: ~/.npm
	@DOCKER_USER=$(DOCKER_USER) \
	docker compose run --rm angular \
		ng build
# ng build --base-href = "/app/"