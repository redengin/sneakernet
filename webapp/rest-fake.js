const PORT = 3000;

const express = require('/usr/local/lib/node_modules/express');
const app = express();

// fake catalog
catalog = {
    "short": { isFolder: true },
    "long%20directory": { isFolder: true },
    "file%201": {
        isFolder: false,
        size: 1,
        timestamp: "2025-02-12T19:26:24Z",
        hasIcon: false,
    },
    "file 2": {
        isFolder: false,
        size: 2,
        timestamp: "2025-02-12T19:26:24Z",
        title: "This is File 2 with a really long name that should wrap",
        hasIcon: false,
    },
};

app.get("/api/catalog/:path?/", (request, response) => {
    // FIXME
    // response.send(catalog["/" + (request.params.path || "")]);
    response.send(catalog);
});


// download the body (file content)
app.use((request, response, next) => {
    let rawBody = '';
    request.on('data', (chunk) => { rawBody += chunk; });
    request.on('end', () => { request.rawBody = rawBody; next(); });
});

app.put("/api/catalog/:path?/", (request, response) => {
    response.send("");
});

app.delete("/api/catalog/:path?/:file", (request, response) => {
    // TODO remove from catalog
    response.send();
});


const http = require('http');
const restServer = http.createServer(app);
restServer.on('error', (err) => {
    console.error('Server error:', err);
});
restServer.listen(PORT, () => { console.log("fake rest server started on port " + PORT); });





