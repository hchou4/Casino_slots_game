/*****************************************************************//**
 * @file main_video_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "vga_core.h"
#include "sseg_core.h"
#include "ps2_core.h"
#include<stdio.h>
#include "ddfs_core.h"
#include "adsr_core.h"


/**
 * play primary notes with ddfs
 * @param adsr_p pointer to adsr core
 * @param ddfs_p pointer to ddfs core
 * @note: music tempo is defined as beats of quarter-note per minute.
 *        60 bpm is 1 sec per quarter note
 *
 */
void adsr_check(AdsrCore *adsr_p, GpoCore *led_p, GpiCore *sw_p) {
	const int melody[] = { 0, 2, 4, 5, 7, 9, 11 };
	int i, oct;

	adsr_p->init();
	// no adsr envelop and  play one octave
	adsr_p->bypass();
	for (i = 0; i < 7; i++) {
		led_p->write(bit(i));
		adsr_p->play_note(melody[i], 3, 500);
		sleep_ms(500);
	}
	adsr_p->abort();
	sleep_ms(1000);
	// set and enable adsr envelop
	// play 4 octaves
	adsr_p->select_env(sw_p->read());
	for (oct = 3; oct < 6; oct++) {
		for (i = 0; i < 7; i++) {
			led_p->write(bit(i));
			adsr_p->play_note(melody[i], oct, 500);
			sleep_ms(500);
		}
	}
	led_p->write(0);
	// test duration
	sleep_ms(1000);
	for (i = 0; i < 4; i++) {
		adsr_p->play_note(0, 4, 500 * i);
		sleep_ms(500 * i + 1000);
	}
}
/**
 * play primary notes with ddfs
 * @param ddfs_p pointer to ddfs core
 * @note: music tempo is defined as beats of quarter-note per minute.
 *        60 bpm is 1 sec per quarter note
 * @note "click" sound due to abrupt stop of a note
 *
 */
void ddfs_check(DdfsCore *ddfs_p, GpoCore *led_p) {
	int i, j;
	float env;

	//vol = (float)sw.read_pin()/(float)(1<<16),
	ddfs_p->set_env_source(0);  // select envelop source
	ddfs_p->set_env(0.0);   // set volume
	sleep_ms(500);
	ddfs_p->set_env(1.0);   // set volume
	ddfs_p->set_carrier_freq(262);
	sleep_ms(2000);
	ddfs_p->set_env(0.0);   // set volume
	sleep_ms(2000);
	// volume control (attenuation)
	ddfs_p->set_env(0.0);   // set volume
	env = 1.0;
	for (i = 0; i < 1000; i++) {
		ddfs_p->set_env(env);
		sleep_ms(10);
		env = env / 1.0109; //1.0109**1024=2**16
	}
	// frequency modulation 635-912 800 - 2000 siren sound
	ddfs_p->set_env(1.0);   // set volume
	ddfs_p->set_carrier_freq(635);
	for (i = 0; i < 5; i++) {               // 10 cycles
		for (j = 0; j < 30; j++) {           // sweep 30 steps
			ddfs_p->set_offset_freq(j * 10);  // 10 Hz increment
			sleep_ms(25);
		} // end j loop
	} // end i loop
	ddfs_p->set_offset_freq(0);
	ddfs_p->set_env(0.0);   // set volume
	sleep_ms(1000);
}

int count_digit(int number) {
	int count = 0;
	while(number != 0) {
		number = number / 10;
		count++;
	}
	return count;
}

/*
* Award payout based on the result of first_slot, second_slot, third_slot
 * @param current_money current money is passing by reference
 * @notes The payout rules is also listed on Github page
 */

void Display_buttons(OsdCore *osd_p, int reverse)
{
	if(!reverse)
	{
		osd_p->wr_char(41, 18, '+');
		osd_p->wr_char(42, 18, '1');
		osd_p->wr_char(43, 18, 'C');


		osd_p->wr_char(53, 18, 'M');
		osd_p->wr_char(54, 18, 'A');
		osd_p->wr_char(55, 18, 'X');

		osd_p->wr_char(64, 18, 'S');
		osd_p->wr_char(65, 18, 'P');
		osd_p->wr_char(66, 18, 'I');
		osd_p->wr_char(67, 18, 'N');
	}
	else
	{
		osd_p->wr_char(41, 18, '+',1);
		osd_p->wr_char(42, 18, '1',1);
		osd_p->wr_char(43, 18, 'C',1);


		osd_p->wr_char(53, 18, 'M',1);
		osd_p->wr_char(54, 18, 'A',1);
		osd_p->wr_char(55, 18, 'X',1);

		osd_p->wr_char(64, 18, 'S',1);
		osd_p->wr_char(65, 18, 'P',1);
		osd_p->wr_char(66, 18, 'I',1);
		osd_p->wr_char(67, 18, 'N',1);
	}

}

