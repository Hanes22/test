const WebSocket = require('ws');

const server = new WebSocket.Server({ port: process.env.PORT || 3000 });

let rooms = {};

server.on('connection', (ws) => {
    ws.on('message', (message) => {
        let data;

        try {
            data = JSON.parse(message);
        } catch {
            return;
        }

        const room = data.room || "default";

        if (!rooms[room]) rooms[room] = [];
        if (!rooms[room].includes(ws)) rooms[room].push(ws);

        rooms[room].forEach(client => {
            if (client !== ws && client.readyState === WebSocket.OPEN) {
                client.send(JSON.stringify(data));
            }
        });
    });

    ws.on('close', () => {
        for (let room in rooms) {
            rooms[room] = rooms[room].filter(c => c !== ws);
        }
    });
});

console.log("Server running...");
