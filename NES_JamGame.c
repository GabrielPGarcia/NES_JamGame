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

  0x31,0x30,0x1C,0x00,	// background palette 0
  0x0C,0x1C,0x1C,0x00,	// background palette 1
  0x06,0x16,0x16,0x00,	// background palette 2
  0x0B,0x00,0x1A,0x00,   // background palette 3

  0x3B,0x1B,0x0B,0x00,	// sprite palette 0
  0x0B,0x3B,0x1B,0x00,	// sprite palette 1

  0x36,0x16,0x06,0x00,	// sprite palette 2
  0x06,0x36,0x16,	// sprite palette 3
};
const char PALETTE2[32] = { 
  0x0F,			// screen color
  0x31,0x30,0x1C,0x00,	// background palette 0
  0x0C,0x1C,0x1C,0x00,	// background palette 1
  0x06,0x16,0x16,0x00,	// background palette 2
  0x0B,0x00,0x1A,0x00,   // background palette 3

  0x31,0x32,0x33,0x00,	// sprite palette 4
  0x33,0x31,0x32,0x00,	// sprite palette 5

  0x2C,0x0C,0x3C,0x00,	// sprite palette 8
  0x21,0x0C,0x3A	// sprite palette 9  

  };
const char PALETTE3[32] = { 
  0x0F,			// screen color
  0x31,0x30,0x1C,0x00,	// background palette 0
  0x0C,0x1C,0x1C,0x00,	// background palette 1
  0x06,0x16,0x16,0x00,	// background palette 2
  0x0B,0x00,0x1A,0x00,   // background palette 3
  0x31,0x30,0x3C,0x00,	// sprite palette 8
  0x3c,0x31,0x30,0x00,	// sprite palette 9  
  0x2C,0x0C,0x3C,0x00,	// sprite palette 6
  0x21,0x0C,0x3A	// sprite palette 7
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

DEF_METASPRITE_2x2(Left02, 0xdc, 2);
DEF_METASPRITE_2x2(Left12, 0xe0, 2);
DEF_METASPRITE_2x2(Left22, 0xdc, 3);
DEF_METASPRITE_2x2(Left32, 0xe0, 3);
DEF_METASPRITE_2x2_FLIP(Right02, 0xdc, 2);
DEF_METASPRITE_2x2_FLIP(Right12, 0xe0, 2);
DEF_METASPRITE_2x2_FLIP(Right22, 0xdc, 3);
DEF_METASPRITE_2x2_FLIP(Right32, 0xe0, 3);
DEF_METASPRITE_2x2(Up02, 0xe4, 2);
DEF_METASPRITE_2x2(Up12, 0xe8, 2);
DEF_METASPRITE_2x2(Up22, 0xe4, 3);
DEF_METASPRITE_2x2(Up32, 0xe8, 3);
DEF_METASPRITE_2x2_FLIPV(Down02, 0xe4, 2);
DEF_METASPRITE_2x2_FLIPV(Down12, 0xe8, 2);
DEF_METASPRITE_2x2_FLIPV(Down22, 0xe4, 3);
DEF_METASPRITE_2x2_FLIPV(Down32, 0xe8, 3);


DEF_METASPRITE_2x2(Door0, 0xc4, 0);
DEF_METASPRITE_2x2(Door1, 0xc4, 1);
DEF_METASPRITE_2x2(Door2, 0xc4, 2);
DEF_METASPRITE_2x2(Door3, 0xc4, 3);
DEF_METASPRITE_2x2(Platform0, 0x80, 0);
DEF_METASPRITE_2x2(Platform1, 0xc4, 1);

const unsigned char* const playerRunSeq[16] = {
  Left0,	Left1,	Left2,	Left3,
  Right0,	Right1,	Right2,	Right3,
  Up0,		Up1,	Up2,	Up3,
  Down0,	Down1,	Down2,	Down3,
};
const unsigned char* const playerRunSeq2[16] = {
  Left02,	Left12,	Left22,	Left32,
  Right02,	Right12,	Right22,	Right32,
  Up02,		Up12,	Up22,	Up32,
  Down02,	Down12,	Down22,	Down32,
};
const unsigned char* const Doors[4] = {
  Door0,	Door1,	Door2,	Door3
  };

const unsigned char* const WoldSeq[16] = {
  World_0_pal,World_0_rle,
  World_1_pal,World_1_rle,
  World_1_pal,World_1_rle
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
Objects object[20];//0-3 keys  4-8 locks  9-15 potion 16-17 potion

void collision(char oam_id,int x1,int x2,int y1,int y2, int iActor,
               int iObject,int ikeys,int iDoor,bool MoveToLevel,int iNextLevelMap,int iNextLevel);
void ObjectKey(char oam_id,int x,int y,int inum,int icolor);
void PickUpObject(char oam_id,int x1,int x2,int y1,int y2,int icolor);
void pickupkey(int ikeys);
void lock(char oam_id,int x, int y, int ikeys,int icolor);
void MoveMap(int iMap2,int iNextLevel);
void setupgame(char oam_id);
void ObjectPotion(char oam_id,int x,int y,int iPotion,int icolor);
void pickupPotion(int ikeys);
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

int iGameState = 0;//what state were going to be
int iGamePath = 0;//what path the player picks
int iGameType = 0;//what Type the player picks
int iNPotion = 0;//N number of potions
int iNLives = 6;//N number of lives
int x = 80;	    //x of player path
int y = 80;      // y of player path 
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
/*(char oam_id,int x1,int x2,int y1,int y2, int iActor,
                int iObject,int ikeys,int iDoor,bool MoveToLevel,int iNextLevelMap,int iNextLevel)
{  
*/
//oam_id,x1,x2,y1,y2,actor,object[0-3],ikey id, iDoor, MoveToLevel,iNextLevelMap,iNextLevel
void Door(char oam_id,int ix, int iy,int id,int icolor)
{
  if(iOpen[id] != 1)
  {    
    collision (oam_id,ix-10,ix+10,iy-15,iy+15, 0,0,0,0,FALSE,0,0);
    oam_id = oam_meta_spr(ix, iy,  oam_id,Doors[icolor]);
  }
}
void levelcopie(char oam_id, int actor)
{
  //oam_id,x1,x2,y1,y2,actor,object[0-3],ikey id, iDoor, MoveToLevel,iNextLevelMap,iNextLevel
  //ikey id if there is a key
  //iDoor if collision affects a door
  //MoveToLevel boolean check if we moving
  //NextLevelMap where to we are moving 
  //type of level we are going to 
  collision (oam_id,0,16,46,172, actor,0,0,0,FALSE,0,0);
  collision (oam_id,208,220,46,172, actor,0,0,0,FALSE,0,0);
  collision (oam_id,120,220,0,62, actor,0,0,0,FALSE,0,0);
  collision (oam_id,0,104,0,62, actor,0,0,0,FALSE,0,0);
  collision (oam_id,0,250,192,221, actor,0,0,0,FALSE,0,0);
  collision (oam_id,0,48,162,198, actor,0,0,0,FALSE,0,0);
  collision (oam_id,176,250,162,198, actor,0,0,0,FALSE,0,0);

}
void MapTower (char oam_id)
{ 
  if(iMap == 0)
  {
    levelcopie(oam_id,0);
    oam_id = oam_meta_spr(32, 95,  oam_id,Platform0);
    collision (oam_id,32-8,32+8,95-8,95+12, 0,1,0,0,FALSE,0,0);

    Door(oam_id,112,47,0,0); 
    collision (oam_id,80,158,10,12, 0,0,0,0,TRUE,0,1);

  } 
  if(iMap == 1)
  {      
    levelcopie(oam_id,0); 

    ObjectKey(oam_id+16,79,180,0,4);//oam_id,x1,x2,y1,y2,key id,key color
    lock(oam_id+24,180,52,0,4);//oam_id,x,y,key id,lock color
    Door(oam_id,112,47,0,0); //oam_id,x,y,key id,door color[0-3]
    collision (oam_id,80,158,10,12, 0,0,0,0,TRUE,0,2);
  }
  if(iMap == 2)
  {      
    levelcopie(oam_id,0); 

    ObjectPotion(oam_id+16,89,170,0,4);//oam_id,x1,x2,y1,y2,key id,key color
    //ObjectKey(oam_id+16,79,180,0,4);//oam_id,x1,x2,y1,y2,key id,key color
    //lock(oam_id+24,180,52,0,4);//oam_id,x,y,key id,lock color
    Door(oam_id,112,47,0,0); //oam_id,x,y,key id,door color[0-3]
    collision (oam_id,80,158,10,12, 0,0,0,0,TRUE,0,2);
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

void StartMenus(char oam_id)
{  
  pad = pad_trigger(0);//get one input from player
  rlud = 8;		//type of fairy animation
  switch(iGameState)	//switch game state
  {
    case 0://update background and move to case: 3
      //to provent background from loading in a loop
      BackGround(Menu0_pal, Menu0_rle); iGameState = 3;
      break;
    case 1: //update background and move to case: 4
      //to provent background from loading in a loop
      BackGround(Menu1_pal, Menu1_rle); iGameState = 4; 
      x = 75;y = 80;
      break;
    case 2: //update background and move to case: 3
      //to provent background from loading in a loop
      BackGround(Menu2_pal, Menu2_rle); iGameState = 5;   
      x = 85; y= 45;  //update x and y location 
      iGameType = 0;
      iGamePath = 0;  //save path chosen
      break;
    case 3: //stay in start screen till Start as been pressed
      if(pad&PAD_START)
      {iGameState = 1;}//go to screen update 
      break;
    case 4: 
      directions(rlud,rlud+3); //this is the animation of fairy
      if(pad&PAD_UP && iGamePath != 0)//up input with path 
      {y=80;iGamePath =0;}//the Tower path
      else if(pad&PAD_DOWN && iGamePath != 1)
      {y=120;iGamePath = 1;}//the Story path      
      if(pad&PAD_A)//send to character select since the path was chosen 
      {iGameState = 2;}
      //fairy's animation and location
      oam_id = oam_meta_spr(x, y,  oam_id, playerRunSeq[runseq]);
      ppu_wait_frame();
      break;
    case 5: 
      directions(rlud,rlud+3); //this is the animation of fairy
      if(pad&PAD_UP && iGameType != 0)//iGameType is character's color
      {
        y=y-32;//move up
        iGameType = iGameType - 1;//update character color type
      }
      else if(pad&PAD_DOWN && iGameType != 4)//iGameType is character's color
      {
        y = y+32;//move down
        iGameType = iGameType + 1;//update character color type
      }
      if(iGameType == 0){//color update 0 = green
        pal_all(PALETTE);
        oam_id = oam_meta_spr(x, y,  oam_id, playerRunSeq[runseq]);
      }if(iGameType == 1){//color update 1 = red
        pal_all(PALETTE);
        oam_id = oam_meta_spr(x, y,  oam_id, playerRunSeq2[runseq]);
      }if(iGameType == 2){//color update 2 = pink
        pal_all(PALETTE2);
        oam_id = oam_meta_spr(x, y,  oam_id, playerRunSeq[runseq]);
      }if(iGameType == 3){//color update 1 = blue
        pal_all(PALETTE2);
        oam_id = oam_meta_spr(x, y,  oam_id, playerRunSeq2[runseq]);
      }if(iGameType == 4){//color update 1 = white/blue
        pal_all(PALETTE3);
        oam_id = oam_meta_spr(x, y,  oam_id, playerRunSeq[runseq]);
      }if(pad&PAD_B)//iGameType is character's color
      {iGameState = 1;}
      if(pad&PAD_A && iGamePath == 0)
      {iGameState = 6;  MoveMap(iMap,2);}//go to screen update 
      if(pad&PAD_A && iGamePath == 1)
      {iGameState = 7;  MoveMap(iMap,0);}//go to screen update 
      ppu_wait_frame();
      break;
    case 6: //Tower path 
      setupgame(oam_id);
      break;
    case 7: //Story path
      break;
  }


}
void playerlives(char oam_id,int ilives)
{
  int id = ilives+15;
  object[id].x = 15 * ilives;
  object[id].y = 15;    
  object[id].id = ilives;    
  object[id].state = 25;

  oam_id = oam_spr(object[id].x, object[id].y, object[id].state, 4, oam_id); 
}void playerhalflives(char oam_id,int ilives)
{
  int id = ilives+15;
  object[id].x = 15 + ilives;
  object[id].y = 15;    
  object[id].id = ilives;    
  object[id].state = 41;
  oam_id = oam_spr(object[id].x, object[id].y, object[id].state, 8, oam_id); 
}
void setPlayerLives(char oam_id)
{
oam_id=oam_id;
}
void MoveMap(int iNextLevelMap,int iNextLevel)
{
  int iCount;//count down for resetting
  iNextLevelMap = iNextLevelMap*2;//this is for the seq 0-1, 2-3,...
  for(iCount = 0; iCount <9;iCount++)
    items[iCount]=0;//reset
  for(iCount = 0; iCount <5;iCount++)
    iOpen[iCount]=0;//reset
  SetPlayer();//re-setplayer
  iMap = iNextLevel;//update level
  BackGround(WoldSeq[iNextLevelMap], WoldSeq[iNextLevelMap+1]);//change background
} 

void setupgame(char oam_id)
{
  player_action(oam_id,0,1); 
  if(iMap >=2)
    setPlayerLives(oam_id);
  MapTower(oam_id+64);    
  ppu_wait_frame();

}

void main(void)
{
  char oam_id = 0;	// sprite ID
  rlud = 0;        // save input [right left up down]
  setup_graphics();//set graphics
  // infinite loop
  while(1) {
    StartMenus(oam_id);//start at StartMenus state
  }
}
void ObjectAffect(int iObject,int ikeys,int iDoor,bool MoveToLevel,int iNextLevelMap,int iNextLevel)
{
  if(iObject == 1)//if player stand on object open door
  {iOpen[iDoor] = 1;//open door
  }if(iObject == 2)//if object is a key 
  {pickupkey(ikeys);//place key in player inventory 
  }if(iObject == 3)//if object is a key 
  {iOpen[iDoor] = 1;
  }if(iObject == 4)//if object is a key 
  {iNPotion = iNPotion+1;pickupPotion(ikeys);//place key in player inventory 
  }else if(MoveToLevel == TRUE)
  {
    MoveMap(iNextLevelMap,iNextLevel);
  }
}

void collision (char oam_id,int x1,int x2,int y1,int y2, int iActor,
                int iObject,int ikeys,int iDoor,bool MoveToLevel,int iNextLevelMap,int iNextLevel)
{  
  //oam_id,x1,x2,y1,y2,actor,object[0-3],ikey id, iDoor, MoveToLevel,iNextLevelMap,iNextLevel
  //ikey id if there is a key
  //iDoor if collision affects a door
  //MoveToLevel boolean check if we moving
  //NextLevelMap where to we are moving 
  //type of level we are going to 

  //top of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y1-1 &&  actor_y[iActor] <= y1 )&&  (actor_x[iActor] >= x1 && actor_x[iActor] <=x2))
  { 
    ObjectAffect(iObject,ikeys,iDoor,MoveToLevel,iNextLevelMap, iNextLevel);
    if(iObject == 0)
      actor_y[iActor] = actor_y[iActor]-1;


  }
  //bottom of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y2-1 && actor_y[iActor] <= y2) &&  (actor_x[iActor] >= x1 && actor_x[iActor] <=x2))
  {
    ObjectAffect(iObject,ikeys,iDoor,MoveToLevel,iNextLevelMap, iNextLevel);
    if(iObject == 0)
      actor_y[iActor] = actor_y[iActor]+1;
  }   
  //right side of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y1 && actor_y[iActor] <= y2) &&  (actor_x[iActor] >= x1-1 && actor_x[iActor] <=x2))
  {
    ObjectAffect(iObject,ikeys,iDoor,MoveToLevel,iNextLevelMap, iNextLevel);
    if(iObject == 0)
      actor_x[iActor] = actor_x[iActor]-1;
  }
  //right side of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y1 && actor_y[iActor] <= y2) &&  (actor_x[iActor] >= x2 && actor_x[iActor] <=x2+1))
  {
    ObjectAffect(iObject,ikeys,iDoor,MoveToLevel,iNextLevelMap, iNextLevel);
    if(iObject == 0)
      actor_x[iActor] = actor_x[iActor]+1;
  } 
  if(test==0){
    oam_id = oam_spr(x1+16, y1+12, 1, 0, oam_id+8);  
    oam_id = oam_spr(x1+16, y2-8, 1, 0, oam_id+8);  
    oam_id = oam_spr(x2-8, y1+12, 1, 0, oam_id+8);  
    oam_id = oam_spr(x2-8, y2-8, 1, 0, oam_id+8); } 

}
void lock(char oam_id,int x, int y, int ikeys,int icolor)
{
  int id = ikeys+4;
  object[id].x = x;
  object[id].y = y;    
  object[id].id = ikeys;    
  object[id].state = 40;
  oam_id = oam_spr(object[id].x, object[id].y, object[id].state, icolor, oam_id);

  if(items[ikeys] == 1)
  {    
    collision (oam_id,object[id].x-20,object[id].x+15,object[id].y-16,object[id].y+16, 0,3,0,ikeys,FALSE,0,0);
  }

}
void pickupkey(int ikeys)
{
  object[ikeys].x = 188+(ikeys*6);
  object[ikeys].y =18;  
  items[ikeys]=1;
}
void ObjectKey(char oam_id,int x,int y,int ikeyid,int icolor)
{
  if(items[ikeyid] == 0)
  {

    object[ikeyid].x = x;
    object[ikeyid].y = y;    
    object[ikeyid].id = ikeyid;    
    object[ikeyid].state = 24;
    items[ikeyid] = 2;
  }    
  else if(items[ikeyid] == 2)
  {    
    oam_id = oam_spr(object[ikeyid].x, object[ikeyid].y, object[ikeyid].state, icolor, oam_id);  
    collision (oam_id,object[ikeyid].x-16,object[ikeyid].x+8,object[ikeyid].y-16,object[ikeyid].y+8, 0,2,ikeyid,0,FALSE,0,0);//(x1,x2,y1,y2,actor,object,map border,new map)
  }
  else
    oam_id = oam_spr(object[ikeyid].x, object[ikeyid].y, object[ikeyid].state, icolor, oam_id); 
};
void pickupPotion(int iPotion)
{
  object[iPotion].x = 180;
  object[iPotion].y =18;       
  object[iPotion].state = 0;
}
void ObjectPotion(char oam_id,int x,int y,int iPotion,int icolor)
{
  iPotion = iPotion+9;
  if(items[iPotion] == 0)
  {
    object[iPotion].x = x;
    object[iPotion].y = y;    
    object[iPotion].id = iPotion;    
    object[iPotion].state = 26;
    //oam_id = oam_spr(object[iPotion].x, object[iPotion].y, object[iPotion].state, icolor, oam_id); 
  }    
  oam_id = oam_spr(object[iPotion].x, object[iPotion].y, object[iPotion].state, icolor, oam_id);  
  collision (oam_id,object[iPotion].x-16,object[iPotion].x+8,object[iPotion].y-16,object[iPotion].y+8, 0,4,iPotion,0,FALSE,0,0);
};