/*
* Award payout based on the result of first_slot, second_slot, third_slot
 * @param current_money current money is passing by reference
 * @notes The payout rules is also listed on Github page
 */
void Payout_rules(int& current_money, int credit,int first_slot, int second_slot, int third_slot )
{
	enum
	{
		BlANK = 0x0,
		BAR = 0x1,
		Brown = 0x3,
		BAR2 = 0x5,
		HORSESHOE = 0x7,
		BAR3 = 0x9,
		Silver = 0xb,
		Gold = 0xd
	};


	if( (first_slot == Gold && second_slot==Gold) ||
			(first_slot == Gold &&  third_slot== Gold )||
			(second_slot == Gold &&   third_slot== Gold) )
	{

		if(first_slot == Gold && second_slot==Gold && third_slot== Gold)
		{

			current_money += 150*credit;
		}else
		{
			current_money += 10*credit;
		}
		return;



	}else if( (first_slot == Silver && second_slot==Silver) ||
			(first_slot == Silver &&  third_slot== Silver)||
			(second_slot == Silver &&   third_slot== Silver) )
	{

		if(first_slot == Silver && second_slot==Silver && third_slot== Silver)
		{
			current_money += 100*credit;
		}else
		{
			current_money += 10*credit;
		}
		return;

	}else if( (first_slot == Brown && second_slot==Brown) ||
			(first_slot == Brown &&  third_slot== Brown) ||
			(second_slot == Brown &&   third_slot== Brown) )
	{

		if(first_slot == Brown && second_slot==Brown && third_slot== Brown)
		{
			current_money += 50*credit;
		}else
		{
			current_money += 10*credit;
		}
		return;

	}else if( (first_slot == BAR3 && second_slot==BAR3) ||
			(first_slot == BAR3 &&  third_slot== BAR3)||
			(second_slot == BAR3 &&   third_slot== BAR3) )
	{

		if(first_slot == BAR3 && second_slot==BAR3 && third_slot== BAR3)
		{
			current_money += 30*credit;

		}else
		{
			current_money += 10*credit;
		}
		return;
	}else if( (first_slot == BAR2 && second_slot==BAR2) ||
			(first_slot == BAR2 &&  third_slot== BAR2) ||
			(second_slot == BAR2 &&   third_slot== BAR2) )
	{

		if(first_slot == BAR2 && second_slot==BAR2 && third_slot== BAR2)
		{
			current_money += 25*credit;
		}else
		{
			current_money += 10*credit;
		}
		return;
	}else if( (first_slot == BAR && second_slot==BAR) ||
			(first_slot == BAR &&  third_slot== BAR) ||
			(second_slot == BAR &&   third_slot== BAR) )
	{

		if(first_slot == BAR && second_slot==BAR && third_slot== BAR)
		{
			current_money += 20*credit;
		}else
		{
			current_money += 10*credit;
		}
		return;
	}

	if(first_slot == HORSESHOE || second_slot==HORSESHOE || third_slot== HORSESHOE)
	{
		if( (first_slot == HORSESHOE && second_slot==HORSESHOE) ||
				(first_slot == HORSESHOE &&  third_slot== HORSESHOE) ||
				(second_slot == HORSESHOE &&   third_slot== HORSESHOE) )
		{
			if (first_slot == HORSESHOE && second_slot==HORSESHOE && third_slot== HORSESHOE)
			{
				current_money += 15*credit;
			}else
			{
				current_money += 10*credit;
			}
		}
		else
		{
			current_money += 5*credit;
		}
	}



}
/*
* play specific tones based on sel value. There is 4 preset tones.
 * @param adsr_p pointer to adsr core
 */
