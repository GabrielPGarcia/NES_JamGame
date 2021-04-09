#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"
extern const byte World_0_pal[16];
extern const byte World_0_rle[];

extern const byte World_1_pal[16];
extern const byte World_1_rle[];

// link the pattern table into CHR ROM
//#link "World_0.s"
/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x30,			// screen color

  0x0F,0x07,0x1C,0x00,	// background palette 0
  0x1C,0x20,0x2C,0x00,	// background palette 1
  0x00,0x10,0x20,0x00,	// background palette 2
  0x06,0x16,0x26,0x00,   // background palette 3

  0x3C,0x0C,0x1C,0x00,	// sprite palette 0
  0x2C,0x0C,0x3C,0x00,	// sprite palette 1
  0x21,0x0C,0x3A,0x00,	// sprite palette 2
  0x0D,0x27,0x2A	// sprite palette 3
  };
#define NUM_ACTORS 5
#define NUM_OBJECT 5


#define DEF_METASPRITE_2x2(name,code,pal)\
const unsigned char name[]={\
  0,0, (code)+0, pal, \
  0,8, (code)+1, pal, \
  8,0, (code)+2, pal, \
  8,8, (code)+3, pal,\
  128\
};
#define DEF_METASPRITE_2x2_FLIP(name,code,pal)\
const unsigned char name[]={\
  8,0, (code)+0, (pal)|OAM_FLIP_H, \
  8,8, (code)+1, (pal)|OAM_FLIP_H, \
  0,0, (code)+2, (pal)|OAM_FLIP_H, \
  0,8, (code)+3, (pal)|OAM_FLIP_H, \
  128\
};
#define DEF_METASPRITE_2x2_FLIPV(name,code,pal)\
const unsigned char name[]={\
  0,8, (code)+0, (pal)|OAM_FLIP_V, \
  0,0, (code)+1, (pal)|OAM_FLIP_V, \
  8,8, (code)+2, (pal)|OAM_FLIP_V, \
  8,0, (code)+3, (pal)|OAM_FLIP_V, \
  128\
};
DEF_METASPRITE_2x2(playerRRun0, 0xdc, 0);
DEF_METASPRITE_2x2(playerRRun1, 0xe0, 0);
DEF_METASPRITE_2x2(playerRRun2, 0xdc, 1);
DEF_METASPRITE_2x2(playerRRun3, 0xe0, 1);

DEF_METASPRITE_2x2(playerRRun4, 0xe4, 0);
DEF_METASPRITE_2x2(playerRRun5, 0xe8, 0);
DEF_METASPRITE_2x2(playerRRun6, 0xe4, 1);
DEF_METASPRITE_2x2(playerRRun7, 0xe8, 1);

DEF_METASPRITE_2x2_FLIP(playerRRu0, 0xdc, 0);
DEF_METASPRITE_2x2_FLIP(playerRRu1, 0xe0, 0);
DEF_METASPRITE_2x2_FLIP(playerRRu2, 0xdc, 1);
DEF_METASPRITE_2x2_FLIP(playerRRu3, 0xe0, 1);

DEF_METASPRITE_2x2_FLIPV(playerRRu4, 0xe4, 0);
DEF_METASPRITE_2x2_FLIPV(playerRRu5, 0xe8, 0);
DEF_METASPRITE_2x2_FLIPV(playerRRu6, 0xe4, 1);
DEF_METASPRITE_2x2_FLIPV(playerRRu7, 0xe8, 1);

DEF_METASPRITE_2x2(Door0, 0xc4, 0);
DEF_METASPRITE_2x2(Door1, 0xc4, 0);

const unsigned char* const playerRunSeq[16] = {
  playerRRun0,	playerRRun1,	playerRRun2,	playerRRun3,
  playerRRun4,	playerRRun5,	playerRRun6,	playerRRun7, 
  playerRRu0,	playerRRu1,	playerRRu2,	playerRRu3 , 
  playerRRu4,	playerRRu5,	playerRRu6,	playerRRu7 
  };

const unsigned char* const WoldSeq[16] = {
  World_0_pal,World_1_rle,
  World_0_pal,World_0_rle,World_1_pal,World_1_rle
  };

byte actor_x[NUM_ACTORS];
byte actor_y[NUM_ACTORS];
sbyte actor_dx[NUM_ACTORS];
sbyte actor_dy[NUM_ACTORS];

byte Object_x[NUM_OBJECT];
byte Object_y[NUM_OBJECT];
sbyte Object_dx[NUM_OBJECT];
sbyte Object_dy[NUM_OBJECT];

typedef struct Objects {
  word yy;		// Y position in pixels (16 bit)
  byte x;		// X position in pixels (8 bit)
  byte y;		// X position in pixels (8 bit)
  byte state;		// ActorState
} Objects;

Objects OKey[1];
Objects OLock[1];
Objects mStone[1];  //moveable stone
Objects lives[3];  	//number of lives 
Objects arrow[1];  	//arrow


