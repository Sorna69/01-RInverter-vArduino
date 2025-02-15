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
	// Los mensajes JSon UpdateGPIO y UpdateDAYA son ENVIADOS DESDE API del ESP32 HACIA bakcend (servidor web)
	// Los mensajes sendGPIO y sendPWM son ENVIADOS desde backend (aqui) HACIA API del ESP32 y este devuelve un echo!

	json = JSON.parse(data)

	if (json.command == 'updateGPIO') {
		console.log ('Process updateGPIO. id:' +json.id + ' Status: ' +json.status);
		updateGPIO(json.id, json.status);
	}
	else if (json.command == 'updateDATA') {
		updateDATA(json.pHData, json.tempData, json.modeData);
	}

	else {
		console.log ('No se reconoce el mesaje');
	}
}

function sendGPIO(id, status) {

	console.log("Function sedGPIOESP32Mod");

	// Código adicional, para esperar confirmacion
	// Cuando se envia un comando para activar/desactivar un PIN se deshabilita el switch (hasta recibir confirmacion)
	// Esto no lo hago bien, creo que tiene que ver con como selecciono la propiedad del elemento Query?
	//document.getElementById('output-switch-' + id).disabled = true;

	// Código original, sin añadir control respuesta
	let data = {
		device: "ESP32",
		command: "setGPIO",
		id: id,
		status: status
	}

	let json = JSON.stringify(data);
	connection.send(json);
}

function enablePWM(id, status) {

	console.log("Function sedGPIOESP32Mod");

	// Código adicional, para esperar confirmacion
	// Cuando se envia un comando para activar/desactivar un PIN se deshabilita el switch (hasta recibir confirmacion)
	// Esto no lo hago bien, creo que tiene que ver con como selecciono la propiedad del elemento Query?
	//document.getElementById('output-switch-' + id).disabled = true;

	// Código original, sin añadir control respuesta
	let data = {
		device: "ESP32",
		command: "enablePWM",
		id: id,
		status: status
	}

	let json = JSON.stringify(data);
	connection.send(json);
}

function updateGPIO(id, status) {
	console.log ('Funcion updateGPIO. id:' +id + ' Status: ' +status);

	// Cuando se recibe el echo se vuelve a activar el boton/checkbox
	// Esto tampo lo hago bien, creo que tiene que ver con como selccionoolas propiedades del Switch
	//document.getElementById('output-switch-' + id).disabled = false;
	//document.getElementById('output-switch-' + id).enabled = status;

	// y se actualiza el estado y contenido de la etiqueta
	document.getElementById('input-label-GPIO' + id).textContent = status;

	// Igual el atributo disable y enable del switch esta dentro de la clase de swithc
	if (status == 'ON') {
		document.getElementById('input-label-GPIO' + id).classList.add('On-style');
		document.getElementById('input-label-GPIO' + id).classList.remove('Off-style');
	}
	else {
		document.getElementById('input-label-GPIO' + id).classList.add('Off-style');
		document.getElementById('input-label-GPIO' + id).classList.remove('On-style');
	}
	
}


function sendPwm(event, id) {
	var inputElement = event.target;
	// Inicialización de los valores de Tpwm y Duty
	const Tpwm = document.getElementById('form-pwm-'+id).elements["slider-Tpwm"].value;
	const Duty = document.getElementById('form-pwm-'+id).elements["slider-Duty"].value;

	if (inputElement.name == 'slider-Tpwm' || inputElement.name == 'slider-text-Tpwm'){
		const Tpwm = inputElement.value
		updateSliderText('Tpwm-', id, Tpwm);
	}
	else if (inputElement.name == 'slider-Duty' || inputElement.name == 'slider-text-Duty') {
		const Duty = inputElement.value
		console.log('if')
		updateSliderText('Duty-', id, Duty);
	}

	Timer_value = Tpwm*160;
	let data = {
		device: "ESP32",
		command: "setPWM",
		id: id,
		Tpwm: Timer_value,
		Duty: Duty
	}

	let json = JSON.stringify(data);
	connection.send(json);
}


// ESTO ESTÁ SIN IMPLEMENTAR DEL TODO
// HAy que ver como se traducen el Duty Cyle a valor de lo que sea
function sendDAC(id, Dac) {
	updateSliderText('DAC-', id, Dac);
	let data = {
		device: "ESP32",
		command: "setDAC",
		id: id,
		//SIN COMPLETAR
		DAC: Dac
	}

	let json = JSON.stringify(data);
	//connection.send(json);
}

/** 
 function sendPwm(id, Tpwm, Duty) {
	// No estoy actualizando bien los valores de Slider y Text. Pero no tiene sentido que no fucniones
	updateSliderText('pwm-', id, Tpwm);
	updateLabel('pwm-', id, Tpwm);
	//console.log('WebSocket Error', error);

	Timer_value = Tpwm*160;
	let data = {
		device: "ESP32",
		command: "setPWM",
		id: id,
		Tpwm: Timer_value,
		Duty: Duty
	}

	let json = JSON.stringify(data);
	connection.send(json);
}

function sendTpwm(id, Tpwm) {
	updateSliderText('pwm-', id, Tpwm);
	updateLabel('pwm-', id, Tpwm);

	Timer_value = Tpwm*160;
	let data = {
		device: "ESP32",
		command: "setPWM",
		id: id,
		pwm: Timer_value
	}

	let json = JSON.stringify(data);
	connection.send(json);
}

// ESTO ESTÁ SIN IMPLEMENTAR DEL TODO
// HAy que ver como se traducen el Duty Cyle a valor de lo que sea
function sendDuty(id, Duty) {
	updateSliderText('duty-', id, Duty);
	//Timer_value = Tpwm*160;
	let data = {
		device: "ESP32",
		command: "setDuty",
		id: id,
		//SIN COMPLETAR
		Duty: Duty
	}

	let json = JSON.stringify(data);
	connection.send(json);
}
*/

function updateDATA(pHData, tempData, modeData) {
	document.getElementById('input-label-pH').textContent = pHData;
	document.getElementById('input-label-temp').textContent = tempData;
	document.getElementById('input-label-mode').textContent = modeData;
}

function updateSliderText(tipo, id, value) {
	console.log('slider-' +tipo + id);
	document.getElementById('slider-' +tipo + id).value = value;
	document.getElementById('slider-text-' +tipo + id).value = value;
}

function updateLabel(tipo, id, value) {
	document.getElementById('input-label-' +tipo + id).textContent = Math.round(1000000/value);
}