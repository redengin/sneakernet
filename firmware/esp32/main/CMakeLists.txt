idf_component_register(
    SRCS
        "main.cpp"
        "WifiAccessPoint.cpp"
        "WebServer.cpp"
        "SdCard.cpp"
        "Catalog.cpp"
        "rest/catalog.cpp"
        "rest/utils.cpp"

    EMBED_FILES
        "~/keystore/sneakernet.pem" # HTTPS public key
        "~/keystore/sneakernet"     # HTTPS private key
        # add portal files
        "../../../webapp/angular/dist/browser/favicon.ico"
        "../../../webapp/angular/dist/browser/index.html"
        "../../../webapp/angular/dist/browser/styles.css"
        "../../../webapp/angular/dist/browser/main.js"
        "../../../webapp/angular/dist/browser/polyfills.js"
)