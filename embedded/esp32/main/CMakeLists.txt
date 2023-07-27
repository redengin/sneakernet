idf_component_register(
    SRCS    "main.cpp"
            "dns_server.c"
            "SneakerNet.cpp"
            "sneakernet/Catalog.cpp"
            "WebServer.cpp"
    EMBED_FILES "index.html"
)
