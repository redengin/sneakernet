# base
FROM alpine as base
RUN apk add \
        npm \
    && rm -rf /var/cache/apk/*

# Angular
FROM base as angular-cli
RUN npm install -g @angular/cli

# Json Server
FROM base as json-server
RUN npm install -g json-server