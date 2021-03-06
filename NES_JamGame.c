#include <stdlib.h>
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

extern const byte World_2_pal[16];
extern const byte World_2_rle[];

extern const byte World_3_pal[16];
extern const byte World_3_rle[];

extern const byte World_4_pal[16];
extern const byte World_4_rle[];

extern const byte World_5_pal[16];
extern const byte World_5_rle[];

extern const byte Menu0_pal[16];
extern const byte Menu0_rle[];

extern const byte Menu1_pal[16];
extern const byte Menu1_rle[];

extern const byte Menu2_pal[16];
extern const byte Menu2_rle[];

extern const byte fight1_pal[16];
extern const byte fight1_rle[];

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
  0x0F,0x00,0x31,0x00,	// sprite palette 1

  0x0F,0x0C,0x1C,0x00,	// sprite palette 2
  0x0F,0x04,0x24,	// sprite palette 3
};const char PALETTE1[32] = { 
  0x0F,			// screen color
  0x31,0x30,0x1C,0x00,	// background palette 0
  0x0C,0x1C,0x1C,0x00,	// background palette 1
  0x06,0x16,0x16,0x00,	// background palette 2
  0x0B,0x00,0x1A,0x00,   // background palette 3

  0x36,0x16,0x06,0x00,	// sprite palette 0
  0x0F,0x00,0x31,0x00,	// sprite palette 1

  0x0F,0x0C,0x1C,0x00,	// sprite palette 2
  0x0F,0x04,0x24,	// sprite palette 3

};const char PALETTE2[32] = { 
  0x0F,			// screen color
  0x31,0x30,0x1C,0x00,	// background palette 0
  0x0C,0x1C,0x1C,0x00,	// background palette 1
  0x06,0x16,0x16,0x00,	// background palette 2
  0x0B,0x00,0x1A,0x00,   // background palette 3

  0x31,0x32,0x33,0x00,	// sprite palette 0
  0x0F,0x00,0x31,0x00,	// sprite palette 1

  0x0F,0x0C,0x1C,0x00,	// sprite palette 2
  0x0F,0x04,0x24,	// sprite palette 3

};const char PALETTE3[32] = { 
  0x0F,			// screen color
  0x31,0x30,0x1C,0x00,	// background palette 0
  0x0C,0x1C,0x1C,0x00,	// background palette 1
  0x06,0x16,0x16,0x00,	// background palette 2
  0x0B,0x00,0x1A,0x00,   // background palette 3

  0x2C,0x0C,0x3C,0x00,	// sprite palette 0
  0x0F,0x00,0x31,0x00,	// sprite palette 1

  0x0F,0x0C,0x1C,0x00,	// sprite palette 2
  0x0F,0x04,0x24,	// sprite palette 3

};const char PALETTE4[32] = { 
  0x0F,			// screen color
  0x31,0x30,0x1C,0x00,	// background palette 0
  0x0C,0x1C,0x1C,0x00,	// background palette 1
  0x06,0x16,0x16,0x00,	// background palette 2
  0x0B,0x00,0x1A,0x00,   // background palette 3

  0x31,0x30,0x3C,0x00,	// sprite palette 0
  0x0F,0x00,0x31,0x00,	// sprite palette 1

  0x0F,0x0C,0x1C,0x00,	// sprite palette 2
  0x0F,0x04,0x24,	// sprite palette 3

};
#define NUM_ACTORS 20
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
DEF_METASPRITE_2x2_FLIP(Right0, 0xdc, 0);
DEF_METASPRITE_2x2_FLIP(Right1, 0xe0, 0);
DEF_METASPRITE_2x2(Up0, 0xe4, 0);
DEF_METASPRITE_2x2(Up1, 0xe8, 0);
DEF_METASPRITE_2x2_FLIPV(Down0, 0xe4, 0);
DEF_METASPRITE_2x2_FLIPV(Down1, 0xe8, 0);

DEF_METASPRITE_2x2(Left0e1, 0xdc, 1);
DEF_METASPRITE_2x2(Left1e1, 0xe0, 1);
DEF_METASPRITE_2x2_FLIP(Right0e1, 0xdc, 1);
DEF_METASPRITE_2x2_FLIP(Right1e1, 0xe0, 1);
DEF_METASPRITE_2x2(Up0e1, 0xe4, 1);
DEF_METASPRITE_2x2(Up1e1, 0xe8, 1);
DEF_METASPRITE_2x2_FLIPV(Down0e1, 0xe4, 1);
DEF_METASPRITE_2x2_FLIPV(Down1e1, 0xe8, 1);

DEF_METASPRITE_2x2(Left0e2, 0xdc, 2);
DEF_METASPRITE_2x2(Left1e2, 0xe0, 2);
DEF_METASPRITE_2x2_FLIP(Right0e2, 0xdc, 2);
DEF_METASPRITE_2x2_FLIP(Right1e2, 0xe0, 2);
DEF_METASPRITE_2x2(Up0e2, 0xe4, 2);
DEF_METASPRITE_2x2(Up1e2, 0xe8, 2);
DEF_METASPRITE_2x2_FLIPV(Down0e2, 0xe4, 2);
DEF_METASPRITE_2x2_FLIPV(Down1e2, 0xe8, 2);

