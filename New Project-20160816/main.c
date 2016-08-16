#include <stdio.h>
#include <string.h>
#include "PrntChar.h"

#define PERSIAN_INDEX	0x06
#define ENGLISH_INDEX	0x00
#define CHAR_MODE_UNNOWN	0
#define CHAR_MODE_SINGLE		1
#define CHAR_MODE_DUAL		2
#define CHAR_MODE_QUAD		3

#define CHAR_STATE_FIRST	0
#define CHAR_STATE_MIDDLE	1

#define CHAR_LAM			0xF3//0x44
#define CHAR_LA				0xF2	//	??
#define CHAR_ALEF_UTF		0x27
#define ARRAY_SIZE(foo) (sizeof(foo)/sizeof(foo[0]))

typedef unsigned char   bool;
typedef unsigned char   u08;
#	ifndef false
#		define false    0
#	endif
#	ifndef true
#		define true     1
#	endif
void ConvertUtfToThermal(u08* thrml, u08* utf, u08 uSize){
	if((utf&&thrml)&&(uSize>1)){
		u08 termalByte=0;
		u08 trmBuffer[0xFF];
		u08 charMode=CHAR_MODE_UNNOWN,
			preCharMode=CHAR_MODE_UNNOWN,
			charState=CHAR_STATE_FIRST;
		
		//select only char code:	H06	27
		//						^
		for (u08 i=0; i<uSize; i=i+2)
		{
			if (utf[i]==PERSIAN_INDEX)
			{
				thrml[termalByte]=utf[i+1];		//jump to next char after loop
				trmBuffer[termalByte++]=0;		//termalIndex increase here
			} 
			else if (utf[i]==ENGLISH_INDEX)
			{
				thrml[termalByte]=0;
				trmBuffer[termalByte++]=utf[i+1];	//termalIndex increase here
			}
		}
		bool	charDown=false,
				quadEndSticked=false;
		u08 preCharIndx=0;
		
		for (u08 i=0;i<termalByte;i++)
		{
			if(thrml[i])	//current char is Persian
			{
				charDown=false;
				
				for(u08 j=0;j<ARRAY_SIZE(PrntSingleMask);j++)	//  ????????
				{
					if (PrntSingleMask[j]==thrml[i])
					{
						preCharIndx=j;
						if (PrntSingleMask[j]==CHAR_ALEF_UTF)
						{
							if(charMode==CHAR_STATE_MIDDLE)
								thrml[i]=PrntSingleChar[j+1];
							if (thrml[i-1]==CHAR_LAM)
							{
								thrml[i-1]=CHAR_LA;
								thrml[i]=0;			//NULL ???
							}
						}else
							thrml[i]=PrntSingleChar[j];	//sticked char
						preCharMode=charMode;
						charMode=CHAR_MODE_SINGLE;
						charState=CHAR_STATE_FIRST;
						charDown=true;
						break;
					}
				}
				if(charDown) continue;
				for(u08 j=0;j<ARRAY_SIZE(PrntDualMask);j++){
					if (PrntDualMask[j]==thrml[i])
					{
						preCharIndx=(j*2)+1;
						thrml[i]=PrntDualChar[preCharIndx];	//sticked char
						preCharMode=charMode;
						charMode=CHAR_MODE_DUAL;
						charState=CHAR_STATE_MIDDLE;
						charDown=true;
						break;
					}
				}
				if(charDown) continue;
				for(u08 j=0;j<ARRAY_SIZE(PrntQuadMask);j++){
					if (PrntQuadMask[j]==thrml[i])
					{
						u08 plus=3;
						if(charState==CHAR_STATE_MIDDLE)
						{
							plus=2;
							quadEndSticked=true;
						}else
						{
							quadEndSticked=false;
						}
						preCharIndx=(j*4)+plus;
						thrml[i]=PrntQuadChar[preCharIndx];	//sticked char
						preCharMode=charMode;
						charMode=CHAR_MODE_QUAD;
						charState=CHAR_STATE_MIDDLE;
						charDown=true;
						break;
					}
				}
				if(charDown) continue;
				//if not find...
				thrml[i]=DEBUG_CHAR;
				charMode=CHAR_MODE_UNNOWN;
				//if(memchr(PrntDualMask,thrml[i],ARRAY_SIZE(PrntDualMask)!=NULL)
			}else	//current char is English
			{
				charState=CHAR_STATE_FIRST;
				thrml[i]=trmBuffer[i];
				switch (charMode)
				{
				case CHAR_MODE_DUAL:
					thrml[i-1]=PrntDualChar[preCharIndx-1];
					break;
				case CHAR_MODE_QUAD:
				{
					u08 inx=3;
					if(quadEndSticked)	
					{
						inx=2;
						quadEndSticked=false;
					}
					thrml[i-1]=PrntQuadChar[preCharIndx-inx];
					break;
				}
				}
			}
		}
	}
}

int main()
{
    char *remainSentence = strstr("+CMT: \"+989132022759\",\"\",\"16/08/16,10:32:48+14\"", "\"");//pointer to fist "
	char derivedSentence[100];
	// +CMT: "+989132022759","","16/08/16,10:32:48+14"
	// 0633064406270645	<salam
	sscanf(remainSentence, "%15s", derivedSentence);	//copy first word(until white space"
    printf(derivedSentence);
    printf("\r\n");
    
    u08 utf[]="\x06\x33\x06\x44\x06\x27\x06\x45";
    u08 trm[100];
    ConvertUtfToThermal(trm,utf,strlen(utf));
    printf("Wigth:%d\r\n",strlen(trm));
    for(u08 i=0;i<10;i++)
    {
        printf("%2x-",trm[i]);
    }
    printf("\r\n");
    return 0;
}

