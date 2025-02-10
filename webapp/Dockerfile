# base
FROM alpine as npm
RUN apk add \
        npm \
    && rm -rf /var/cache/apk/*

# Fake REST backend
FROM npm as rest-fake
RUN npm install -g express

# Angular
FROM npm as angular-cli
RUN npm install -g @angular/cli

# Json Server
FROM npm as json-server
RUN npm install -g json-server