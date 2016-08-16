#include <stdio.h>
#include "string.h"
#include "PrntChar.h"

typedef unsigned char   bool;
typedef unsigned char   u08;

#define ARRAY_SIZE(foo) (sizeof(foo)/sizeof(foo[0]))
#	ifndef false
#		define false    0
#	endif
#	ifndef true
#		define true     1
#	endif
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
				if(!utf[i+1])
				{
					//end of string
					break;
				}
			}
		}
		if(thrml[termalByte-1])	
		{
			thrml[++termalByte]=0;	//terminate to zero & correct index
		}
		for(u08 i=0;i<10;i++){
            printf("%2x ",trmBuffer[i]);
        }
        printf("\r\n");
		
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
				printf("%i:thrml:%2x trmBuffer:%2x",i,thrml[i],trmBuffer[i]);
                printf("\r\n");
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
				charMode=CHAR_MODE_UNNOWN;
			}
		}
		u08 *endString=thrml+termalByte-1;	//before terminated zero 
		for(u08 i=0;i<sizeof(thrml);i++){
            printf("%2x ",thrml[i]);
        }
        printf("\r\n");
		for (u08 *i=thrml;i<endString;i++)
		{
			u08 *buff=memchr(i,0,endString-i+1);
			if (buff!=NULL)
			{
				i=buff;
				memmove(buff,buff+1,endString-buff+1);
			}
			else break;
		}
	}
}
int main()
{
    u08 utf[]="\x0\x30\x0\x39\x6\x33\x06\x44\x06\x27\x6\x45\x0\xd";
    u08 trm[100];
    printf("Hello, World:\n");
    ConvertUtfToThermal(trm,utf,sizeof(utf));
    for(u08 i=0;i<strlen((char*)trm);i++){
        printf("%2x ",trm[i]);
    }
    printf("\r\n");
    
    return 0;
}

