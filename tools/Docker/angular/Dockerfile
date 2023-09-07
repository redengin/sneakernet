FROM alpine

# install npm (from wuich angular comes)
RUN apk add \
    dumb-init \
    npm \
 && rm -rf /var/cache/apk/*

# install angular
RUN npm install -g @angular/cli

ENTRYPOINT ["/usr/bin/dumb-init", "--"]