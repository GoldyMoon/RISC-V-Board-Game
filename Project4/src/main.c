#include <stdint.h>
#include "ImageData.h"
void OutputString(const char *str);
void MoveCursor(uint32_t x, uint32_t y);
uint32_t ReadController(void);
void InitializeTimer(void);
uint32_t GetTimerTicks(void);
void UINT32ToHEX(char *str, uint32_t val);
void ClearBlock(uint32_t x, uint32_t y);


#define LEFT        0x1
#define UP          0x2
#define DOWN        0x4
#define RIGHT       0x8
#define BUTTON1     0x10
#define BUTTON2     0x20
#define BUTTON3     0x40
#define BUTTON4     0x80
#define Enable_INTERRUPT   (*(volatile uint32_t*)0x40000000) //Reading the enable interrupt

/*
typedef struct{
    uint32_t DBlue : 8;
    uint32_t DGreen : 8;
    uint32_t DRed : 8;
    uint32_t DAlpha : 8;
} SColor, *SColorRef;
*/
typedef struct{
    SColor DColors[256];
} SPalette, *SPaletteRef;
typedef struct{
    uint8_t DData[64][64];
} SLargeData, *SLargeDataRef;

typedef struct{
    uint32_t BPalette : 2;
    uint32_t BXoffset: 10;
    uint32_t BYoffset: 10;
    uint32_t BZoffset: 3;
    uint32_t Resevered: 7;
}BackgroundControl,*BackgroundControlRef;

typedef struct{
    uint32_t DPalette : 2;
    uint32_t DXOffset : 10;
    uint32_t DYOffset : 9;
    uint32_t DWidth : 5;
    uint32_t DHeight : 5;
    uint32_t DReserved : 1;
} SLargeSpriteControl, *SLargeSpriteControlRef;
volatile int global = 42;
volatile uint32_t controller_status = 0;
//volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);
volatile uint32_t *VIDEO_MODE = (volatile uint32_t *)(0x500FF414);
volatile SPalette *SPRITE_PALETTES1 = (volatile SPalette *)(0x500FD000);
volatile SPalette *SPRITE_PALETTES2 = (volatile SPalette *)(0x500FD400);
//volatile SPalette *SPRITE_PALETTES3 = (volatile SPalette *)(0x500FD800); //test1 delete it

volatile SLargeData  *LARGE_SPRITE_DATA1 = (volatile SLargeData *)(0x500B4000);
volatile SLargeData *LARGE_SPRITE_DATA2 = (volatile SLargeData *)(0x500B5000);
//volatile SLargeData *LARGE_SPRITE_DATA3 = (volatile SLargeData *)(0x500B6000); //test1 delete it 



volatile SLargeSpriteControl *LARGE_SPRITE_CONTROL1 = (volatile SLargeSpriteControl *)(0x500FF114);
volatile SLargeSpriteControl *LARGE_SPRITE_CONTROL2 = (volatile SLargeSpriteControl *)(0x500FF118);
//volatile SLargeSpriteControl *LARGE_SPRITE_CONTROL3 = (volatile SLargeSpriteControl *)(0x500FF114); //test1 delete it 

volatile uint8_t *BackgroundImage = (volatile uint8_t *)(0x50000000);
volatile SPalette *Background_PALETTES = (volatile SPalette *)(0x500FC000);
volatile BackgroundControl *Background_CONTROL = (volatile BackgroundControl *)(0x500FF100);

volatile int win = 0;
volatile int clearing = 0;

