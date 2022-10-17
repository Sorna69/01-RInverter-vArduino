var connection = new WebSocket('ws://' + location.hostname + '/ws', ['arduino']);

const processReceived = (evento) => {
	// Los mensajes JSon UpdateGPIO y UpdateDAYA son ENVIADOS DESDE API del ESP32 HACIA bakcend (servidor web)
	// Los mensajes sendGPIO y sendPWM son ENVIADOS desde backend (aqui) HACIA API del ESP32 y este devuelve un echo!

	json = JSON.parse(evento.data)

	// CON VUE?? SE PODRÍA HACER UN DATA CON LOS STATUS DE TODOS LOS ELEMENTSO?? (SERVIVIA UN JSON IGUALMNTE)
	if (json.command == 'statusGPIO') {
		console.log('statusGPIO. id:' + json.id + ' Status: ' + json.status);
		updateGPIO(json.id, json.status);
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
	console.log('WebSocket connection closed');
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

function updateGPIO(id, status) {
	console.log('Funcion updateGPIO. id:' + id + ' Status: ' + status);

	// Cuando se recibe el echo se vuelve a activar el boton/checkbox
	// Esto tampo lo hago bien, creo que tiene que ver con como selccionoolas propiedades del Switch

	//document.getElementById('output-switch-' + id).disabled = false;


	//document.getElementById('output-switch-' + id).enabled = true;
	//document.getElementById('output-switch-' + id).enabled = status;

	//document.getElementById('output-switch-' + id).status = status;
	//document.getElementById('output-switch-' + id).checked  = status;


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

function updateDATA(pHData, tempData, modeData) {
	document.getElementById('input-label-pH').textContent = pHData;
	document.getElementById('input-label-temp').textContent = tempData;
	document.getElementById('input-label-mode').textContent = modeData;
}


function sendGPIO(id, status) {

	console.log("Function sedGPIOESP32Mod");

	// Código adicional, para esperar confirmacion
	// Cuando se envia un comando para activar/desactivar un PIN se deshabilita el switch (hasta recibir confirmacion)
	// Esto no lo hago bien, creo que tiene que ver con como selecciono la propiedad del elemento Query?

	//document.getElementById('output-switch-' + id).disabled = true;

	// Código original, sin añadir control respuesta
	let data = {
		command: "setGPIO",
		id: id,
		status: status
	}

	let json = JSON.stringify(data);
	connection.send(json);
}

function enablePWM(status) {
	// Reescribir añadiendo const id como parámetro (ID del PWM)--> Funcion genérica para todos PWMs
	console.log("Function sedGPIOESP32Mod");

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
}


function updateSliderText(id, value) {
	document.getElementById('slider-pwm-' + id).value = value;
	document.getElementById('slider-text-pwm-' + id).value = value;
	document.getElementById('input-label-Freq').textContent = Math.round(1000000 / value);
}