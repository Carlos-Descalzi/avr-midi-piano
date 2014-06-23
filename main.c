#include <avr/io.h>
#include <util/delay.h>

/*
 * PINOUT
 *
 *	PORTC 0-2: Output: Line selector
 *	PORTA 0-7: Inputs
 *	PORTD: Output - MIDI Output
 *
 *
 */

#define MSG_NOTE_ON     0x90
#define MSG_NOTE_OFF    0x80

#define NUM_INPUTS      3
#define BUFF_SIZE       24

#define PIN(p,n,v)      (v ? (p |= 1 << n) : (p &=~(1 << n)))
#define PINON(p,n)      (p |= 1 << n)
#define PINOFF(p,n)     (p &= ~(1 << n))

#define BIT_LENGTH      32

#define MIDIOUT			PORTD
#define LINESEL			PORTC
#define INPUT			PINA

static int buffer_pointer;
static unsigned char inputs			[NUM_INPUTS];
static unsigned char output_buffer	[BUFF_SIZE];

static void 		setup		(void);
static void 		read_inputs	(void);
static void 		send_midi	(void);
static void 		send_msg	(unsigned char data);

#define	OFFSET	65

static char notas[] = {
	OFFSET + 6, 
	OFFSET + 7, 
	OFFSET + 8, 
	OFFSET + 9, 
	OFFSET + 10,
	OFFSET + 11,

	OFFSET + 12,
	OFFSET + 13,
	OFFSET + 14,
	OFFSET + 15,
	OFFSET + 16,
	OFFSET + 17,
	OFFSET + 18,
	OFFSET + 19,

	OFFSET + 20,
	OFFSET + 21,
	OFFSET + 22,
	OFFSET + 23,
	OFFSET + 24,
	OFFSET + 25,
	OFFSET + 26,
	OFFSET + 27,
	OFFSET + 28,
	OFFSET + 29
};

int main(void) {
    setup();

    while(1) {
        read_inputs();
        send_midi();
    }

    return 0;
}

static void read_inputs(){
    unsigned char i,j,c,p,b;
    buffer_pointer = 0;
    for (i=0;i<NUM_INPUTS;i++){
        LINESEL = ~(1 << i);
        c = ~INPUT;
        p = inputs[i];
        if (c != p){
            for (j=0;j<=7;j++){
                b = 1 << j;
                if ((c & b) && !(p & b)){
                    output_buffer[buffer_pointer++] = 0x80 | notas[(i << 3) | j];
                } else if (!(c & b) && (p & b)){
                    output_buffer[buffer_pointer++] = notas[(i << 3) | j];
                }
            }
        }
        inputs[i] = c;
    }
}

static void send_midi(){
    unsigned char i,c;
    for (i=0;i<buffer_pointer;i++){
        c = output_buffer[i];
        if (c & 0x80){ // pressed
            send_msg(MSG_NOTE_ON);
            send_msg(c & 0x7F);
            send_msg(100);
        } else { // released
            send_msg(MSG_NOTE_OFF);
            send_msg(c & 0x7F);
            send_msg(100);
        }
    }
}

static void send_msg(unsigned char data){
    unsigned char i;

    MIDIOUT = 0; // Start bit
    _delay_us(BIT_LENGTH);
    for (i=0;i<8;i++){
        MIDIOUT = data & (1 << i) ? 1 : 0;
        _delay_us(BIT_LENGTH);
    }
    MIDIOUT = 1; // Start bit
    _delay_us(BIT_LENGTH); // Stop bit
}

static void setup(){
	int i;
	DDRA = 0x00;
	PORTA = 0xFF;
    DDRC = 0x07;
    DDRD = 0x01;
    MIDIOUT = 1;
	buffer_pointer = 0;
	for (i=0;i<NUM_INPUTS;i++){
		inputs[i] = 0;
	}
}
