var connection = new WebSocket('ws://' + location.hostname + '/ws', ['arduino']);


// FUNCIÓN PARA GESTIONAR MENSAJES RECIBIDOS DESDE ESP32
const processReceived = (evento) => {
	// CON VUE?? SE PODRÍA HACER UN DATA CON LOS STATUS DE TODOS LOS ELEMENTSO?? (SERVIVIA UN JSON IGUALMNTE) 

	json = JSON.parse(evento.data)

	if (json.command == 'conectionOk') {
		// Cuando se recibe el comando de conectionOK del ESP32 se pide el valor de los estados iniciales
		requestIntialStatus();
	}
	else if (json.command == 'initialStatus') {
		console.log('initialStatus. id:' + json.id + ' Status: ' + json.status);
		initialStatus(json.id, json.status);
	}
	else if (json.command == 'statusGPIO') {
		console.log('(ProcessReceived) statusGPIO. id:' + json.id + ' Status: ' + json.status);
		updateGpioStatus(json.id, json.status);
	}
	else if (json.command == 'updateDATA') {
		updateDATA(json.pHData, json.tempData, json.modeData);
	}

	else {
		console.log('No se reconoce el mesaje');
	}
}

connection.onopen = function () {
	connection.send('Received from Client');
	console.log('Connected');
	//requestIntialStatus();
};

connection.onerror = function (error) {
	console.log('WebSocket Error', error);
};

connection.addEventListener('message', processReceived);

/*
connection.onmessage = function (e) {
	console.log('Received from server: ', e.data);
	processReceived(e.data);
};
*/


connection.onclose = function () {
	//console.log('WebSocket connection closed');
	//alert('WebSocket connection closed');
	desactivarControles();
};


/*
function processReceived(data) {
	// Los mensajes JSon UpdateGPIO y UpdateDAYA son ENVIADOS DESDE API del ESP32 HACIA bakcend (servidor web)
	// Los mensajes sendGPIO y sendPWM son ENVIADOS desde backend (aqui) HACIA API del ESP32 y este devuelve un echo!

	json = JSON.parse(data)

	if (json.command == 'updateGPIO') {
		console.log('Process updateGPIO. id:' + json.id + ' Status: ' + json.status);
		updateGPIO(json.id, json.status);
	}
	else if (json.command == 'updateDATA') {
		updateDATA(json.pHData, json.tempData, json.modeData);
	}

	else {
		console.log('No se reconoce el mesaje');
	}
}
*/


// FUNCIONES JS
function desactivarControles() {
	// Se DESHABILITAN los SWITCH. 2 Opciones: atributo HTML o funcion JS
	//document.querySelector("#label-output-switch-15").MaterialSwitch.disable();
	//for (let elemento of misSwitchesLab) { elemento.MaterialSwitch.disable()};
	const misSwitches = document.getElementsByClassName("mdl-switch__input");
	for (let elemento of misSwitches) { elemento.disabled = true };
}

function initialStatus(id, status) {
	console.log('Funcion initialStatus. id:' + id + ' Status: ' + status);

	// Se HABILITAN los controles (SWITCH). 2 Opciones: atributo HTML o funcion JS 
	//const misSwitches = document.querySelectorAll(".mdl-switch__input");
	//for (let elemento of misSwitches) { elemento.disabled = false };
	const misSwitchesLab = document.querySelectorAll(".mdl-switch")
	for (let elemento of misSwitchesLab) {elemento.MaterialSwitch.enable()};

	// PARA DESPUES, REVISAR TODOS LOS SIWTHC:
	/* for (let elemento of misSiwthc){
		if (index.statud == ON){elemento.MaterialSwitch.on();}
		else if (status == OFF){elemento.MaterialSwithc.off();}
	}
	*/
	// Igual el atributo disable y enable del switch esta dentro de la clase de switch
	if (status == 'ON') {
		//misSwitchesLab.MaterialSwitch.on();
		for (let elemento of misSwitchesLab) { elemento.MaterialSwitch.on() };

	}
	else {
		//misSwitchesLab.MaterialSwitch.off();
		for (let elemento of misSwitchesLab) { elemento.MaterialSwitch.off() };
	}
}

function updateGpioStatus(id, status) {
	console.log('Funcion updateGpioStatus. id:' + id + ' Status: ' + status);

	// Una vez se recibe la respuesta desde el esp32, se HABILITAN los controles (SWITCH). 2 Opciones: atributo HTML o funcion JS 
	//document.querySelector("#output-switch-" +id).disable = false;
	document.querySelector("#label-output-switch-" + id).MaterialSwitch.enable();
}

function updatePwmStatus(id, status) {
/* SE UTILIZARÁ PARA CONFIRMAR LOS VALORES DE FRECUENCIA Y PERIDOD, SI FUERE NECESARIO */
}

function updateDATA(pHData, tempData, modeData) {
	document.getElementById('input-label-pH').textContent = pHData;
	document.getElementById('input-label-temp').textContent = tempData;
	document.getElementById('input-label-mode').textContent = modeData;
}

function updateSliderText(id, value) {
	document.getElementById('slider-pwm-' + id).value = value;
	document.getElementById('slider-text-pwm-' + id).value = value;
	document.getElementById('input-label-Freq').textContent = Math.round(1000000 / value);
}

// COMANDOS ENVIADOS AL ESP32
function requestIntialStatus() {

	console.log("Commad: requestIntialStatus");

	// Código original, sin añadir control respuesta
	let data = {
		command: "requestIntialStatus"
	}

	let json = JSON.stringify(data);
	connection.send(json);
}

function sendGPIO(id,tipo,status) {

	console.log("Function setGPIOESP32Mod");

	// Se DESACTIVA el control (SWITCH) ASOCIADO. Al recibir confirmacion [Funcion updateGpioStatus] se ACTIVA nuevamente.
	document.querySelector("#label-output-switch-" + id).MaterialSwitch.disable();
	//document.querySelector("#output-switch-" + id).disabled = true;

	let data = {
		command: "setGPIO",
		id: id,
		tipo:tipo,
		status: status
	}

	let json = JSON.stringify(data);
	connection.send(json);
}

function enablePWM(status) {
	// Reescribir añadiendo const id como parámetro (ID del PWM)--> Funcion genérica para todos PWMs
	console.log("Function enablePWM");

	// Código adicional, para esperar confirmacion
	// Cuando se envia un comando para activar/desactivar un PIN se deshabilita el switch (hasta recibir confirmacion)
	// Esto no lo hago bien, creo que tiene que ver con como selecciono la propiedad del elemento Query?
	//document.getElementById('output-switch-' + id).disabled = true;

	// Código original, sin añadir control respuesta
	let data = {
		command: "enablePWM",
		status: status
	}

	let json = JSON.stringify(data);
	connection.send(json);
}

function sendTpwm(id, Tpwm) {
	updateSliderText(id, Tpwm);
	Timer_value = Tpwm * 160;
	let data = {
		command: "setPWM",
		id: id,
		pwm: Timer_value
	}

	let json = JSON.stringify(data);
	connection.send(json);

	// Se podría esperar un mensaje d actualización antes de modificar el slider
	//updateSliderText(id, Tpwm);
}