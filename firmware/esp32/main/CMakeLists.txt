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

    EMBED_FILES
        # FIXME https not able to handle keys
        # "~/keystore/sneakernet.pem"         # HTTPS private key
        # "~/keystore/sneakernet_public.pem"  # HTTPS public key
        # add portal files
        "../../../webapp/angular/dist/browser/favicon.ico"
        "../../../webapp/angular/dist/browser/index.html"
        "../../../webapp/angular/dist/browser/styles.css"
        "../../../webapp/angular/dist/browser/main.js"
        "../../../webapp/angular/dist/browser/polyfills.js"
)