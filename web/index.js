document.getElementById('loginForm').addEventListener('submit', function(e) {
    e.preventDefault();

    const username = document.getElementById('username').value;
    const password = document.getElementById('password').value;

    fetch('accounts/users.txt')
        .then(response => response.text())
        .then(data => {
            const users = parseUsers(data);
            if (users[username] === password) {
                showSuccessMessage();
            } else {
                showErrorMessage();
            }
        });
});

function parseUsers(data) {
    const lines = data.split('\n');
    const users = {};
    lines.forEach(line => {
        const [user, pass] = line.split('=');
        users[user.trim()] = pass.trim();
    });
    return users;
}

function showErrorMessage() {
    document.getElementById('errorMessage').classList.remove('hidden');
    document.getElementById('successMessage').classList.add('hidden');
}

function showSuccessMessage() {
    document.getElementById('successMessage').classList.remove('hidden');
    document.getElementById('errorMessage').classList.add('hidden');
    setTimeout(() => {
        window.location.href = 'select.html';
    }, 2000);
}
