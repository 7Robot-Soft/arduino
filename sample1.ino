// Constantes
const int buttonPin = 2;
const int ledPin = 13;
const int boardId = 5;

// Variables
int buttonState = 1;
int ledState = 1;

// Variables liees a la lectures des packets
int packetState = 1;
int packetId = 0;
int packetDataType = 0;
int packetData[16];
int packetDataLen = 0;
int packetDataPtr = 0;

int pending = 0;

/* Tableaux pour les stocker les arguments des packets */
// unsigned char
unsigned char ucharv[8];
int ucharc = 0;
// float
float floatv[8];
int floatc = 0;

// Fonctions d'envoi de donnees
void sendPacket(int id);
void sendUByte(int id, int value);
void sendBoardId();

// Lorsqu'un packet est recu
void processPacket()
{
	if (packetId == 254) {
		sendBoardId();
	} else if (packetId == 200) {
		digitalWrite(ledPin, LOW);
	} else if (packetId == 201) {
		digitalWrite(ledPin, HIGH);
	} else if (packetId == 150) {
		buttonState = digitalRead(buttonPin);
		sendPacket(100+buttonState);
	}
}


void setup()
{
	Serial.begin(115200);
	pinMode(buttonPin, INPUT);
	digitalWrite(buttonPin, HIGH);
	pinMode(ledPin, OUTPUT);
	packetState = 1;
	while (Serial.read() != -1) {}
	sendBoardId();
}

void loop()
{
	if (Serial.available() > 0) {
		pending = Serial.read();
		switch (packetState) {
			case 1:
				if (pending == 129) {
					packetState = 2;
					ucharc = 0;
				}
				break;
			case 2:
				packetId = pending;
				packetState = 3;
				break;
			case 3:
				if (pending == 128) {
					processPacket();
					packetState = 1;
				} else {
					packetDataType = pending;
					packetState = 4;
					packetDataLen = pending & 0b1111;
					packetDataPtr = 0;
				}
				break;
			case 4:
				packetData[packetDataPtr++] = pending;
				if (packetDataPtr == packetDataLen) {
					if (packetDataType == 1) {
						ucharv[ucharc++] = pending;
					}
					packetState = 3;
				}
		}
	}
	
	pending = digitalRead(buttonPin);
	if (pending != buttonState) {
		buttonState = pending;
		sendPacket(100 + buttonState);
	}
}

// Envoyer un packet sans argument
void sendPacket(int id)
{
	Serial.write(129);
	Serial.write(id);
	Serial.write(128);
}

// Envoyer un packet avec un unsigned char en argument
void sendUByte(int id, int value)
{
	Serial.write(129);
	Serial.write(id);
	Serial.write(1);
	Serial.write(value);
	Serial.write(128);	
}

// Envoyer l'identifiant de la carte
void sendBoardId()
{
	sendUByte(255, boardId);
}