void collision (int x1,int x2,int y1,int y2, int actor,int iObject,int ikeys,int iMap1,int iMap2,int iMap3);
void key(int x1,int x2,int y1,int y2,int inum);
void PickUpObject(int x1,int x2,int y1,int y2);
void pickupkey(int inum,int ikeys);
void lock(int inum,int ikeys);
// setup PPU and tables
void setup_graphics() {
  // clear sprites
  oam_clear();
  // set palette colors
  pal_all(PALETTE);
}
void BackGround(const byte* pal, const byte* rle) {
  // disable rendering
  ppu_off();
  // set palette, virtual bright to 0 (total black)
  pal_bg(pal);


  // unpack nametable into the VRAM
  vram_adr(0x2000);
  vram_unrle(rle);
  // enable rendering
  ppu_on_all();
}
void actors_setup()
{
  actor_x[0] = 128;
  actor_y[0] = 189;
  actor_dx[0] = 0;
  actor_dy[0] = 0;
}
void SetPlayer()
{
  actor_x[0] = 128;
  actor_y[0] = 189;
  actor_dx[0] = 0;
  actor_dy[0] = 0;
}
char pad;	//input pad
byte runseq;	//actor next Seq
char oam_id;	// sprite ID
char oam_id1;	// sprite ID
int items[9] = {0,0,0,0,0,0,0,0,0};//keys/potions
int iOpen = 0; //door on/off
int rlud = 0; // right,lest,up, and Down save last key input
int test = 0; //display collision 
int i = 0;    //actor Seq
int iMap = 0; //the level the player will be
int keyplaced = 0; //setup key

