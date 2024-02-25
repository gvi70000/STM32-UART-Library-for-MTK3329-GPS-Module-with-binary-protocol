#include "MTK3329.h"

union long_union
{
	int32_t dword;
	uint8_t  byte[4];
} longUnion;

// Packet checksum accumulators
uint8_t _ck_a;
uint8_t _ck_b;

// State machine state
uint8_t _step;
uint8_t _payload_counter;

uint8_t MTK_buffer[MTK_MSG_L];

uint32_t GPS_timer;
//void gpsWait(uint8_t loops)
//{
//	uint8_t count = 0;
//	while(buffer_available() && (count<loops))
//	{
//		count++;
//		buffer_read();
//	}	
//}

int32_t Bytes2Long(unsigned char Buffer[])
{
	longUnion.byte[3] = *(Buffer+3);
	longUnion.byte[2] = *(Buffer+2);
	longUnion.byte[1] = *(Buffer+1);
	longUnion.byte[0] = *Buffer;
	return(longUnion.dword);
}

void mtk_init(void)
{
	//wait for GPS to start - we get the lines below
	//when MTK 3329 is ON we get  + 2 bytes for each line (CR and LF)
	//$PMTK011,MTKGPS*08
	//$PMTK010,001*2E	
	//gpsWait(37);
	delay_ms(2000);
	USART1_puts(MTK_SBAS_ON);// set 5Hz update rate
	//$PMTK313,1*2E
	//$PMTK001,313,3*31
	//gpsWait(34);
	delay_ms(1000);
	USART1_puts(MTK_WAAS_ON);// set 5Hz update rate
	//$PMTK301,2*2E
	//$PMTK001,301,3*32
	//gpsWait(34);
	delay_ms(1000);
	USART1_puts(MTK_OUTPUT_5HZ);// set 5Hz update rate
	//$PMTK001,220,3*30
	//gpsWait(19);
	delay_ms(1000);
	USART1_puts(MTK_SET_BINARY);// initialize serial port for binary protocol use
	//$PGACK,16,1*68
	//gpsWait(16);
	delay_ms(1000);
	GPS_timer = millis();//Restarting timer...
}


uint8_t mtk_read(mtk3329* GPS_Data)
{
	uint8_t data, fix;
	int16_t numc;
	uint8_t ErrCode = 1;
	uint8_t i = 0;
	
	//char buf[50];
	numc = buffer_available();
	if(numc > 31)
	{
		for (i = 0; i < numc; i++) // Process bytes received
		{        
			data = buffer_read();// read the next byte
			//sprintf(buf, "Data 0x%02X \r\n", data);
			//USART3_puts(buf);
			//if(_step)
			//{
			//	sprintf(buf, "Step %d \r\n", _step);
			//	USART3_puts(buf);
			//}
			restart:
			switch(_step)
			{
			// Message preamble, class, ID detection
			//
			// If we fail to match any of the expected bytes, we reset the state machine and re-consider the failed
			// byte as the first byte of the preamble.  This improves our chances of recovering from a mismatch
			// and makes it less likely that we will be fooled by the preamble appearing as data in some other message.
			// 
				case 0:
					if (data == PREAMBLE1_V16)
						_step++;
				break;
				case 1:
					if (data == PREAMBLE1)
						_step++;
					else
					{
						_step = 0;
						goto restart;
					}
				break;
				case 2:
					if (data == PREAMBLE2)
					{
						_step++;
						_ck_b = _ck_a = data;// reset the checksum accumulators
						_payload_counter = 0;
					}
					else
					{
						_step = 0;// reset and wait for a message of the right class
						goto restart;
					}
				break;
				// Receive message data
				case 3:
					MTK_buffer[_payload_counter++] = data;
					_ck_b += (_ck_a += data);
					if (_payload_counter == MTK_MSG_L) _step++;
				break;
					// Checksum and message processing
				case 4:
					_step++;
					if (_ck_a != data)
					{
						_step = 0;
						ErrCode = 2;
					}
				break;
				case 5:
					_step = 0;
					if (_ck_b != data)
					{
						ErrCode = 3;
					}
					else
					{
						fix = MTK_buffer[21];
						if(!fix)
						{
							ErrCode = 3;//No Fix
							break;
						}
						else
						{
							GPS_Data->Lattitude = Bytes2Long(&MTK_buffer[0]);//actualy the lat and long are *10^6 not by 10^7
							GPS_Data->Longitude = Bytes2Long(&MTK_buffer[4]);
							GPS_Data->Altitude = Bytes2Long(&MTK_buffer[8]);
							GPS_Data->Speed = Bytes2Long(&MTK_buffer[12]);		
							GPS_Data->Heading = Bytes2Long(&MTK_buffer[16]);
							GPS_Data->Sats = MTK_buffer[20];
							GPS_Data->Date = Bytes2Long(&MTK_buffer[22]);
							GPS_Data->UTC = Bytes2Long(&MTK_buffer[26])/100;//actualy the time is given by gps in HH MM SS MsMsMs, i need it in tens of second
							GPS_Data->HDOP = (MTK_buffer[31] << 8) + MTK_buffer[30];
							ErrCode = 0;
							GPS_timer = millis();
						}
					}
			}
		}
		if((millis() - GPS_timer) > 5000)
		{
			ErrCode = 4;//Time out (>5s) reinit GPS
			mtk_init();
		}
	}
	return ErrCode;	
}