DEF_METASPRITE_2x2(Left0e3, 0xdc, 3);
DEF_METASPRITE_2x2(Left1e3, 0xe0, 3);
DEF_METASPRITE_2x2_FLIP(Right0e3, 0xdc, 3);
DEF_METASPRITE_2x2_FLIP(Right1e3, 0xe0, 3);
DEF_METASPRITE_2x2(Up0e3, 0xe4, 3);
DEF_METASPRITE_2x2(Up1e3, 0xe8, 3);
DEF_METASPRITE_2x2_FLIPV(Down0e3, 0xe4, 3);
DEF_METASPRITE_2x2_FLIPV(Down1e3, 0xe8, 3);

DEF_METASPRITE_2x2(Platform0, 0x80, 2);
DEF_METASPRITE_2x2(Platform1, 0x80, 3);
DEF_METASPRITE_2x2(Door0, 0xc4, 0);
DEF_METASPRITE_2x2(Door1, 0xc4, 1);
DEF_METASPRITE_2x2(Door2, 0xc4, 2);
DEF_METASPRITE_2x2(Door3, 0xc4, 3);
DEF_METASPRITE_2x2(Target, 0x84, 3);

const unsigned char* const playerRunSeq[8] = {
  Up0,		Up1,
  Down0,	Down1,
  Left0,	Left1,
  Right0,	Right1
  };
const unsigned char* const playerRunSeqe1[8] = {
  Up0e1,		Up1e1,
  Down0e1,	Down1e1,
  Left0e1,	Left1e1,
  Right0e1,	Right1e1
  };const unsigned char* const playerRunSeqe2[8] = {
  Up0e2,		Up1e2,
  Down0e2,	Down1e2,
  Left0e2,	Left1e2,
  Right0e2,	Right1e2
  };const unsigned char* const playerRunSeqe3[8] = {
  Up0e3,		Up1e3,
  Down0e3,	Down1e3,
  Left0e3,	Left1e3,
  Right0e3,	Right1e3
  };
