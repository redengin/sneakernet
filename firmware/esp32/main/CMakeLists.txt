idf_component_register(
    SRCS
        "main.cpp"
        "SdCard.cpp"
        "WifiAccessPoint.cpp"
        "WebServer.cpp"
        "dns_server.c"
        "rest/firmware.cpp"
        "Catalog.cpp"
        "rest/catalog.cpp"
        "rest/utils.cpp"

    EMBED_TXTFILES
        #[[
            Create a self-signed cert
            openssl req -newkey rsa:4096 -nodes -keyout sneakernet_https.key \
                -x509 -days 3650 -out sneakernet_https.cert \
                -subj "/C=US/ST=Massachusetts/L=Boston/O=SneakerNet/OU=Org/CN=sneakernet.monster"
        ]]
        # CAPPORT needs a CA-signed certificate
        "~/keystore/sneakernet_https.cert"
        "~/keystore/sneakernet_https.key"   # private key


    EMBED_FILES
        # add captive portal files
        "../../../webapp/angular/dist/browser/captiveportal.html"
        # add portal files
        "../../../webapp/angular/dist/browser/favicon.ico"
        "../../../webapp/angular/dist/browser/index.html"
        "../../../webapp/angular/dist/browser/styles.css"
        "../../../webapp/angular/dist/browser/main.js"
        "../../../webapp/angular/dist/browser/polyfills.js"
)