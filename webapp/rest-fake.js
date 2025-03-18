const PORT = 3000;

const express = require('/usr/local/lib/node_modules/express');
const app = express();

// fake catalog
catalog = {
    "locked" : false,
    "entries" : {
        "short" : { isFolder: true },
        "long directory" : { isFolder: true },
        "file 1": { isFolder: false,
            size: 1,
            timestamp : "2025-02-12T19:26:24Z",
            hasIcon: false,
        },
        "file 2": { isFolder: false,
            size: 2,
            timestamp : "2025-02-12T19:26:24Z",
            title: "This is File 2",
            hasIcon: false,
        },
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
    console.log(request.rawBody);
    // response.status(500).send();
    response.send("" + request.rawBody.length);
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
restServer.listen(PORT, () => { console.log("fake rest server started on port "+PORT); });





