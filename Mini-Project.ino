volatile int Digits[] = {10, 10, 10};
volatile int CurrentDigit = 0;
const int PWD = 147;


byte readOnce() {
    PORTD = 0b11111100;
    delay(2);
    switch ((PIND>>2)&0x0F) {
        case 0b0111: return 0x0C;
        case 0b1011: return 0x09;
        case 0b1101: return 0x06;
        case 0b1110: return 0x03;
        default: break;
    }
    
    PORTD = 0b01111100;
    delay(2);
    switch ((PIND>>2)&0x0F) {
        case 0b0111: return 0x00;
        case 0b1011: return 0x08;
        case 0b1101: return 0x05;
        case 0b1110: return 0x02;
        default: break;
    }
    
    PORTD = 0b10111100;
    delay(2);
    switch ((PIND>>2)&0x0F) {
        case 0b0111: return 0x0A;
        case 0b1011: return 0x07;
        case 0b1101: return 0x04;
        case 0b1110: return 0x01;
        case 0b1111: return 0x0F;
        default: break;
    }

    return 0xFF;
}

byte readKey(int debounceDelay=5) {
    byte finalRead;
    byte previousRead = readOnce();
    for (int i=0; i<debounceDelay; ++i) {
        delay(1);
        finalRead = readOnce();
        if (finalRead != previousRead) {
            i=0;
            previousRead = finalRead;
        }
    }
    return finalRead;
}

void showResult() {
    int num = Digits[0] + 10*Digits[1] + 100*Digits[2];
    if (num == PWD) {
        digitalWrite(13, HIGH);
        delay(1000);
        digitalWrite(13, LOW);
    }
    else {
        digitalWrite(13, HIGH);
        delay(200);
        digitalWrite(13, LOW);
        delay(200);
        digitalWrite(13, HIGH);
        delay(200);
        digitalWrite(13, LOW);
    }
}

ISR(TIMER2_OVF_vect) {
    const byte NUMERAL[] = {
        0b00111111, //0
        0b00000110, //1
        0b01011011, //2
        0b01001111, //3
        0b01100110, //4
        0b01101101, //5
        0b01111101, //6
        0b00000111, //7
        0b01111111, //8
        0b01101111, //9
        0b01000000, //-
    };

    PORTB = (PORTB | 0b11100) & ~(0b100<<CurrentDigit);
    PORTC = NUMERAL[Digits[CurrentDigit]];
    digitalWrite(8, NUMERAL[Digits[CurrentDigit]] & 0b01000000);
    CurrentDigit = (CurrentDigit+1) % 3;
    delayMicroseconds(1000);
}

void setup() {
    DDRB = 0xFF;
    DDRC = 0xFF;
    DDRD = 0b11000000;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;

    TCCR2A = 0;
    TCCR2B = 0b110;
    TIMSK2 = 1;
    sei();
}


void loop() {
    byte keyPressed = readKey();
    switch (keyPressed) {
        case 0x0C: 
            showResult();
        case 0x0A:
            for (int i=0; i<3; ++i) { Digits[i] = 10; }
            break;
        case 0x0F:
        case 0xFF:
            break;
        default:
            Digits[2] = Digits[1];
            Digits[1] = Digits[0];
            Digits[0] = keyPressed;
    }
    PORTD = 0b00111100;
    while(((PIND>>2)&0x0F) != 0b1111);
}
