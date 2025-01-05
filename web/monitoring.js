document.addEventListener('DOMContentLoaded', function() {
    fetchPlayers();
});

function fetchPlayers() {
    fetch('players/players.txt')
        .then(response => response.text())
        .then(data => {
            const players = parsePlayers(data);
            renderPlayers(players);
        });
}

function parsePlayers(data) {
    const lines = data.split('\n');
    const players = [];
    lines.forEach(line => {
        const [nick, steamid, devicename] = line.split('-');
        players.push({ nick: nick.trim(), steamid: steamid.trim(), devicename: devicename.trim() });
    });
    return players;
}

function renderPlayers(players) {
    const playerList = document.getElementById('playerList');
    players.forEach(player => {
        const playerRow = document.createElement('div');
        playerRow.className = 'player-row';
        playerRow.innerHTML = `
            <div class="player-column">${player.nick}</div>
            <div class="player-column">${player.steamid}</div>
            <div class="player-column">${player.devicename}</div>
            <div class="player-column"><button class="ban-button" data-steamid="${player.steamid}" data-devicename="${player.devicename}">Check...</button></div>
        `;
        playerList.appendChild(playerRow);
        checkBanStatus(player.steamid, playerRow);
    });
}

function checkBanStatus(steamid, playerRow) {
    fetch('banned/usersbanned.txt')
        .then(response => response.text())
        .then(data => {
            const banned = data.split('\n').includes(steamid);
            const button = playerRow.querySelector('button');
            if (banned) {
                button.textContent = 'Unban';
                button.className = 'unban-button';
                button.onclick = () => toggleBan(steamid, 'unban');
            } else {
                button.textContent = 'Ban';
                button.className = 'ban-button';
                button.onclick = () => toggleBan(steamid, 'ban');
            }
        });
}

function toggleBan(steamid, action) {
    const devicename = event.target.dataset.devicename;
    fetch(`banned/${action}.php`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: `steamid=${steamid}&devicename=${devicename}`
    }).then(() => {
        location.reload();
    });
}