void Play_tones(AdsrCore *adsr_p, int sel)
{
	enum
	{
		CreditAndMax =0,
		Spin =1,
		winning =2,
		losing =3

	};


	switch (sel)
	{
	case CreditAndMax:
		adsr_p->play_note(6, 5, 500);
		sleep_ms(100);
		adsr_p->play_note(6, 5, 500);
		sleep_ms(100);
		adsr_p->play_note(6, 5, 500);
		sleep_ms(100);
		adsr_p->abort();
		break;
	case Spin:
		adsr_p->play_note(0, 5, 1000);
		sleep_ms(100);
		adsr_p->play_note(1, 5, 1000);
		sleep_ms(100);
		adsr_p->play_note(2, 5, 1000);
		sleep_ms(100);
		adsr_p->abort();
		break;
	case winning:
		adsr_p->play_note(5, 5, 1000);
		sleep_ms(100);
		adsr_p->play_note(4, 5, 1000);
		sleep_ms(100);
		adsr_p->play_note(0, 5, 1000);
		sleep_ms(100);
		adsr_p->abort();
		break;
	case losing:
		adsr_p->play_note(0, 5, 1000);
		sleep_ms(100);
		adsr_p->play_note(4, 5, 1000);
		sleep_ms(100);
		adsr_p->play_note(5, 5, 1000);
		sleep_ms(100);
		adsr_p->abort();

		break;
	default:
		adsr_p->play_note(6, 5, 500);
		sleep_ms(100);
		adsr_p->play_note(6, 5, 500);
		sleep_ms(100);
		adsr_p->play_note(6, 5, 500);
		sleep_ms(100);
		adsr_p->abort();

		break;
	}
}

/*
 * Generate random value between 0 to 14 for first, second, and third_slot
 * @param all parameter are passing by reference
 */
void generate_random(int &first_slot, int &second_slot, int &third_slot )
{
	first_slot = rand() % 14;
	second_slot = rand() % 14;
	third_slot = rand() % 14;
}

/**
 * This is the Main UI of the game. This is responsible for user mouse input, buttons, animation generation,etc
 * @param ghost_p pointer to mouse sprite instance
 * @param *mouse_p refer to SpriteCore
 * @param *ps2_p refers to the Ps2Core,
 * @param *ghost_p refers to the SpriteCore ,
 * @param  *frame_p refers to the FrameCore,
 * @param  *osd_p refers to the OsdCore,
 * @param  *adsr_p refers to the AdsrCore
 */