int main() {
    volatile int x_pos = 64 + 110 + 4;
    volatile int y_pos = 64;
    char arr[] = "|   |   |   |   |   |   |   |"; //text mode variables start
    char arr2[] = "X";
    char arr3[] = "O";
    char winstr[] = "X win!";
    char winstr2[] = "O win";
    char *str = arr;
    char *word1 = arr2;
    char *word2 = arr3;
    char *winstring = winstr;
    char *winstring2 = winstr2;// text mode variables end
    volatile uint32_t ControllerStat,updated = 11111111; //Controller status related
    volatile int count = 0;
    volatile int stack = 6;
    volatile int countdouble = 0;
    volatile int county = 0;
    volatile int countx = 0;
    volatile int win = 0;//Counter related
    volatile int tempx[84],tempy[84]; //Previous Tokens data 
    volatile int enableu = 0;
    Enable_INTERRUPT = 0x4; //Enable interrupt for CMIE 
    *VIDEO_MODE = 1; //Change mode -- Text mode may have problems due to the repeating usage of variable names

    for(int i = 0; i < 235;i++){
        SPRITE_PALETTES1->DColors[i] = Palette[i];
        SPRITE_PALETTES2->DColors[i] = Palette[i]; //try to comment it?
        Background_PALETTES->DColors[i] = Palette[i];

    }
    for(int i = 0; i < 240;i++){
        for(int j = 0; j < 280; j++){
            BackgroundImage[i*512+j] = BoardImage[i*280+j]; // From office hour
        }
    }
    
    for(int i = 0; i < 42; i+=2){ //from Piazza -- Create 42 tokens, 21 yellow, 21 blue
        for(int j = 0; j < 32; j++ ){
            for(int k = 0;k < 32;k++){
                LARGE_SPRITE_DATA1[i].DData[j][k] = BluePieceImage[j* 32 + k];
                LARGE_SPRITE_DATA2[i].DData[j][k] = YellowPieceImage[j* 32 +k];
            }
        }
    }
    
    for(int i = 0; i < 42; i+=2){                      // Initialize 42 tokens.
        LARGE_SPRITE_CONTROL1[i].DPalette = 0;
        LARGE_SPRITE_CONTROL1[i].DHeight = 31;
        LARGE_SPRITE_CONTROL1[i].DWidth = 31;
        LARGE_SPRITE_CONTROL2[i].DPalette = 0;
        LARGE_SPRITE_CONTROL2[i].DHeight = 31;
        LARGE_SPRITE_CONTROL2[i].DWidth = 31;
        LARGE_SPRITE_CONTROL1[i].DXOffset = 0;
        LARGE_SPRITE_CONTROL1[i].DYOffset = 0;
        LARGE_SPRITE_CONTROL2[i].DXOffset = 0;
        LARGE_SPRITE_CONTROL2[i].DYOffset = 0;
    }

    Background_CONTROL[0].BXoffset = 512+110;
    Background_CONTROL[0].BYoffset = 288+34;
    Background_CONTROL[0].BZoffset = 7;


    while (1) {
        while(clearing == 1){
            clearing = 0;
            count = 0;
            stack = 6;
            countdouble = 0;
            county = 0;
            countx = 0;
            win = 0;
            updated = 11111111;
            x_pos = 64 + 110 + 4;
            y_pos = 64;
            
            for(int i = 0; i < 42; i+=2){
                LARGE_SPRITE_CONTROL1[i].DXOffset = 0;
                LARGE_SPRITE_CONTROL1[i].DYOffset = 0;
                LARGE_SPRITE_CONTROL2[i].DXOffset = 0;
                LARGE_SPRITE_CONTROL2[i].DYOffset = 0;  
            }
            

            /*
            LARGE_SPRITE_CONTROL1[0].DXOffset = 68 + 110;
            LARGE_SPRITE_CONTROL1[0].DYOffset = 64;
            */
            for(int i = 0; i < 84; i++){
                tempx[i] = 0;
                tempy[i] = 0;
            }
        } 
        
        ControllerStat = ReadController();
        if((updated != ControllerStat)){  //from discussion 
                if(ControllerStat & LEFT && (x_pos > 64 + 110 + 4)){ //左墙壁需要修改 -- 修改完毕
                    //ClearBlock(x_pos+24,y_pos); //text mode
                    //x_pos = x_pos - 4;    //Text mode change
                    
                    x_pos = x_pos -40;
                }
            
                else if(ControllerStat & RIGHT && x_pos < 64 + 110 + 4 + 40*6 && (win == 0)){ //右墙壁需要修改 -- 修改完毕
                    //ClearBlock(x_pos,y_pos); //text mode
                    //x_pos = x_pos + 4;    //Text mode change

                    x_pos = x_pos + 40;
                }

                else if((ControllerStat & BUTTON1) && (y_pos < 64 + 288 - (40* 2) && (win == 0)) /*&& (win == 0)*/){
                    //ClearBlock(x_pos,y_pos);  //text mode
                    for(int j = 0; j < countdouble; j+=2){
                        if(x_pos == tempx[j]){
                            stack--;
                            if(stack < 1){   //Prevent overflow -- the height is full 
                                stack = 0;
                                break;
                            }
                        }
                    }

                    y_pos = y_pos + 40 * stack;
                    if(y_pos != y_pos - (40 * stack)){ //Prevent overflow -- the height is full 
                        tempx[countdouble] = x_pos;
                        tempy[countdouble] = y_pos;
                        count ++;
                        countdouble +=2;
                        if(count % 2 == 0){
                            county += 2;
                        }
                        else{
                            countx += 2;
                        }
                        x_pos = 68 + 110;
                        y_pos = 64;
                    }
                    stack = 6;
                }
                //text mode
                /*
                MoveCursor(18,1);
                OutputString(str);
                MoveCursor(18,2);
                OutputString(str);
                MoveCursor(18,3);
                OutputString(str);
                MoveCursor(18,4);
                OutputString(str);
                MoveCursor(x_pos+24,y_pos);
                if(count %2 == 0){
                    OutputString(word1);
                }
                else{
                    OutputString(word2);
                }
                for(int i = 0; i < count; i++){
                    MoveCursor(tempx[i]+24,tempy[i]);
                    if(i%2 == 0){
                        OutputString(word1);
                    }
                    else{
                        OutputString(word2);
                    }
                }
                
                */             
            }

        for(int j = 0; j < countdouble; j+=4){        // Possiblity optimize this part using iteration 
            for(int k = 0; k < countdouble; k+=4){
                if((tempx[k] == tempx[j] + 40) && (tempy[k] == tempy[j])){   //For o in a horizontal line
                    for(int a = 0; a < countdouble; a+=4){
                        if((tempx[a] == tempx[k] + 40) && (tempy[a] == tempy[k])){
                            for(int b = 0; b < countdouble; b+=4){
                                if((tempx[b] == tempx[a] + 40) && (tempy[b] == tempy[a])){
                                    win = 1;
                                }
                            }
                        }
                    }
                }
                else if((tempy[k] == tempy[j] - 40) && (tempx[k] == tempx[j])){  //For x in a vertical line
                    for(int a = 0; a < countdouble; a+=4){
                        if((tempy[a] == tempy[k] - 40) && (tempx[a] == tempx[k])){
                            for(int b = 0; b < countdouble; b+=4){
                                if((tempy[b] == tempy[a] - 40) && (tempx[b] == tempx[a])){
                                    win = 1;
                                }
                            }
                        }
                    }
                }
                else if((tempx[k] == tempx[j] + 40) && (tempy[k] == tempy[j] - 40)){ //For x in a diagonal line right top corner
                    for(int a = 0; a < countdouble; a+=4){
                        if((tempx[a] == tempx[k] + 40) && (tempy[a] == tempy[k] - 40)){
                            for(int b = 0; b < countdouble; b+=4){
                                if((tempx[b] == tempx[a] + 40) && (tempy[b] == tempy[a] - 40)){
                                    win = 1;
                                }
                            }
                        }
                    }
                }
                else if((tempx[k] == tempx[j] - 40) && (tempy[k] == tempy[j] - 40)){ //For x in a diagonal line to left top corner
                    for(int a = 0; a < countdouble; a+=4){
                        if((tempx[a] == tempx[k] - 40) && (tempy[a] == tempy[k] - 40)){
                            for(int b = 0; b < countdouble; b+=4){
                                if((tempx[b] == tempx[a] - 40) && (tempy[b] == tempy[a] - 40)){
                                    win = 1;
                                }
                            }
                        }
                    }
                }
            }
        }       
        for(int j = 2; j < countdouble; j+=4){
            for(int k = 2; k < countdouble; k+=4){
               if((tempx[k] == tempx[j] + 40) && (tempy[k] == tempy[j])){   //For o in a horizontal line
                    for(int a = 2; a < countdouble; a+=4){
                        if((tempx[a] == tempx[k] + 40) && (tempy[a] == tempy[k])){
                            for(int b = 2; b < countdouble; b+=4){
                                if((tempx[b] == tempx[a] + 40) && (tempy[b] == tempy[a])){
                                    win = 1;
                                }
                            }
                        }
                    }
                }
                else if((tempy[k] == tempy[j] - 40) && (tempx[k] == tempx[j])){  //For y in a vertical line
                    for(int a = 2; a < countdouble; a+=4){
                        if((tempy[a] == tempy[k] - 40) && (tempx[a] == tempx[k])){
                            for(int b = 2; b < countdouble; b+=4){
                                if((tempy[b] == tempy[a] - 40) && (tempx[b] == tempx[a])){
                                    win = 1;
                                }
                            }
                        }
                    }
                }
                else if((tempx[k] == tempx[j] + 40) && (tempy[k] == tempy[j] - 40)){ //For x in a diagonal line right top corner
                    for(int a = 2; a < countdouble; a+=4){
                        if((tempx[a] == tempx[k] + 40) && (tempy[a] == tempy[k] - 40)){
                            for(int b = 2; b < countdouble; b+=4){
                                if((tempx[b] == tempx[a] + 40) && (tempy[b] == tempy[a] - 40)){
                                    win = 1;
                                }
                            }
                        }
                    }
                }
                else if((tempx[k] == tempx[j] - 40) && (tempy[k] == tempy[j] - 40)){ //For x in a diagonal line to left top corner
                    for(int a = 2; a < countdouble; a+=4){
                        if((tempx[a] == tempx[k] - 40) && (tempy[a] == tempy[k] - 40)){
                            for(int b = 2; b < countdouble; b+=4){
                                if((tempx[b] == tempx[a] - 40) && (tempy[b] == tempy[a] - 40)){
                                    win = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
        if(count % 2 == 0 && (win == 0)){ //Show the new ball
            LARGE_SPRITE_CONTROL1[countx].DXOffset = x_pos; 
            LARGE_SPRITE_CONTROL1[countx].DYOffset = y_pos;
        }

        else if(count % 2 == 1 && (win == 0) ){
            LARGE_SPRITE_CONTROL2[county].DXOffset = x_pos;
            LARGE_SPRITE_CONTROL2[county].DYOffset = y_pos;
        }

        for(int g = 0; g < countdouble; g+=2){   //Always show the ball 
            if(g % 4 == 0){
                LARGE_SPRITE_CONTROL1[countx - 2].DXOffset = tempx[g];   
                LARGE_SPRITE_CONTROL1[countx - 2].DYOffset = tempy[g];
            }
            else{
                LARGE_SPRITE_CONTROL2[county - 2].DXOffset = tempx[g]; 
                LARGE_SPRITE_CONTROL2[county - 2].DYOffset = tempy[g];
            }
        }   
        updated = ControllerStat;
        
        /*
        LARGE_SPRITE_CONTROL1[0].DXOffset = x_pos;
        LARGE_SPRITE_CONTROL1[0].DYOffset = y_pos;
        */
        //int c = a + b + global; what's this for?
        /*if(global != last_global){
            x_pos += delta_x;
            y_pos += delta_y>>4;
            delta_y++;
            if(x_pos < 64){
                x_pos = 64;
                delta_x = -delta_x;
                SPRITE_PALETTES1[0].DColors[0].DRed++;
                if(!SPRITE_PALETTE1[0].DColors[0].DRed){
                    SPRITE_PALETTES1[0].DColors[0].DRed = 128;
                }
            }
            if(x_pos > 512){
                x_pos = 512;
                delta_x = -delta_x;
                SPRITE_PALETTES1[0].DColors[0].DGreen++;
                if(!SPRITE_PALETTES1[0].DColors[0].DGreen){
                    SPRITE_PALETTES1[0].DColors[0].DGreen = 128;
                }
            }
            if(y_pos < 64){
                y_pos = 64;
                delta_y = 0;//-delta_y;
                SPRITE_PALETTES1[0].DColors[0].DBlue++;
                if(!SPRITE_PALETTES1[0].DColors[0].DBlue){
                    SPRITE_PALETTES1[0].DColors[0].DBlue = 128;
                }
            }
            if(y_pos > 288){
                y_pos = 288;
                delta_y = -((delta_y/2)+(delta_y/4));
                SPRITE_PALETTES1[0].DColors[0].DRed++;
                if(!SPRITE_PALETTES1[0].DColors[0].DRed){
                    SPRITE_PALETTES1[0].DColors[0].DRed = 128;
                }
            }
            last_global = global;
        }
        */
    }
    return 0;
}

void ClearBlock(uint32_t x, uint32_t y){
    MoveCursor(x,y);
    OutputString("                             ");
    MoveCursor(x,y+1);
    OutputString("                             ");
    MoveCursor(x,y+2);
    OutputString("                             ");
    MoveCursor(x,y+3);
    OutputString("                             ");
    MoveCursor(x,y+4);
    OutputString("                             ");
    MoveCursor(x,y+5);
    OutputString("                             ");
}
