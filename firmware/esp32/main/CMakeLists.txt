idf_component_register(
    SRCS
        "main.cpp"
        "SdCard.cpp"
        "WifiAccessPoint.cpp"
        "WebServer.cpp"
        "rest/firmware.cpp"
        "Catalog.cpp"
        "rest/catalog.cpp"
        "rest/utils.cpp"

    EMBED_TXTFILES
        "/project/keystore/sneakernet_https_priv.pem"  # HTTPS private key
        "/project/keystore/sneakernet_https_pub.pem"   # HTTPS public key

    EMBED_FILES
        # add portal files
        "/project/webapp/angular/dist/browser/favicon.ico"
        "/project/webapp/angular/dist/browser/index.html"
        "/project/webapp/angular/dist/browser/styles.css"
        "/project/webapp/angular/dist/browser/main.js"
        "/project/webapp/angular/dist/browser/polyfills.js"
)