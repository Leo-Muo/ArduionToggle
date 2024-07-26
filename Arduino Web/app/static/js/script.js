document.addEventListener('DOMContentLoaded', function () {
    const toggleSwitch = document.getElementById('toggleSwitch');
    const body = document.body;
    const socket = io.connect('http://172.20.10.3:5001');

    function updateBackground(isChecked) {
        console.log(isChecked)
        if (isChecked) {
            body.style.backgroundColor = '#fff8dc'; // Light is on
        } else {
            body.style.backgroundColor = '#f2f2f2'; // Light is off
        }
    }

    function toggleLight() {
        socket.emit('toggle_light');
    }


    toggleSwitch.addEventListener('change', toggleLight);

    // Set initial background color based on initial checkbox state
    updateBackground(toggleSwitch.checked);

    // Listen for light status changes from the server
    socket.on('light_status_changed', function(data) {
        toggleSwitch.checked = data.light_status;
        console.log('Updated toggleSwitch.checked:', toggleSwitch.checked);
        updateBackground(data.light_status);
    });

});