const unsigned char* const WoldSeq[12] = {
  World_0_pal,World_0_rle,
  World_1_pal,World_1_rle,
  World_2_pal,World_2_rle,
  World_3_pal,World_3_rle,
  World_4_pal,World_4_rle,
  World_5_pal,World_5_rle
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
Objects object[20];//0-3 keys  4-8 locks  9-10 potion 11-20 health

void collision(char oam_id,int x1,int x2,int y1,int y2, int iActor,
               int iObject,int ikeys,int iDoor,bool MoveToLevel,int iNextLevelMap,int iNextLevel);
void ObjectKey(char oam_id,int x,int y,int inum,int icolor);
void PickUpObject(char oam_id,int x1,int x2,int y1,int y2,int icolor);
void pickupkey(int ikeys);
void lock(char oam_id,int x, int y, int ikeys,int icolor);
void MoveMap(int iMap2,int iNextLevel);
void enemy_action(char oam_id,int ienemyid,int iColor);
void fightaction(char oam_id);
void fighting(char oam_id);
void passkey(char oam_id,int iColor,int id);
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


int iGameState;	//what state were going to be
int iGamePath;	//what path the player picks
int iGameType;	//what Type the player picks
int iNPotion = 3;	//N number of potions
int iPHealth = 20;	//player's Health
int iPLevel = 6;	//player's level
int iPDamage = 6;	//player's damage
int iPMoves = 6;	//player's 
int iELives = 20;	//N number of lives
int iELevel = 10;	//N numver of levels
int iEDamage = 12;	//N numver of damage
int x = 80;	//x of player path
int y = 80;     // y of player path 
int iRandx;	// random num x
int iRandy;    	// random num y
int iRandx1;	// random num x
int iRandy1;  	// random num y

char pad;	//input pad
byte runseq;	//actor next Seq
byte runseqe;	//actor next Seq enemy
int items[9] = {0,0,0,0,0,0,0,0,0};	//keys/potions
int iEnemyid;
int iEColor;
int ienemy[20] = {0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0};
int i;
int iOpen[5] = {0,0,0,0,0}; 		//door on/off
int rlud; 	// right,lest,up, and Down save last key input
int test = 0; 	//display collision 
int iCurrentLevel = 0; 	//the level the player will be
int keyplaced = 0; 	//setup key
int actorlastx;	//
int actorlasty;	//

bool bEnemyAttack;	//set up next enemy attack
int iPlayerAction;	//get player's next action
int iPlayerChoice;      //set player's action
int iPlayerY = 2;	//set players y location in fight
int iPlayerX = 3;	//set players x location in fight
int iMapPAL;		//save map
int iMapRLE;		//save map

int iMaxSeq = 10;
int iStartSeq = 0;    //actor Seq
void setupeverything()
{
  iNPotion = 3;	//N number of potions
  iPHealth = 20;	//player's Health
  iPLevel = 6;	//player's level
  iPDamage = 6;	//player's damage
  iPMoves = 6;	//player's 
  iELives = 20;	//N number of lives
  iELevel = 10;	//N numver of levels
  iEDamage = 12;	//N numver of damage
  x = 80;	//x of player path
  y = 80;     // y of player path 
  for(i = 0; i < 9; i++)
    items[i] = 0;	//keys/potions
  for(i = 0; i < 20; i++)
    ienemy[i] = 0;
  for(i = 0; i < 5; i++)
    iOpen[i] = 0; 		//door on/off
  test = 0; 	//display collision 
  iCurrentLevel = 0; 	//the level the player will be
  keyplaced = 0; 	//setup key
  iPlayerY = 2;	//set players y location in fight
  iPlayerX = 3;	//set players x location in fight
  iMaxSeq = 10;
  iStartSeq = 0;    //actor Seq
}
/*void SetEnemy(int id,int x, int y, int dx, int dy, int level)
{
  actor_x[id] = x;
  actor_y[id] = y;
  actor_dx[id] = dx;
  actor_dy[id] = dy;
  iELives = 3*level;	//N number of lives
  iELevel = level;	//N numver of levels
  iEDamage = 2*level;	//N numver of damage

}*/
void SetEnemy(int id,int x, int y, int dx, int dy)
{
  actor_x[id] = x;
  actor_y[id] = y;
  actor_dx[id] = dx;
  actor_dy[id] = dy;

}
void SetPlayer(int ix, int iy)
{
  actor_x[0] = ix;
  actor_y[0] = iy;
  actor_dx[0] = 0;
  actor_dy[0] = 0;
}

//oam_id,x1,x2,y1,y2,actor,object[0-3],ikey id, iDoor, MoveToLevel,iNextLevelMap,iNextLevel
void Door(char oam_id,int ix, int iy,int id,int icolor)
{
  if(iOpen[id] != 1)
  {    
    collision (oam_id,ix-10,ix+10,iy-15,iy+15, 0,0,0,0,FALSE,0,0);
    if(icolor == 0)
      oam_id = oam_meta_spr(ix, iy,  oam_id,Door0);
    if(icolor == 1)
      oam_id = oam_meta_spr(ix, iy,  oam_id,Door1);
    if(icolor == 2)
      oam_id = oam_meta_spr(ix, iy,  oam_id,Door2);
    if(icolor == 3)
      oam_id = oam_meta_spr(ix, iy,  oam_id,Door3);
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
void ColThree(char oam_id, int actor)
{      collision (oam_id,0,16,46,172, actor,0,0,0,FALSE,0,0);
 collision (oam_id,204,248,46,172, actor,0,0,0,FALSE,0,0);
 collision (oam_id,0,60,110,140, actor,0,0,0,FALSE,0,0);
 collision (oam_id,172,232,110,140, actor,0,0,0,FALSE,0,0);
 collision (oam_id,0,48,158,206, actor,0,0,0,FALSE,0,0);
 collision (oam_id,172,248,158,206, actor,0,0,0,FALSE,0,0);
 collision (oam_id,48,172,192,232, actor,0,0,0,FALSE,0,0);
 collision (oam_id,160,248,110,140, 0,0,0,0,FALSE,0,0);
 collision (oam_id,30,96,80,140, actor,0,0,0,FALSE,0,0);
 collision (oam_id,128,192,80,140, 0,0,0,0,FALSE,0,0);
 collision (oam_id,0,104,0,64, actor,0,0,0,FALSE,0,0);
 collision (oam_id,120,240,0,64, actor,0,0,0,FALSE,0,0);
}
void ColFour(char oam_id, int actor)
{      collision (oam_id,0,16,46,140, actor,0,0,0,FALSE,0,0);
 collision (oam_id,204,248,46,172, actor,0,0,0,FALSE,0,0);
 collision (oam_id,0,248,158,206, actor,0,0,0,FALSE,0,0);
 collision (oam_id,0,240,0,64, actor,0,0,0,FALSE,0,0);
 collision (oam_id,112,144,0,112, actor,0,0,0,FALSE,0,0);
 collision (oam_id,112,144,128,188, actor,0,0,0,FALSE,0,0);}
void ColFive(char oam_id, int actor)
{
  collision (oam_id,0,16,46,222, actor,0,0,0,FALSE,0,0);
  collision (oam_id,208,248,46,146, actor,0,0,0,FALSE,0,0);
  collision (oam_id,32,248,178,222, actor,0,0,0,FALSE,0,0);
  collision (oam_id,180,248,162,206, actor,0,0,0,FALSE,0,0);
  collision (oam_id,0,240,0,64, actor,0,0,0,FALSE,0,0);
  collision (oam_id,32,80,0,160, actor,0,0,0,FALSE,0,0);
  collision (oam_id,142,192,82,180, actor,0,0,0,FALSE,0,0);
  collision (oam_id,96,192,82,128, actor,0,0,0,FALSE,0,0);
  collision (oam_id,96,170,146,188, actor,0,0,0,FALSE,0,0);

}
void ColSix(char oam_id, int actor)
{
  collision (oam_id,0,16,0,222, actor,0,0,0,FALSE,0,0);
  collision (oam_id,208,248,46,222, actor,0,0,0,FALSE,0,0);
  collision (oam_id,0,248,192,222, actor,0,0,0,FALSE,0,0);
  collision (oam_id,32,240,0,64, actor,0,0,0,FALSE,0,0);
}
void MapTower (char oam_id)
{ 
  switch(iCurrentLevel)	//switch game state
  {
    case 0:
      levelcopie(oam_id,0);
      oam_id = oam_meta_spr(32, 95,  oam_id,Platform0);
      collision (oam_id,32-8,32+8,95-8,95+12, 0,1,0,0,FALSE,0,0);

      Door(oam_id,112,47,0,2); 
      collision (oam_id,80,158,10,12, 0,0,0,0,TRUE,0,1);
      break;

    case 1: 
      levelcopie(oam_id,0); 

      Door(oam_id,112,47,0,1); 
      ObjectKey(oam_id+16,70,180,0,4);//oam_id,x1,x2,y1,y2,key id,key color    
      lock(oam_id+68,140,52,0,4);//oam_id,x,y,key id,lock color    
      collision (oam_id,80,158,10,12, 0,0,0,0,TRUE,0,2);
      break;
    case 2:   
      levelcopie(oam_id,0); 
      levelcopie(oam_id,1); 
      iEnemyid = 1;
      iEColor = 1;

      if(ienemy[1] == 0)
        enemy_action(oam_id+32,iEnemyid,iEColor); //set enemy

      collision (oam_id,80,158,10,12, 0,0,0,0,TRUE,1,3);
      break;
    case 3:  

      iEnemyid = 1;
      iEColor = 2;

      if(ienemy[1] == 0)
        enemy_action(oam_id,iEnemyid,iEColor); //set enemy

      ObjectKey(oam_id+32,34,116,0,0);//oam_id,x1,y1,key id,key color
      if(items[0]==1)passkey(oam_id+32,0,0);
      if(items[1]==1)passkey(oam_id+40,1,1);
      if(items[2]==1)passkey(oam_id+48,2,2);
      lock(oam_id+56,164,52,1,5);//oam_id,x,y,key id,lock color 
      lock(oam_id+64,68,52,2,6);//oam_id,x,y,key id,lock color
      lock(oam_id+72,128,52,0,0);//oam_id,x,y,key id,lock color


      Door(oam_id+104,112,47,0,0); //oam_id,x,y,key id,door color[0-3]
      Door(oam_id+136,176,71,1,1); //oam_id,x,y,key id,door color[0-3]
      Door(oam_id+168,48,71,2,2); //oam_id,x,y,key id,door color[0-3]   

      collision (NULL,80,158,10,12, 0,0,0,0,TRUE,5,7);
      collision (NULL,200,250,130,180, 0,0,0,0,TRUE,2,4);
      collision (NULL,0,32,130,180, 0,0,0,0,TRUE,3,5);
      ColThree(NULL,0);
      ColThree(NULL,1);
      break;
    case 4:   

      iEnemyid = 2;
      iEColor = 1;


      if(ienemy[iEnemyid] == 0)
        enemy_action(oam_id,iEnemyid,iEColor); //set enemy
      else if(ienemy[3] == 0)
      {
        iEnemyid = 3;
        iEColor = 2;

        if(ienemy[iEnemyid] == 0)
          enemy_action(oam_id,iEnemyid,iEColor); //set enemy
      }

      ObjectKey(oam_id+40,198,116,1,1);//oam_id,x1,x2,y1,y2,key id,key color 

      if(items[0]==1)passkey(oam_id+32,0,0);
      if(items[1]==1)passkey(oam_id+40,1,1);
      if(items[2]==1)passkey(oam_id+48,2,2);

      collision (oam_id,0,16,130,180, 0,0,0,0,TRUE,1,3);
      ColFour(oam_id,0);
      ColFour(NULL,2);
      break;
    case 5: 
      iEnemyid = 4;
      iEColor = 2;
      if(ienemy[iEnemyid] == 0)
        enemy_action(oam_id,iEnemyid,iEColor); //set enemy
      else       oam_id = oam_meta_spr(16, 63,  oam_id,Platform1);

      if(items[0]==1)passkey(oam_id+32,0,0);
      if(items[1]==1)passkey(oam_id+40,1,1);
      if(items[2]==1)passkey(oam_id+48,2,2);
      lock(oam_id+56,158,148,1,1);//oam_id,x,y,key id,lock color 
      Door(oam_id+88,32,143,1,1); //oam_id,x,y,key id,door color[0-3]
      Door(oam_id+120,16,143,1,1); //oam_id,x,y,key id,door color[0-3] 
      Door(oam_id+152,32,191,3,3); //oam_id,x,y,key id,door color[0-3]
      Door(oam_id+184,16,191,3,3); //oam_id,x,y,key id,door color[0-3]      

      collision (oam_id,200,250,130,180, 0,0,0,0,TRUE,1,3);
      collision (oam_id,0,62,190,222, 0,0,0,0,TRUE,4,6);
      collision (oam_id,16-8,16+8,63-8,63+12, 0,1,0,3,FALSE,0,0);

      ColFive(oam_id,0);
      ColFive(NULL,4);

      break;
    case 6: 
      iEnemyid = 5;
      iEColor = 0;

      if(ienemy[iEnemyid] == 0)
        enemy_action(oam_id,iEnemyid,iEColor); //set enemy
      else        ObjectKey(oam_id+48,198,116,2,2);

      if(items[0]==1)passkey(oam_id+32,0,0);
      if(items[1]==1)passkey(oam_id+40,1,1);
      if(items[2]==1)passkey(oam_id+48,2,2);
      collision (oam_id,0,62,190,222, 0,0,0,0,TRUE,3,5);
      ColSix(oam_id,0);
      ColSix(oam_id,2);
      break;    
    case 7: 
      collision (oam_id,0,188,0,140, 0,0,0,0,FALSE,0,0);
      collision (oam_id,0,96,0,222, 0,0,0,0,FALSE,0,0);
      collision (oam_id,32,248,200,222, 0,0,0,0,FALSE,0,0);
      collision (oam_id,130,248,0,222, 0,0,0,0,FALSE,0,0);
      pad = pad_poll(0);
      if (pad&PAD_START)
      {setup_graphics(); iGameState = 0;iGamePath = 0;iGameType=0;
       iCurrentLevel = 0;}
      break;    
    case 8: 

      setup_graphics(); iGameState = 0;iGamePath = 0;iGameType=0;
      iCurrentLevel = 0;
      break;
  }
}

//** actors animation and directions facing**\\
void directions()
{  
  if(runseq == rlud+1 )
    runseq = rlud;
  else 
    runseq++;
}
void directions2(int iStart,int iEnd)
{  
  if(runseqe == iEnd )
    runseqe = iStart;
  else 
    runseqe++;
}
/*const unsigned char* const playerRunSeq[16] = {
  Left0,	Left1,	
  Right0,	Right1,
  Up0,		Up1,	
  Down0,	Down1,	
};*/
//** What the player can do**\\
int j;
void player_action(char oam_id)
{        
  pad = pad_poll(j);
  directions();//animation


  if (pad&PAD_UP)
  {

    actor_dy[0] = -2;actor_y[0] += actor_dy[0]; 
    rlud=0; 
    if(runseq == 0 || runseq > 1)
      runseq = 0;  


  }else if (pad&PAD_DOWN)
  {
    actor_dy[0]=2;actor_y[0] += actor_dy[0];
    rlud=2; 
    if(runseq < 2 || runseq > 3)
      runseq = 2; 

  }else if(pad&PAD_LEFT )
  {
    actor_dx[0] = -2; actor_x[0] += actor_dx[0];
    rlud = 4;
    if(runseq < 4 || runseq > 5)
      runseq = 4;   

  }else if (pad&PAD_RIGHT)
  {
    actor_dx[0] = 2;actor_x[0] += actor_dx[0];
    rlud=6;   
    if(runseq < 6 || runseq > 7)
      runseq = 6;    

  }else{ 
    rlud = 0;
    if(runseq == 0 || runseq > 1)
      runseq = 0; 
  }


  oam_id = oam_meta_spr(actor_x[0], actor_y[0],  oam_id, playerRunSeq[runseq]);

  if(iNPotion >=10) oam_id = oam_spr(230, 22, (iNPotion/10%10)+48, 4, oam_id);
  oam_id = oam_spr(238, 22, (iNPotion%10)+48, 4, oam_id);
  if(iPHealth >=10) oam_id = oam_spr(230, 14, (iPHealth/10%10)+48, 4, oam_id);
  oam_id = oam_spr(238, 14, (iPHealth%10)+48, 4, oam_id);


  ppu_wait_frame();
  if (oam_id!=0) oam_hide_rest(oam_id);

}//** What the player can do**\\

void enemy_action(char oam_id,int ienemyid,int iColor,)
{   
  collision (oam_id,actor_x[ienemyid]-16,actor_x[ienemyid]+16,actor_y[ienemyid]-16,actor_y[ienemyid]+16, 0,
             4,0,0,FALSE,0,0);
  directions2(2,2+1);

  actor_x[ienemyid] += actor_dx[ienemyid];//let the enemy move rl
  actor_y[ienemyid] += actor_dy[ienemyid];//let the enemy move ud
  if(iColor == 0)
    oam_id = oam_meta_spr(actor_x[ienemyid], actor_y[ienemyid], oam_id, playerRunSeqe1[runseqe]);
  else if(iColor == 1)
    oam_id = oam_meta_spr(actor_x[ienemyid], actor_y[ienemyid], oam_id, playerRunSeqe2[runseqe]);
  else if(iColor == 2) 
    oam_id = oam_meta_spr(actor_x[ienemyid], actor_y[ienemyid], oam_id, playerRunSeqe3[runseqe]);
  else if (oam_id!=0) oam_hide_rest(oam_id);

}
void SaveLastLocationBF()//save last location befor fight
{
  actorlastx = actor_x[0];
  actorlasty = actor_y[0];
}

void StartMenus(char oam_id)
{  
  int path;
  pad = pad_trigger(0);//get one input from player
  rlud = 0;		//type of fairy animation

  switch(iGameState)	//switch game state
  {
    case 0://update background and move to case: 3
      //to provent background from loading in a loop
      setupeverything();
      BackGround(Menu0_pal, Menu0_rle); iGameState = 3;
      break;
    case 1: //update background and move to case: 4
      //to provent background from loading in a loop
      BackGround(Menu1_pal, Menu1_rle); iGameState = 4;
      x = 69; y= 77;  //update x and y location
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
      directions(); //this is the animation of fairy
      //if(pad&PAD_UP && iGamePath != 0)//up input with path 
      //{y=80;iGamePath =0;}//the Tower path
      //else if(pad&PAD_DOWN && iGamePath != 1)
      //{y=120;iGamePath = 1;}//the Story path      
      if(pad&PAD_A)//send to character select since the path was chosen 
      {iGameState = 2;}
      //fairy's animation and location
      oam_id = oam_meta_spr(x, y,  oam_id, playerRunSeq[runseq]);
      path = iGamePath;
      ppu_wait_frame();
      break;
    case 5: 
      directions(); //this is the animation of fairy
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
        pal_all(PALETTE1);
        oam_id = oam_meta_spr(x, y,  oam_id, playerRunSeq[runseq]);
      }if(iGameType == 2){//color update 2 = pink
        pal_all(PALETTE2);
        oam_id = oam_meta_spr(x, y,  oam_id, playerRunSeq[runseq]);
      }if(iGameType == 3){//color update 1 = blue
        pal_all(PALETTE3);
        oam_id = oam_meta_spr(x, y,  oam_id, playerRunSeq[runseq]);
      }if(iGameType == 4){//color update 1 = white/blue
        pal_all(PALETTE4);
        oam_id = oam_meta_spr(x, y,  oam_id, playerRunSeq[runseq]);
      }if(pad&PAD_B)//iGameType is character's color
      {iGameState = 1;}

      if(pad&PAD_A && path == 1)
      {iGameState = 6;  MoveMap(iCurrentLevel,0);}//go to screen update 
      else if(pad&PAD_A && path == 0)
      {iGameState = 6; MoveMap(iCurrentLevel,0); }//go to screen update 
      ppu_wait_frame();
      break;
    case 6: //Tower path 

      player_action(oam_id); //set player

      MapTower(oam_id+32);  //type of map set  
      //points_count(oam_id+260);
      ppu_wait_frame();
      break;
    case 7: //Story path

      break;    
    case 8: //Story path
      fightaction(oam_id);
      break;
  }


}

void MoveMap(int iNextLevelMap,int iNextLevel)
{
  int iCount;//count down for resetting
  iNextLevelMap = iNextLevelMap*2;//this is for the seq 0-1, 2-3,...

  for(iCount = 0; iCount <9;iCount++)
    items[iCount]=0;//reset
  for(iCount = 0; iCount <5;iCount++)
    iOpen[iCount]=0;//reset
  SetPlayer(128, 189);//re-setplayer
  if(iNextLevel == 2)
    SetEnemy(1,100,50,1,0);
  iCurrentLevel = iNextLevel;//update level
  BackGround(WoldSeq[iNextLevelMap], WoldSeq[iNextLevelMap+1]);//change background
}
void MoveMapC(int iNextLevelMap,int iNextLevel, int ix, int iy)//move to new location with items
{
  iNextLevelMap = iNextLevelMap*2;//this is for the seq 0-1, 2-3,...
  if(iCurrentLevel == 2)
    ienemy[1]=0;
  SetEnemy(1,198,80,0,1);
  SetEnemy(2,198,80,0,1);
  SetEnemy(3,ix-16,iy,0,0);
  SetEnemy(4,198,76,0,0);
  SetEnemy(5,120,130,0,0);
  SetPlayer(ix, iy);//re-setplayer
  iCurrentLevel = iNextLevel;//update level
  BackGround(WoldSeq[iNextLevelMap], WoldSeq[iNextLevelMap+1]);//change background
}
void fightaction(char oam_id)
{
  int v;int xx; int yy;
  if(iELives <= 0)
  {
    ienemy[iEnemyid] = 1;
    actor_x[0] = actorlastx;
    actorlastx = 0;
    actor_y[0] = actorlasty;
    actorlasty = 0;    
    iELives = 20;
    setup_graphics();//set graphics
    if(iCurrentLevel < 3)
      BackGround(WoldSeq[iCurrentLevel], WoldSeq[iCurrentLevel-1]);//change background
    else{
      if(iCurrentLevel == 3)v = 2;
      if(iCurrentLevel == 4)v = 4;
      if(iCurrentLevel == 5)v = 6;
      if(iCurrentLevel == 6)v = 8;
      BackGround(WoldSeq[v], WoldSeq[v+1]);//change background
    }
    iGameState = 6;
  }  
  if(iPHealth <= 0)
  {     
    setup_graphics();//set graphics
    iCurrentLevel = 8;
    iGameState = 6;
  }
  if(pad&PAD_UP && iPlayerAction != 0)//move up
  {iPlayerAction = iPlayerAction-1;iPlayerChoice = iPlayerChoice-8;
  }if(pad&PAD_DOWN && iPlayerAction != 5)//move down
  {iPlayerAction = iPlayerAction+1;iPlayerChoice = iPlayerChoice+8;}

  if(pad&PAD_A&&iPlayerAction == 0&&iPMoves !=0)//heal 
  {if(iPHealth >=1){iPHealth = iPHealth+1;iNPotion--;}iPMoves--;}
  if(pad&PAD_A&&iPlayerAction == 1&&iPMoves !=0)//atack
  {if(iPlayerY == iRandy1 && iPlayerX == iRandx1)//if player in enemy atack
  {iPHealth = iPHealth-iEDamage;iELives = iELives- iPDamage;bEnemyAttack = FALSE;iPMoves = iPMoves+iPLevel;}
   if(iPlayerY == iRandy && iPlayerX == iRandx)//if player in enemy atack
   {iPHealth = iPHealth-iEDamage;iELives = iELives- iPDamage;bEnemyAttack = FALSE;iPMoves = iPMoves+iPLevel;}
   else{iELives = iELives- iPDamage;bEnemyAttack = FALSE;}
   iPMoves = iPMoves+iPLevel;
  }
  if(pad&PAD_A&&iPlayerAction == 2&&iPMoves !=0)//move down
  {if(iPlayerY != 4)
  {actor_y[0] = actor_y[0]+16;iPlayerY = iPlayerY+1;}iPMoves--;}
  if(pad&PAD_A&&iPlayerAction == 3&&iPMoves !=0)//move up
  {if(iPlayerY != 0)
  {actor_y[0] = actor_y[0]-16;iPlayerY = iPlayerY-1;}iPMoves--;}
  if(pad&PAD_A&&iPlayerAction == 4&&iPMoves !=0)//move left
  {if(iPlayerX != 0)
  {actor_x[0] = actor_x[0]-16;iPlayerX = iPlayerX-1;}iPMoves--;}
  if(pad&PAD_A&&iPlayerAction == 5&&iPMoves !=0)//move right
  {if(iPlayerX != 6)
  {actor_x[0] = actor_x[0]+16;iPlayerX = iPlayerX+1;}iPMoves--;}
  if(iPMoves == 0){
    if(iPlayerY == iRandy1 && iPlayerX == iRandx1)//if player in enemy atack
    {iPHealth = iPHealth - iEDamage;
     iELives = iELives - iPDamage;
     bEnemyAttack = FALSE;}
    if(iPlayerY == iRandy && iPlayerX == iRandx)//if player in enemy attack
    {iPHealth = iPHealth - iEDamage;
     iELives = iELives - iPDamage;
     bEnemyAttack = FALSE;
    }else{ iELives = iELives - iPDamage;
          bEnemyAttack = FALSE;
         }iPMoves = iPMoves + iPLevel;}

  //-----------------------------sprit----------------------
  if(iPMoves >=10) oam_id = oam_spr(224, 134, (iPMoves/10%10)+48, 4, oam_id);
  oam_id = oam_spr(230, 134, (iPMoves%10)+48, 4, oam_id);

  if(iPDamage >=10) oam_id = oam_spr(224, 110, (iPDamage/10%10)+48, 4, oam_id);
  oam_id = oam_spr(230, 110, (iPDamage%10)+48, 4, oam_id);

  if(iPHealth >=10) oam_id = oam_spr(224, 102, (iPHealth/10%10)+48, 4, oam_id);
  oam_id = oam_spr(230, 102, (iPHealth%10)+48, 4, oam_id);

  if(iPLevel >=10) oam_id = oam_spr(224, 94, (iPLevel/10%10)+48, 4, oam_id);
  oam_id = oam_spr(230, 94, (iPLevel%10)+48, 4, oam_id);

  if(iNPotion >=10) oam_id = oam_spr(224, 118, (iNPotion/10%10)+48, 4, oam_id);
  oam_id = oam_spr(230, 118, (iNPotion%10)+48, 4, oam_id);

  if(iELevel >=10)   oam_id = oam_spr(64, 32, (iELevel/10%10)+48, 4, oam_id);
  oam_id = oam_spr(72, 32, (iELevel%10)+48, 4, oam_id);

  if(iELives >=10)  oam_id = oam_spr(64, 40, (iELives/10%10)+48, 4, oam_id);
  oam_id = oam_spr(72, 40, (iELives%10)+48, 4, oam_id);

  if(iEDamage >=10)   oam_id = oam_spr(64, 48, (iEDamage/10%10)+48, 4, oam_id);
  oam_id = oam_spr(72, 48, (iEDamage%10)+48, 4, oam_id);
  //------------------------------------------------------------

  if(bEnemyAttack == FALSE){
    iRandx = (rand()%7);iRandy = (rand()%5);bEnemyAttack = TRUE;
    iRandx1 = iPlayerX; iRandy1 = iPlayerY; // random num y
    xx = actor_x[0], yy = actor_y[0];
  }else{
    oam_id = oam_meta_spr(xx, yy,  oam_id,Target);
    oam_id = oam_meta_spr(16+(iRandx*16), 127+(iRandy*16),  oam_id,Target);
  }
  oam_id = oam_spr(176, 150+iPlayerChoice, 62, 4, oam_id);
  //runseq = 0;   
  directions();//animation  
  oam_id = oam_meta_spr(actor_x[0], actor_y[0],  oam_id, playerRunSeq[runseq]);  
  if(iEColor == 0)
    oam_id = oam_meta_spr(actor_x[iEnemyid], actor_y[iEnemyid], oam_id, playerRunSeqe1[runseq]);
  else if(iEColor == 1)
    oam_id = oam_meta_spr(actor_x[iEnemyid], actor_y[iEnemyid], oam_id, playerRunSeqe2[runseq]);
  else if(iEColor == 2) 
    oam_id = oam_meta_spr(actor_x[iEnemyid], actor_y[iEnemyid], oam_id, playerRunSeqe3[runseq]);
  if (oam_id!=0) oam_hide_rest(oam_id);
}

void setupfight()
{
  iPlayerY = 2;
  iPlayerX = 3;
  actor_x[0] = 64;
  actor_y[0] = 158;
  actor_dx[0] = 0;
  actor_dy[0] = 0;
  actor_x[iEnemyid] = 64;
  actor_y[iEnemyid] = 94;
  actor_dx[iEnemyid] = 0;
  actor_dy[iEnemyid] = 0;
}
void movetofightscreen()
{
  setup_graphics();
  BackGround(fight1_pal, fight1_rle);//change background
  setupfight();
  iGameState = 8;
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
  }if(iObject == 4)//fight object
  {   
    runseq = 0;
    //go to fight sceen
    SaveLastLocationBF();
    movetofightscreen();

  }else if(MoveToLevel == TRUE)
  {
    if(iNextLevel < 3)
      MoveMap(iNextLevelMap,iNextLevel); 
    if(iCurrentLevel < 3 && iNextLevel == 3)
      MoveMapC(iNextLevelMap,iNextLevel,110,180);
    if(iCurrentLevel == 4 && iNextLevel == 3)
      MoveMapC(iNextLevelMap,iNextLevel,196,150);
    if(iCurrentLevel == 5 && iNextLevel == 3)
      MoveMapC(iNextLevelMap,iNextLevel,40,150);
    if(iCurrentLevel == 3 && iNextLevel == 4)
      MoveMapC(iNextLevelMap,iNextLevel,40,150);
    if(iCurrentLevel == 3 && iNextLevel == 5)
      MoveMapC(iNextLevelMap,iNextLevel,196,150);
    if(iCurrentLevel == 6 && iNextLevel == 5)
      MoveMapC(iNextLevelMap,iNextLevel,28,180);
    if(iNextLevel == 6)
      MoveMapC(iNextLevelMap,iNextLevel,28,60);
    if(iNextLevel == 7)
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
  if((actor_y[iActor] >= y1-1 &&  actor_y[iActor] <= y1 )&& 
     (actor_x[iActor] >= x1 && actor_x[iActor] <=x2))
  { 
    ObjectAffect(iObject,ikeys,iDoor,MoveToLevel,iNextLevelMap, iNextLevel);
    if(iActor !=0)
    {
      actor_dy[iActor] = - actor_dy[iActor];
    }
    if(iObject == 0)
      actor_y[iActor] = actor_y[iActor]-2;
  }
  //bottom of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y2-1 && actor_y[iActor] <= y2) &&
     (actor_x[iActor] >= x1 && actor_x[iActor] <=x2))
  {
    ObjectAffect(iObject,ikeys,iDoor,MoveToLevel,iNextLevelMap, iNextLevel);
    if(iActor !=0)
    {
      actor_dy[iActor] = - actor_dy[iActor];
    }
    if(iObject == 0)
      actor_y[iActor] = actor_y[iActor]+2;
  }   
  //right side of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y1 && actor_y[iActor] <= y2) &&
     (actor_x[iActor] >= x1+1 && actor_x[iActor] <=x2))
  {
    ObjectAffect(iObject,ikeys,iDoor,MoveToLevel,iNextLevelMap, iNextLevel);
    if(iActor !=0)
    {
      actor_dx[iActor] = - actor_dx[iActor];
    }
    if(iObject == 0)
      actor_x[iActor] = actor_x[iActor]+2;
  }
  //left side of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y1 && actor_y[iActor] <= y2) && 
     (actor_x[iActor] >= x1 && actor_x[iActor] <=x2-1))
  {
    ObjectAffect(iObject,ikeys,iDoor,MoveToLevel,iNextLevelMap, iNextLevel);
    if(iActor !=0)
    {
      actor_dx[iActor] = - actor_dx[iActor];

    }
    if(iObject == 0)
      actor_x[iActor] = actor_x[iActor]-2;
  } 
  if(test==1 ){
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
    collision (oam_id,object[id].x-25,object[id].x+15,object[id].y-25,object[id].y+16, 0,3,0,ikeys,FALSE,0,0);
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
void passkey(char oam_id,int iColor,int id)
{
  oam_id = oam_spr(object[id].x, object[id].y, object[id].state, iColor, oam_id); 

}
