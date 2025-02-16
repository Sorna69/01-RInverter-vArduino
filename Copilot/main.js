var connection = new WebSocket('ws://' + location.hostname + '/ws', ['arduino']);

connection.onopen = function () {
    connection.send('Received from Client');
    console.log('Connected');
};

connection.onerror = function (error) {
    console.log('WebSocket Error', error);
};

connection.onmessage = function (e) {
    console.log('Received from server: ', e.data);
    processReceived(e.data);
};

connection.onclose = function () {
    console.log('WebSocket connection closed');
};

function processReceived(data) {
    var json = JSON.parse(data);

    if (json.command == 'updateGPIO') {
        console.log('Process updateGPIO. id:' + json.id + ' Status: ' + json.status);
        updateGPIO(json.id, json.status);
    } else if (json.command == 'updateDATA') {
        updateDATA(json.pHData, json.tempData, json.modeData);
    } else {
        console.log('No se reconoce el mensaje');
    }
}

function sendGPIO(id, status) {
    var data = {
        device: "ESP32",
        command: "setGPIO",
        id: id,
        status: status
    };

    var json = JSON.stringify(data);
    connection.send(json);
}

function enablePWM(id, status) {
    var data = {
        device: "ESP32",
        command: "enablePWM",
        id: id,
        status: status
    };

    var json = JSON.stringify(data);
    connection.send(json);
}

function sendPwm(event, id) {
    var inputElement = event.target;
    var Tpwm = document.getElementById('slider-Tpwm-1').value;
    var Duty = document.getElementById('slider-Duty-1').value;

    if (inputElement.name == 'slider-Tpwm' || inputElement.name == 'slider-text-Tpwm') {
        Tpwm = inputElement.value;
        updateSliderText('Tpwm-', id, Tpwm);
    } else if (inputElement.name == 'slider-Duty' || inputElement.name == 'slider-text-Duty') {
        Duty = inputElement.value;
        updateSliderText('Duty-', id, Duty);
    }

    var data = {
        device: "ESP32",
        command: "setPWM",
        id: id,
        Tpwm: Tpwm,
        Duty: Duty
    };

    var json = JSON.stringify(data);
    connection.send(json);
}

function sendDAC(id, Dac) {
    updateSliderText('DAC-', id, Dac);
    var data = {
        device: "ESP32",
        command: "setDAC",
        id: id,
        DAC: Dac
    };

    var json = JSON.stringify(data);
    connection.send(json);
}

function updateGPIO(id, status) {
    console.log('Funcion updateGPIO. id:' + id + ' Status: ' + status);

    var label = document.getElementById('input-label-GPIO' + id);
    label.textContent = status;

    if (status == 'ON') {
        label.classList.add('On-style');
        label.classList.remove('Off-style');
    } else {
        label.classList.add('Off-style');
        label.classList.remove('On-style');
    }
}

function updateDATA(pHData, tempData, modeData) {
    document.getElementById('input-label-pH').textContent = pHData;
    document.getElementById('input-label-temp').textContent = tempData;
    document.getElementById('input-label-mode').textContent = modeData;
}

function updateSliderText(tipo, id, value) {
    document.getElementById('slider-' + tipo + id).value = value;
    document.getElementById('slider-text-' + tipo + id).value = value;
}