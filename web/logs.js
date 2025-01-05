document.getElementById('logForm').addEventListener('submit', function(e) {
    e.preventDefault();
    document.getElementById('logForm').classList.add('hidden');
    document.getElementById('logOptions').classList.remove('hidden');
});

function requestScreenshot() {
    const steamid = document.getElementById('steamid').value;
    fetch(`screens/${steamid}.png`)
        .then(response => {
            if (!response.ok) {
                throw new Error('Screenshot not found');
            }
            return response.blob();
        })
        .then(blob => {
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.style.display = 'none';
            a.href = url;
            a.download = `${steamid}.png`;
            document.body.appendChild(a);
            a.click();
            URL.revokeObjectURL(url);
        })
        .catch(error => {
            alert(error.message);
        });
}

function requestLastDetectLog() {
    const steamid = document.getElementById('steamid').value;
    fetch(`detectedusers/${steamid}.txt`)
        .then(response => {
            if (!response.ok) {
                throw new Error('Log not found');
            }
            return response.blob();
        })
        .then(blob => {
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.style.display = 'none';
            a.href = url;
            a.download = `${steamid}.txt`;
            document.body.appendChild(a);
            a.click();
            URL.revokeObjectURL(url);
        })
        .catch(error => {
            alert(error.message);
        });
}
