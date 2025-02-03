FROM alpine as baseline
RUN apk add \
        npm \
    && rm -rf /var/cache/apk/*

# Angular
FROM baseline as angular-cli
RUN npm install -g @angular/cli

# Json Server
FROM baseline as json-server
RUN npm install -g json-server