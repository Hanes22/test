const WebSocket = require('ws');

const server = new WebSocket.Server({ port: process.env.PORT || 3000 });

// rooms[roomCode] = [ { ws, name, hp, maxHp } ]
let rooms = {};

function getRoomClients(room) {
    if (!rooms[room]) rooms[room] = [];
    return rooms[room];
}

function broadcast(room, data, exclude) {
    const msg = JSON.stringify(data);
    getRoomClients(room).forEach(client => {
        if (client.ws !== exclude && client.ws.readyState === WebSocket.OPEN) {
            client.ws.send(msg);
        }
    });
}

function broadcastAll(room, data) {
    const msg = JSON.stringify(data);
    getRoomClients(room).forEach(client => {
        if (client.ws.readyState === WebSocket.OPEN) {
            client.ws.send(msg);
        }
    });
}

server.on('connection', (ws) => {
    let currentRoom = null;
    let currentName = null;

    ws.on('message', (message) => {
        let data;
        try { data = JSON.parse(message); } catch { return; }

        const room = data.room || 'default';
        const clients = getRoomClients(room);

        // ── join ──────────────────────────────────────────────────────────────
        if (data.type === 'join') {
            currentRoom = room;
            currentName = data.name || 'Player';

            // Remove any stale entry for this ws (reconnect case)
            rooms[room] = clients.filter(c => c.ws !== ws);

            const isHost = rooms[room].length === 0;
            const entry = { ws, name: currentName, hp: data.hp || 100, maxHp: data.maxHp || 100 };
            rooms[room].push(entry);

            // Tell this client its role
            ws.send(JSON.stringify({ type: 'role', isHost, room }));

            // If a peer is already in the room, cross-notify both
            if (!isHost) {
                const host = rooms[room][0];
                // Tell the guest about the host (joined packet)
                ws.send(JSON.stringify({
                    type: 'joined',
                    name: host.name,
                    hp: host.hp,
                    maxHp: host.maxHp,
                    room
                }));
                // Tell the host about the guest
                host.ws.send(JSON.stringify({
                    type: 'joined',
                    name: currentName,
                    hp: data.hp || 100,
                    maxHp: data.maxHp || 100,
                    room
                }));
            }
            return;
        }

        // ── pong (reply to server ping — just swallow it) ─────────────────────
        if (data.type === 'pong') return;

        // ── leave ─────────────────────────────────────────────────────────────
        if (data.type === 'leave') {
            broadcast(room, data, ws);
            return;
        }

        // ── relay: state, attack, mob_hit, mob_sync ───────────────────────────
        broadcast(room, data, ws);
    });

    ws.on('close', () => {
        if (currentRoom) {
            rooms[currentRoom] = getRoomClients(currentRoom).filter(c => c.ws !== ws);
            if (currentName) {
                broadcast(currentRoom, { type: 'leave', room: currentRoom, name: currentName }, ws);
            }
            if (rooms[currentRoom].length === 0) {
                delete rooms[currentRoom];
            }
        }
    });
});

// Ping all clients every 30s so multiplayer.js can measure latency
setInterval(() => {
    const now = Date.now();
    server.clients.forEach(ws => {
        if (ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify({ type: 'ping', t: now }));
        }
    });
}, 30000);

console.log('Server running on port', process.env.PORT || 3000);
