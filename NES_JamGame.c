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


extern const byte Menu0_pal[16];
extern const byte Menu0_rle[];

extern const byte Menu1_pal[16];
extern const byte Menu1_rle[];
extern const byte Menu2_pal[16];
extern const byte Menu2_rle[];


// link the pattern table into CHR ROM
//#link "World_0.s"
//#link "Menu.s"
/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x0F,			// screen color

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
DEF_METASPRITE_2x2(Left0, 0xdc, 0);
DEF_METASPRITE_2x2(Left1, 0xe0, 0);
DEF_METASPRITE_2x2(Left2, 0xdc, 1);
DEF_METASPRITE_2x2(Left3, 0xe0, 1);

DEF_METASPRITE_2x2_FLIP(Right0, 0xdc, 0);
DEF_METASPRITE_2x2_FLIP(Right1, 0xe0, 0);
DEF_METASPRITE_2x2_FLIP(Right2, 0xdc, 1);
DEF_METASPRITE_2x2_FLIP(Right3, 0xe0, 1);

DEF_METASPRITE_2x2(Up0, 0xe4, 0);
DEF_METASPRITE_2x2(Up1, 0xe8, 0);
DEF_METASPRITE_2x2(Up2, 0xe4, 1);
DEF_METASPRITE_2x2(Up3, 0xe8, 1);


DEF_METASPRITE_2x2_FLIPV(Down0, 0xe4, 0);
DEF_METASPRITE_2x2_FLIPV(Down1, 0xe8, 0);
DEF_METASPRITE_2x2_FLIPV(Down2, 0xe4, 1);
DEF_METASPRITE_2x2_FLIPV(Down3, 0xe8, 1);

DEF_METASPRITE_2x2(Door0, 0xc4, 0);
DEF_METASPRITE_2x2(Door1, 0xc4, 1);
DEF_METASPRITE_2x2(Door2, 0xc4, 2);
DEF_METASPRITE_2x2(Door3, 0xc4, 3);

const unsigned char* const playerRunSeq[16] = {
  Left0,	Left1,	Left2,	Left3,
  Right0,	Right1,	Right2,	Right3,
  Up0,		Up1,	Up2,	Up3,
  Down0,	Down1,	Down2,	Down3,
};
const unsigned char* const Doors[4] = {
  Door0,	Door1,	Door2,	Door3
  };

const unsigned char* const WoldSeq[16] = {
  World_0_pal,World_1_rle,
  World_0_pal,World_0_rle,World_1_pal,World_1_rle
  };

byte actor_x[NUM_ACTORS];
byte actor_y[NUM_ACTORS];
sbyte actor_dx[NUM_ACTORS];
sbyte actor_dy[NUM_ACTORS];


typedef struct Objects {
  word yy;		// Y position in pixels (16 bit)
  byte x;		// X position in pixels (8 bit)
  byte y;		// X position in pixels (8 bit)
  byte id;		// ActorState
  byte state;		// ActorState
} Objects;
Objects object[20];//0-3 keys  4-8 locks  

void collision (char oam_id,int x1,int x2,int y1,int y2, int iActor,
                int iObject,int ikeys,int iDoor,int iMap1,int iMap2,int iMap3);
void ObjectKey(char oam_id,int x1,int x2,int y1,int y2,int inum,int icolor);
void PickUpObject(char oam_id,int x1,int x2,int y1,int y2,int icolor);
void pickupkey(int ikeys);
void lock(char oam_id,int x, int y, int ikeys,int icolor);
// setup PPU and tables