void moue_button(SpriteCore *mouse_p,Ps2Core *ps2_p ,SpriteCore *ghost_p,FrameCore *frame_p,OsdCore *osd_p,AdsrCore *adsr_p ) {
	enum
	{
		CreditAndMax =0,
		Spin =1,
		winning =2,
		losing =3

	};

	int first_slot;
	int second_slot;
	int third_slot;

	static const int ymax = 480;
	static const int xmax = 640;
	int id;
	int lbtn, rbtn, xmov, ymov;
	int current_money = 1000;
	char Money_max[8];
	int credit =0;
	char credit_ch;

	int b2_x1,  b2_x2;
	int b3_x1,  b3_x2;

	int x_begin = 314 ;
	int x_end  = 372;
	int y_begin = 284;
	int y_end = 312;

	b2_x1 = 406;
	b2_x2 = 461;

	b3_x1 = 498;
	b3_x2 = 549;



	int tmp;
	int x, y;
	y = ymax/2;
	x = xmax/2;

	adsr_p->init();
	adsr_p->bypass();
	adsr_p->abort();


	//set mouse cursor and ghost cursor at middle of the screen
	mouse_p->move_xy(x,y);
	ghost_p->move_xy(x, y);
	mouse_p->bypass(0);

	// clear top and bottom lines
	for (int i = 0; i < 32; i++) {
		mouse_p->wr_mem(i, 0);
		mouse_p->wr_mem(31 * 32 + i, 0);
	}



	id = ps2_p->init();	//select physical mouse

	do {
		if (id == 2) {  // mouse
			if (ps2_p->get_mouse_activity(&lbtn, &rbtn, &xmov, &ymov )) {


				//current money counter and display text
				ghost_p->wr_ctrl(0x10);	//turn orange
				osd_p->wr_char(40, 21, 'B',1);
				osd_p->wr_char(41, 21, 'A',1);
				osd_p->wr_char(42, 21, 'L',1);
				osd_p->wr_char(43, 21, 'A',1);
				osd_p->wr_char(44, 21, 'N',1);
				osd_p->wr_char(45, 21, 'C',1);
				osd_p->wr_char(46, 21, 'E',1);


				osd_p->wr_char(48, 21, '$');
				if(current_money<0) // min =0
				{
					current_money =0;
				}

				if(current_money >9999999)
				{
					current_money =9999999;
				}
				tmp= count_digit(current_money) ;
				sprintf(Money_max, "%d", current_money);

				for(int i =0; i<tmp; i++)
				{
					osd_p->wr_char(49 +i, 21,Money_max[i] );
				}

				Display_buttons(osd_p,0);

				credit_ch = credit +'0';
				osd_p->wr_char(48, 22, credit_ch);

				osd_p->wr_char(40, 22, 'C',1);
				osd_p->wr_char(41, 22, 'R',1);
				osd_p->wr_char(42, 22, 'E',1);
				osd_p->wr_char(43, 22, 'D',1);
				osd_p->wr_char(44, 22, 'I',1);
				osd_p->wr_char(45, 22, 'T',1);
				//current money counter and display text


				//3 buttons drawing
				int x1 = x_begin;
				int x2 = x_end;
				int y1 = y_begin;
				int y2 = y_end;
				int color = 0x00f; // blue

				frame_p->draw_rect(x1, y1, x2, y2, color);
				frame_p->draw_rect(b2_x1, y1, b2_x2, y2, color);
				frame_p->draw_rect(b3_x1, y1, b3_x2, y2, color);



				//first button +1C
				if((x>= x_begin && x<=x_end && y>= y_begin && y<= y_end))
				{
					if(lbtn)
					{
						Display_buttons(osd_p,1);	//Create a visual feedback

						Play_tones(adsr_p, CreditAndMax); // play the CreditandMax tone

						//game logic
						if(credit<3)
						{
							current_money -= 5;
							credit +=1;
						}
						adsr_p->abort();

						osd_p->clr_screen();

					}

				}


				//Second button Max
				if((x>= b2_x1 && x<=b2_x2 && y>= y_begin && y<= y_end))
				{
					if(lbtn)
					{
						Display_buttons(osd_p,1);	//Create a visual feedback
						Play_tones(adsr_p, CreditAndMax);// play the CreditandMax tone

						//game logic
						if(credit <3)
						{
							while (credit!=3)
							{
								credit ++;
								current_money -=5;
							}
						}
						osd_p->clr_screen();

					}
				}


				//Third button SPIN
				if((x>= b3_x1 && x<=b3_x2 && y>= y_begin && y<= y_end))
				{
					if(lbtn)
					{

						Display_buttons(osd_p,1);	//Create a visual feedback
						Play_tones(adsr_p, Spin);	// play the Spin tone
						int tmp3 = current_money;

						//Generate random number between 0 to 14
						generate_random(first_slot,second_slot, third_slot );
						//Awards payout based on those values generated.
						Payout_rules(current_money, credit,first_slot, second_slot, third_slot );


						sleep_ms(300);

						//Write those value of first, second and third slot to the screen
						//Notes: Value 10 = ':', 11 = ';' ,12 = '<', and 13 = '='
						//This is based on ASCII values
						//These are intended to be replace by reel sprite
						osd_p->wr_char(43, 14,first_slot + '0' );
						osd_p->wr_char(55, 14,second_slot+ '0' );
						osd_p->wr_char(66, 14,third_slot + '0' );


						//Ghost animation
						//Notes: This is intend to display a reel sprite but it was not working
						ghost_p->wr_ctrl(0x1c);


						sleep_ms(1000);

						//Play winning or losing tone
						if(tmp3 != current_money) //lose
						{
							Play_tones(adsr_p, losing);
						}else
						{
							Play_tones(adsr_p, winning);
						}

						credit =0;


						int address = 0x10;
						ghost_p->wr_ctrl(0x11);	//turn blue animation
						ghost_p->wr_ctrl(address);	//turn orange

					}
				}


				//Handle Mouse cursor
				x = x + xmov;
				y=  y - ymov;

				if(y<=0)
				{
					y = 0;
				}

				if(x<=0)
				{
					x=0;
				}

				if(x>=(xmax -10)) 	//-10pixel to allow the cursor not disappear
				{
					x= (xmax -10);
				}

				if(y>=(ymax -10))			//-10pixel to allow the cursor not disappear
				{
					y= (ymax -10);
				}

				mouse_p->move_xy(x,y);


			}   // end get_mouse_activitiy()
		}
	} while (1);

	sleep_ms(3000);
}


// external core instantiation
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
FrameCore frame(FRAME_BASE);
GpvCore bar(get_sprite_addr(BRIDGE_BASE, V7_BAR));
GpvCore gray(get_sprite_addr(BRIDGE_BASE, V6_GRAY));
SpriteCore ghost(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
SpriteCore mouse(get_sprite_addr(BRIDGE_BASE, V1_MOUSE), 1024);
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));



Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
DdfsCore ddfs(get_slot_addr(BRIDGE_BASE, S12_DDFS));
AdsrCore adsr(get_slot_addr(BRIDGE_BASE, S13_ADSR), &ddfs);
int main() {

	//turn off led
	unsigned int i =0;
	for(; i<8; i++)
	{
		sseg.write_1ptn(0xFFFF, i);
		sseg.set_dp(0);
	}


	while (1) {
		moue_button(&mouse,&ps2, &ghost,&frame,&osd,&adsr);
	} // while
} //main
