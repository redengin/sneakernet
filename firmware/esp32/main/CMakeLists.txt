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
        # FIXME
        # "../../portal.html"
        "~/keystore/sneakernet.pem" # public key
        "~/keystore/sneakernet"     # private key
)