void setup_graphics() 
{
  ppu_off();
  oam_clear();
  pal_all(PALETTE);
  ppu_on_all();
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
void SetEnemy()
{
  actor_x[0] = 10;
  actor_y[0] = 10;
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
int items[9] = {0,0,0,0,0,0,0,0,0};//keys/potions
int iOpen[5] = {0,0,0,0,0}; //door on/off
int rlud = 0; // right,lest,up, and Down save last key input
int test = 0; //display collision 
int k = 0;    //actor Seq
int iMap = 0; //the level the player will be
int keyplaced = 0; //setup key
int check = 0;


/*void collision (char oam_id,int x1,int x2,int y1,int y2, int iActor,
                int iObject,int ikeys,int iDoor,int iMap1,int iMap2,int iMap3)*/
void Door(char oam_id,int x, int y,int id,int icolor)
{
  if(iOpen[id] != 1)
  {    
    collision (oam_id,x-30,x+40,y-15,y+16, 0,0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    oam_id = oam_meta_spr(x, y,  oam_id,Doors[icolor]);
  }
  else
  {  
    collision (oam_id,x-24,x-5,y-15,y+16, 0,0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    collision (oam_id,x+5,x+40,y-15,y+16, 0,0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)

  }

}
void MapCollision (char oam_id)
{ 
  if(iMap == 3)
  {
    //collision (oam_id+67,0,46,0,220, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    //collision (oam_id+67,208,250,0,220, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    //collision (oam_id+67,146,220,0,78, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    //collision (oam_id+67,0,110,0,78, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    // collision (oam_id+67,0,250,192,221, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    //collision (oam_id+67,98,158,10,12, 0,0,0,1,2,1);//(x1,x2,y1,y2,actor,object,map border,new map)


    collision (oam_id,131-16,131+8,99-16,99+8, 0,1,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)

    Door(oam_id,128,62,0,0);   
  } 
  if(iMap == 4)
  {   
    //collision (oam_id+67,0,46,0,220, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    //collision (oam_id+67,208,250,0,220, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    //collision (oam_id+67,146,220,0,78, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    //collision (oam_id+67,0,110,0,78, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    //collision (oam_id+67,0,250,192,221, 0,0,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
    //lock(oam_id+20,5,0);
    //x,x,y,y,id
    ObjectKey(oam_id,36,220,66,190,0,4);
    ObjectKey(oam_id+8,45,220,66,190,1,5);
    ObjectKey(oam_id+12,36,100,66,190,2,6);
    ObjectKey(oam_id+16,45,150,66,190,3,7);
    lock(oam_id+20,146,70,0,4);
    lock(oam_id+24,158,70,1,5);
    lock(oam_id+28,170,70,2,6);
    lock(oam_id+32,185,70,3,7);
    //collision (oam_id,98,158,10,12, 0,0,0,0,1,1,0);//(x1,x2,y1,y2,actor,object,map border,new map)

    //collision (36,220,66,190, 0,2,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)


    Door(oam_id+96,128,62,0,0); 
    Door(oam_id+80,128,60,1,1);     
    Door(oam_id+48,128,58,2,2);    
    Door(oam_id+64,128,56,3,3);  

  }

}

int g = 10;
int i = 0;    //actor Seq
//** actors animation and directions facing**\\
void directions(int iStart,int iEnd)
{  
  if(i <= g)
    i++;
  else if(i >=g)
    i=0;

  if(runseq <= iEnd-1 && i==g)
  {
    runseq = runseq + 1;
  }
  else if(runseq == iEnd && i==g)
  {
    runseq = iStart;   
  }
}
//** What the player can do**\\
void player_action(char oam_id,bool iAddE, int ienemyid)
{        
  pad = pad_poll(0);

  if(pad&PAD_LEFT)
  {
    actor_dx[0]=-1;
    actor_x[0] += actor_dx[0];
    rlud=0;
    if(runseq >=4)
      runseq = 0;   
  }  if (pad&PAD_RIGHT)
  {
    actor_dx[0]=1;    
    actor_x[0] += actor_dx[0];
    rlud=4;  
    if(runseq <=4 || runseq >=8)
      runseq = 4;   
  }
  if (pad&PAD_UP )
  {
    actor_dy[0]=-1;
    actor_y[0] += actor_dy[0]; 
    rlud=8;  
    if(runseq <=8 || runseq >=12)
      runseq = 8;  
  }

  if (pad&PAD_DOWN)
  {
    actor_dy[0]=1;  
    actor_y[0] += actor_dy[0];
    rlud=12; 
    if(runseq <=12 )
      runseq = 12;     
  }
  if(iAddE)
    if( actor_x[ienemyid] != actor_x[0] || actor_y[ienemyid] != actor_y[0])
    {

      if(actor_x[0] > actor_x[ienemyid]){
        actor_x[ienemyid] = actor_x[ienemyid] + 1;
        oam_id = oam_meta_spr(actor_x[ienemyid], actor_y[ienemyid], oam_id, playerRunSeq[runseq]);}
      else if(actor_x[0] < actor_x[ienemyid]){
        actor_x[ienemyid] = actor_x[ienemyid] - 1;
        oam_id = oam_meta_spr(actor_x[ienemyid], actor_y[ienemyid], oam_id, playerRunSeq[runseq]);}
      else if(actor_y[0] > actor_y[ienemyid]){
        actor_y[ienemyid] = actor_y[ienemyid] + 1;
        oam_id = oam_meta_spr(actor_x[ienemyid], actor_y[ienemyid], oam_id, playerRunSeq[runseq]);}
      else if(actor_y[0] < actor_y[ienemyid]){
        actor_y[ienemyid] = actor_y[ienemyid] - 1;
        oam_id = oam_meta_spr(actor_x[ienemyid], actor_y[ienemyid], oam_id, playerRunSeq[runseq]);}

    }
  //runseq = directions(rlud,rlud+3);
  directions(rlud,rlud+3);
  oam_id = oam_meta_spr(actor_x[0], actor_y[0],  oam_id, playerRunSeq[runseq]);

  if (oam_id!=0) oam_hide_rest(oam_id);

}//** What the player can do**\\
int check0 = 0;
int gameset = 0;
  int x = 80;
  int y=80;  
void StartMenus(char oam_id)
{  
      pad = pad_poll(0);
  rlud = 8;
  switch(check0)
  {
    case 0: 
      BackGround(Menu0_pal, Menu0_rle);
      check0=3;
      break;
    case 1: 
      BackGround(Menu1_pal, Menu1_rle); 
      check0=4;      
      break;
    case 2: 
      BackGround(Menu2_pal, Menu2_rle); 
      check0=5;            
      break;
    case 3: 
      if(pad&PAD_START)
      {check0 = 1;}
      break;
    case 4: 
      directions(rlud,rlud+3);
      if(pad&PAD_UP && gameset != 0)
      {
        x=80;y=80;
        gameset =0;
      }
      else if(pad&PAD_DOWN && gameset != 1)
      {
        x=80;y=120;
        gameset = 1;
      }      
      if(pad&PAD_A)
      {if(gameset == 1)check0 = 6;
      else check0 = 2;}
      oam_id = oam_meta_spr(x, y,  oam_id, playerRunSeq[runseq]);
      
      break;
    case 5: 
      directions(rlud,rlud+3);
      if(pad&PAD_UP && gameset != 0)
      {
        x=80;y=80;
        gameset =0;
      }
      else if(pad&PAD_DOWN && gameset != 1)
      {
        x=80;y=120;
        gameset = 1;
      }      
      if(pad&PAD_A)
      {if(gameset == 1)check0 = 6;
      else check0 = 2;}
      oam_id = oam_meta_spr(x, y,  oam_id, playerRunSeq[runseq]);
      
      break;
  }
  if(check0 == 0)
    BackGround(Menu0_pal, Menu0_rle);
  else if(check0 == 0)
    BackGround(Menu0_pal, Menu0_rle);
  else if(check0 == 0)
    BackGround(Menu0_pal, Menu0_rle);


}
void MoveMap(int iMap2,int iMap3)
{
  iMap2 = iMap2*2;
  iOpen[0]=0;iOpen[1]=0;iOpen[2]=0;iOpen[3]=0;iOpen[4]=0;iOpen[5]=0;
  SetPlayer();
  iMap = iMap3;
  BackGround(WoldSeq[iMap2], WoldSeq[iMap2+1]);
} 
char oam_id1 = 0;
void setupgame(char oam_id)
{
  player_action(oam_id,0,1);  
  MapCollision(oam_id+64);
  //ObjectKey(oam_id0+80,36,220,66,190,0);    
  ppu_wait_frame();

}

void main(void)
{
  char oam_id0 = 0;	// sprite ID
  char oam_id1 = 0;	// sprite ID
  rlud = 0;
  setup_graphics();

  SetPlayer();
  SetEnemy();
  //      0*2=0,if 0 make it 1
  MoveMap(iMap,0);
  // infinite loop
  while(1) {
  StartMenus(oam_id0);


  }
}
void ObjectAffect(char oam_id,int iObject,int ikeys,int iDoor,int iMap1,int iMap2,int iMap3)
{
  if(iObject == 1)//if player stand on object open door
  {iOpen[iDoor] = 1;}  
  if(iObject == 2)//if object is a key 
  {   
    pickupkey(ikeys);//place key in player inventory 
  }  
  if(iObject == 3)//if object is a key 
  {       
    iOpen[iDoor] = 1;
  }


  else if(iMap1 == 1)
  {ikeys=ikeys;oam_id=oam_id;
   MoveMap(iMap2,iMap3);
  }
}
/* (x1,x2,y1,y2) location, actor(player or enemy), object 1 (1: pressure plate)(2:keys)(3:moveing block)(4:pickup item)
imap1(0 or 1)if we need to move, imap2 update map, imap3 what level
*/
void collision (char oam_id,int x1,int x2,int y1,int y2, int iActor,
                int iObject,int ikeys,int iDoor,int iMap1,int iMap2,int iMap3)
{  

  //top of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y1-1 &&  actor_y[iActor] <= y1 )&&  (actor_x[iActor] >= x1 && actor_x[iActor] <=x2))
  { 
    ObjectAffect(oam_id,iObject,ikeys,iDoor,iMap1,iMap2,iMap3);
    if(iObject == 0)
      actor_y[iActor] = actor_y[iActor]-1;

  }
  //bottom of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y2-1 && actor_y[iActor] <= y2) &&  (actor_x[iActor] >= x1 && actor_x[iActor] <=x2))
  {
    ObjectAffect(oam_id,iObject,ikeys,iDoor,iMap1,iMap2,iMap3);
    if(iObject == 0)
      actor_y[iActor] = actor_y[iActor]+1;

  }   
  //right side of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y1 && actor_y[iActor] <= y2) &&  (actor_x[iActor] >= x1-1 && actor_x[iActor] <=x2))
  {
    ObjectAffect(oam_id,iObject,ikeys,iDoor,iMap1,iMap2,iMap3);
    if(iObject == 0)
      actor_x[iActor] = actor_x[iActor]-1;

  }
  //right side of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y1 && actor_y[iActor] <= y2) &&  (actor_x[iActor] >= x2 && actor_x[iActor] <=x2+1))
  {
    ObjectAffect(oam_id,iObject,ikeys,iDoor,iMap1,iMap2,iMap3);
    if(iObject == 0)
      actor_x[iActor] = actor_x[iActor]+1;

  }

}
void lock(char oam_id,int x, int y, int ikeys,int icolor)
{
  object[ikeys+4].x = x;
  object[ikeys+4].y = y;    
  object[ikeys+4].id = ikeys;    
  object[ikeys+4].state = 40;
  oam_id = oam_spr(object[ikeys+4].x, object[ikeys+4].y, object[ikeys+4].state, icolor, oam_id);

  if(items[ikeys] == 1)
  {    
    collision (oam_id,object[ikeys+4].x-8,object[ikeys+4].x+8,object[ikeys+4].y-8,object[ikeys+4].y+16, 0,3,0,ikeys,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)

  }

}
void pickupkey(int ikeys)
{
  object[ikeys].x = 180+(ikeys*6);
  object[ikeys].y =18; 
  object[ikeys].id = ikeys;      
  object[ikeys].state = 24;
  items[ikeys]=1;
}
void ObjectKey(char oam_id,int x1,int x2,int y1,int y2,int inum,int icolor)
{
  if(items[inum] == 0)
  {

    object[inum].x = x1-x2;
    object[inum].y = y1-y2;    
    object[inum].id = inum;    
    object[inum].state = 24;
    items[inum] = 2;
  }    
  else if(items[inum] == 2)
  {    
    oam_id = oam_spr(object[inum].x, object[inum].y, object[inum].state, icolor, oam_id);  
    collision (oam_id,object[inum].x-16,object[inum].x+8,object[inum].y-16,object[inum].y+8, 0,2,inum,0,0,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
  }
  else
    oam_id = oam_spr(object[inum].x, object[inum].y, object[inum].state, icolor, oam_id); 
};



/*    lock(oam_id+20,5,0);
    ObjectKey(oam_id+16,x,x,y,y,0); */

/*void collision (char oam_id,int x1,int x2,int y1,int y2, int iActor,
                int iObject,int ikeys,int iDoor,int iMap1,int iMap2,int iMap3)*/
