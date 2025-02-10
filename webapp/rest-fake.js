const PORT = 3000;

const express = require('/usr/local/lib/node_modules/express');
const app = express();

// fake catalog
catalog = {
    "/" : {
        "locked" : false,
        "subfolders" : [ "short", "long directory" ],
        "files" : [
            { "name" : "file 1", "size" : 1, "timestamp" : "", "hasIcon" : false },
            { "name" : "file 2", "size" : 2, "timestamp" : "", "title" : "Title 2", "hasIcon" : false }
        ]
    },
    "/short" : {
        "locked" : false,
        "files" : [
            { "name" : "short file 1", "size" : 1, "timestamp" : "", "hasIcon" : false },
            { "name" : "short file 2", "size" : 2, "timestamp" : "", "title" : "Short Title 2", "hasIcon" : false }
        ]
    },
    "/long directory" : {
        "locked" : false,
        "files" : [
            { "name" : "long file 1", "size" : 1, "timestamp" : "", "hasIcon" : false },
            { "name" : "long file 2", "size" : 2, "timestamp" : "", "title" : "Long Title 2", "hasIcon" : false }
        ]
    }
};

app.get("/api/catalog/", (request, response) => {
    response.send(catalog["/"]);
});

app.get("/api/catalog/:path/", (request, response) => {
    response.send(catalog["/" + request.params.path]);
});

const http = require('http');
const restServer = http.createServer(app);
restServer.on('error', (err) => {
    console.error('Server error:', err);
  });
restServer.listen(PORT, () => { console.log("fake rest server started on port "+PORT); });





