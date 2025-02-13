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
        # TODO add portal files
)