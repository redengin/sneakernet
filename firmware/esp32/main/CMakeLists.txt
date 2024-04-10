idf_component_register(
    SRCS
       "main.cpp"
       "dns_server.c"
       "sneakernet/SneakerNet.cpp"
       "sneakernet/WebServer.cpp"
    EMBED_FILES
        "welcome.html"
        "app/favicon.ico"
        "app/index.html"
        "app/main.js"
        "app/polyfills.js"
        "app/styles.css"
        # "app/chunk-IGQZRBJ4.js"
        # "app/chunk-2DZBRAUX.js"
)
