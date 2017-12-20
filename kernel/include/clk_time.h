#ifndef _TIME_H_
#define _TIME_H_

#define TIMER_PORT 0x40
#define FREQ_8253 1193182
#define HZ 100

void init_timer(){
		int counter = FREQ_8253 / HZ;
			//assert(TIMER_PORT < 65536);
		out_byte(TIMER_PORT + 3, 0x34);
		out_byte(TIMER_PORT + 0, counter % 256);
		out_byte(TIMER_PORT + 0, counter / 256);
}
			
#endif
			
