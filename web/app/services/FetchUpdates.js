import steam from '../helpers/Steam.js';

import Server from '../models/Server.js';
import ServerRecord from '../models/ServerRecord.js';

export default async function() {
    let servers = await steam.getServers();
    servers = servers.filter((item) => {
        return (item.version >= 1806 && item.version < 2500);
    });

    let outdatedServers = await Server.find({ date: { $lt: Math.floor(Date.now() / 1000) - 3600 }});

    outdatedServers.forEach(async (server) => {
        await Server.deleteOne({ address: server.address, port: server.port });
    });

    await ServerRecord.deleteMany({ date: { $lt: Math.floor(Date.now() / 1000) - 43200 }});

    servers.forEach(async (item) => {
        let addr = item.addr.split(':');

        let server = await Server.findOne({ address: addr[0], port: addr[1] });

        if(server) {
            if(Math.floor(Date.now() / 1000) - server.record_at > 450) {
                await ServerRecord.insertMany({
                    address: server.address,
                    port: server.port,
                    players: server.players,
                    max_players: server.max_players,
                    date: server.date
                });
                server.record_at = Math.floor(Date.now() / 1000);
            }

            server.name = item.name;
            server.players = item.players;
            server.max_players = item.max_players;
            server.map = item.map;
            server.date = Math.floor(Date.now() / 1000);
            server.version = item.version;

            await server.save();
        } else {
            await Server.insertMany({
                name: item.name,
                address: addr[0],
                port: parseInt(addr[1]),
                version: item.version,
                players: item.players,
                max_players: item.max_players,
                map: item.map,
                date: Math.floor(Date.now() / 1000),
                record_at: Math.floor(Date.now() / 1000)
            });
        }
    });
}