void Door(int x, int y)
{
  if(iOpen != 1)
  {    
    oam_id = oam_meta_spr(x, y,  oam_id, Door0);
    collision (x-30,x+40,y-15,y+16, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
  }
  else
  {  
    collision (x-24,x-5,y-15,y+16, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    collision (x+5,x+40,y-15,y+16, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)

  }

}
void MapCollision ()
{ 
  if(iMap == 0)
  {
    collision (0,46,0,220, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    collision (208,250,0,220, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    collision (146,220,0,78, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    collision (0,110,0,78, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    collision (0,250,192,221, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    collision (98,158,10,12, 0,0,0,1,2,1);//(x1,x2,y1,y2,actor,object,map border,new map)
    
  mStone[0].x = 131;
  mStone[0].y = 99;
    collision (mStone[0].x-16,mStone[0].x+8,mStone[0].y-16,mStone[0].y+8, 0,1,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    Door(128,62);
  } 
  if(iMap == 1)
  {   
    collision (0,46,0,220, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    collision (208,250,0,220, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    collision (146,220,0,78, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    collision (0,110,0,78, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    collision (0,250,192,221, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
 
    key(36,220,66,190,0);
    lock(0,0);
    collision (98,158,10,12, 0,0,0,1,1,0);//(x1,x2,y1,y2,actor,object,map border,new map)

    //collision (36,220,66,190, 0,2,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    Door(128,62);
  }

}
//** actors animation and directions facing**\\
void directions(int iStart,int iEnd)
{  
  if(i <= 150)
    i++;
  else if(i >=100)
    i=0;
  if(runseq < iEnd && i==100)
  {
    runseq = runseq+1;
  }
  if(runseq == iEnd && i==100)
  {
    runseq = iStart;   
  }
}
//** What the player can do**\\
void player_action()
{        
  pad = pad_poll(0);

  if (pad&PAD_UP )
  {
    actor_dy[0]=-1;
    actor_y[0] += actor_dy[0]; 
    rlud=0;
    runseq = 5;
  }  
  if (pad&PAD_DOWN)
  {
    actor_dy[0]=1;  
    actor_y[0] += actor_dy[0];
    rlud=1;
    runseq = 13;
  }     
  if(pad&PAD_LEFT)
  {
    actor_dx[0]=-1;
    actor_x[0] += actor_dx[0];
    rlud=2;
    runseq = 0;
  }
  if (pad&PAD_RIGHT)
  {
    actor_dx[0]=1;    
    actor_x[0] += actor_dx[0];
    rlud=3;
    runseq = 9;
  }

  if(rlud == 0)    
    directions(5,8);
  if(rlud == 1)
    directions(13,16);
  if(rlud == 2)
    directions(0,4);
  if(rlud == 3)
    directions(9,12);
  oam_id = oam_meta_spr(actor_x[0], actor_y[0],  oam_id, playerRunSeq[runseq]);
  MapCollision();
  if (oam_id!=0) oam_hide_rest(oam_id);

}
void MoveMap(int iMap2,int iMap3)
{
  iMap2 = iMap2*2;
  iOpen=0;
  SetPlayer();
  //SetEnemy();
  iMap = iMap3;
  BackGround(WoldSeq[iMap2], WoldSeq[iMap2+1]);
}

void main(void)
{
  actors_setup();
  setup_graphics();
  // draw message  
  vram_adr(NTADR_A(2,2));
  vram_write("HELLO, WORLD!", 12);
  // enable rendering
  ppu_on_all();
  //      0*2=0,if 0 make it 1
  MoveMap(iMap,1);
  mStone[0].x = 131;
  mStone[0].y = 99;

  // infinite loop
  while(1) {

    player_action();    

  }
}
void co()
{
}
/* (x1,x2,y1,y2) location, actor(player or enemy), object 1 (1: pressure plate)(2:keys)(3:moveing block)(4:pickup item)
imap1(0 or 1)if we need to move, imap2 update map, imap3 what level
*/
void collision (int x1,int x2,int y1,int y2, int iActor,
                int iObject,int ikeys,int iMap1,int iMap2,int iMap3)
{  

  //top of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y1-1 &&  actor_y[iActor] <= y1 )&&  (actor_x[iActor] >= x1 && actor_x[iActor] <=x2))
  { 
    if(iObject == 1)
    {
      iOpen = 1;
    }  
    if(iObject == 2)
    {  
      if(items[ikeys] == 1)
        iOpen = 1;     
      pickupkey(0,ikeys);
      //lock(0,ikeys);
    }
    else if(iMap1 == 1)
    {
      MoveMap(iMap2,iMap3);
    }
    else
      actor_y[iActor] = actor_y[iActor]-1;

  }

  //bottom of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y2-1 && actor_y[iActor] <= y2) &&  (actor_x[iActor] >= x1 && actor_x[iActor] <=x2))
  {
    if(iObject == 1)
    {
      iOpen = 1;
    }    
    else if(iObject == 2&& iOpen !=1)
    { 
      if(items[ikeys] == 1)
        iOpen = 1;     
      pickupkey(0,ikeys);    
      //lock(0,ikeys);
    }  
    else if(iMap1 == 1)
    {
      MoveMap(iMap2,iMap3);
    }
    else
      actor_y[iActor] = actor_y[iActor]+1;

  }   

  //right side of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y1 && actor_y[iActor] <= y2) &&  (actor_x[iActor] >= x1-1 && actor_x[iActor] <=x2))
  {
    if(iObject == 1)
    {
      iOpen = 1;
    }   
    else if(iObject == 2)
    {
    }
    else if(iMap1 == 1)
    {
      MoveMap(iMap2,iMap3);
    }
    else
      actor_x[iActor] = actor_x[iActor]-1;

  }


  //right side of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y1 && actor_y[iActor] <= y2) &&  (actor_x[iActor] >= x2 && actor_x[iActor] <=x2+1))
  {
    if(iObject == 1)
    {
      iOpen = 1;
    }   
    else if(iObject == 2)
    {
      //iOpen = 1;
    }
    else if(iMap1 == 1)
    {
      MoveMap(iMap2,iMap3);
    }
    else
      actor_x[iActor] = actor_x[iActor]+1;

  }

  if(test == 1){
    oam_id = oam_spr(x1+14, y1+14, 1, 4, oam_id);  
    oam_id = oam_spr(x2-6, y1+14, 1, 4, oam_id);  
    oam_id = oam_spr(x1+14, y2-7, 1, 4, oam_id);
    oam_id = oam_spr(x2-6, y2-7, 1, 4, oam_id);
  } 


}
void lock(int inum,int ikeys)
{
    OLock[inum].x = 146;
    OLock[inum].y =66;    
    OLock[inum].state = 40;

    oam_id = oam_spr(OLock[inum].x, OLock[inum].y, OLock[inum].state, 4, oam_id);
  if(items[ikeys] == 1)
    collision (OLock[inum].x-16,OLock[inum].x+8,OLock[inum].y-8,OLock[inum].y+16, 0,2,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
   
}
void pickupkey(int inum,int ikeys)
{
    OKey[inum].x = 180;
    OKey[inum].y =18;    
    OKey[inum].state = 24;

    oam_id1 = oam_spr(OKey[inum].x, OKey[inum].y, OKey[inum].state, 4, oam_id1); 
  items[ikeys]=1;
}
void key(int x1,int x2,int y1,int y2,int inum)
{
  if(keyplaced == 0)
  {
      OKey[inum].x = (rand()%(x1-x2))+x1;
      OKey[inum].y = (rand()%(y1-y2))+y1;    
      OKey[inum].state = 24;
      keyplaced = 1;
  }    
  else if(keyplaced==1)
  {    
    oam_id = oam_spr(OKey[inum].x, OKey[inum].y, OKey[inum].state, 4, oam_id);  
   collision (OKey[inum].x-16,OKey[inum].x+8,OKey[inum].y-16,OKey[inum].y+8, 0,2,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
  }
  inum = inum;
};






