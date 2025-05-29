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
        # FIXME for CAPPORT likely needs a signed certificate
        "~/keystore/sneakernet_https_priv.pem"  # HTTPS private key
        "~/keystore/sneakernet_https_pub.pem"   # HTTPS public key

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