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
  0x0B,0x3B,0x1B,0x00,	// sprite palette 1

  0x00,0x00,0x00,0x00,	// sprite palette 2
  0x00,0x00,0x00,	// sprite palette 3
};const char PALETTE1[32] = { 
  0x0F,			// screen color
  0x31,0x30,0x1C,0x00,	// background palette 0
  0x0C,0x1C,0x1C,0x00,	// background palette 1
  0x06,0x16,0x16,0x00,	// background palette 2
  0x0B,0x00,0x1A,0x00,   // background palette 3

  0x36,0x16,0x06,0x00,	// sprite palette 0
  0x06,0x36,0x16,	// sprite palette 1

  0x00,0x00,0x00,0x00,	// sprite palette 2
  0x00,0x00,0x00,	// sprite palette 3

};const char PALETTE2[32] = { 
  0x0F,			// screen color
  0x31,0x30,0x1C,0x00,	// background palette 0
  0x0C,0x1C,0x1C,0x00,	// background palette 1
  0x06,0x16,0x16,0x00,	// background palette 2
  0x0B,0x00,0x1A,0x00,   // background palette 3

  0x31,0x32,0x33,0x00,	// sprite palette 0
  0x33,0x31,0x32,0x00,	// sprite palette 1

  0x00,0x00,0x00,0x00,	// sprite palette 2
  0x00,0x00,0x00,	// sprite palette 3

};const char PALETTE3[32] = { 
  0x0F,			// screen color
  0x31,0x30,0x1C,0x00,	// background palette 0
  0x0C,0x1C,0x1C,0x00,	// background palette 1
  0x06,0x16,0x16,0x00,	// background palette 2
  0x0B,0x00,0x1A,0x00,   // background palette 3

  0x2C,0x0C,0x3C,0x00,	// sprite palette 0
  0x21,0x0C,0x3A,0x00,	// sprite palette 1  

  0x00,0x00,0x00,0x00,	// sprite palette 2
  0x00,0x00,0x00,	// sprite palette 3

};const char PALETTE4[32] = { 
  0x0F,			// screen color
  0x31,0x30,0x1C,0x00,	// background palette 0
  0x0C,0x1C,0x1C,0x00,	// background palette 1
  0x06,0x16,0x16,0x00,	// background palette 2
  0x0B,0x00,0x1A,0x00,   // background palette 3

  0x31,0x30,0x3C,0x00,	// sprite palette 0
  0x3c,0x31,0x30,0x00,	// sprite palette 1  

  0x00,0x00,0x00,0x00,	// sprite palette 2
  0x00,0x00,0x00,	// sprite palette 3

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
#define DEF_METASPRITE_2x2Object(name,code,pal)\
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

DEF_METASPRITE_2x2Object(Platform0, 0x80, 0);
DEF_METASPRITE_2x2Object(Platform1, 0xc4, 1);
DEF_METASPRITE_2x2Object(Door0, 0xc4, 0);
DEF_METASPRITE_2x2Object(Door1, 0xc4, 1);
DEF_METASPRITE_2x2Object(Door2, 0xc4, 2);
DEF_METASPRITE_2x2Object(Door3, 0xc4, 3);
DEF_METASPRITE_2x2Object(Target, 0x84, 3);

const unsigned char* const playerRunSeq[10] = {
  Left0,	Left1,	
  Right0,	Right1,	
  Up0,		Up1,	
  Down0,	Down1,
  Down0,	Down1
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
Objects object[20];//0-3 keys  4-8 locks  9-10 potion 11-20 health

void collision(char oam_id,int x1,int x2,int y1,int y2, int iActor,
               int iObject,int ikeys,int iDoor,bool MoveToLevel,int iNextLevelMap,int iNextLevel);
void ObjectKey(char oam_id,int x,int y,int inum,int icolor);
void PickUpObject(char oam_id,int x1,int x2,int y1,int y2,int icolor);
void pickupkey(int ikeys);
void lock(char oam_id,int x, int y, int ikeys,int icolor);
void MoveMap(int iMap2,int iNextLevel);

void fightaction(char oam_id);
void fighting(char oam_id);

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
  actor_x[1] = 100;
  actor_y[1] = 59;
  actor_dx[1] = 1;
  actor_dy[1] = 0;

  actor_x[3] = 150;
  actor_y[3] = 100;
  actor_dx[3] = 0;
  actor_dy[3] = 1;

  actor_x[2] = 100;
  actor_y[2] = 150;
  actor_dx[2] = 1;
  actor_dy[2] = 1;
}

void SetPlayer()
{
  actor_x[0] = 128;
  actor_y[0] = 189;
  actor_dx[0] = 0;
  actor_dy[0] = 0;
}

int iGameState = 0;	//what state were going to be
int iGamePath = 0;	//what path the player picks
int iGameType = 0;	//what Type the player picks
int iNPotion = 12;	//N number of potions
int iPHealth = 26;	//player's Health
int iPLevel = 20;	//player's level
int iPDamage = 12;	//player's damage
int iPMoves = 13;	//player's 
int iELives = 20;	//N number of lives
int iELevel = 10;	//N numver of levels
int iEDamage = 12;	//N numver of damage
int x = 80;	//x of player path
int y = 80;     // y of player path 
int iRandx;	// random num x
int iRandy;    	// random num y
char pad;	//input pad
byte runseq;	//actor next Seq
byte runseqe;	//actor next Seq enemy
int items[9] = {0,0,0,0,0,0,0,0,0};	//keys/potions
int iOpen[5] = {0,0,0,0,0}; 		//door on/off
int rlud; 	// right,lest,up, and Down save last key input
int test = 0; 	//display collision 
int iCurrentLevel = 0; 	//the level the player will be
int keyplaced = 0; 	//setup key
int actorlastx[4]={0,0,0,0};	//
int actorlasty[4]={0,0,0,0};	//

bool bEnemyAttack;	//set up next enemy attack
int iPlayerAction;	//get player's next action
int iPlayerChoice;      //set player's action
int iPlayerY = 2;	//set players y location in fight
int iPlayerX = 3;	//set players x location in fight
int iMapPAL;		//save map
int iMapRLE;		//save map

int iMaxSeq = 10;
int iStartSeq = 0;    //actor Seq
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
void MapTower (char oam_id)
{ 
  if(iCurrentLevel == 0)
  {
    levelcopie(oam_id,0);
    oam_id = oam_meta_spr(32, 95,  oam_id,Platform0);
    collision (oam_id,32-8,32+8,95-8,95+12, 0,1,0,0,FALSE,0,0);

    Door(oam_id,112,47,0,0); 
    collision (oam_id,80,158,10,12, 0,0,0,0,TRUE,0,1);

  } 
  if(iCurrentLevel == 1)
  {      
    levelcopie(oam_id,0); 

    Door(oam_id,112,47,0,1); 
    ObjectKey(oam_id+16,70,180,0,4);//oam_id,x1,x2,y1,y2,key id,key color    
    lock(oam_id+68,140,52,0,4);//oam_id,x,y,key id,lock color    
    collision (oam_id,80,158,10,12, 0,0,0,0,TRUE,0,2);
  }
  if(iCurrentLevel == 2)
  {      
    levelcopie(oam_id,0); 
    levelcopie(oam_id,1); 
    levelcopie(oam_id,2); 

    //ObjectKey(oam_id+16,79,180,0,4);//oam_id,x1,x2,y1,y2,key id,key color
    //lock(oam_id+24,180,52,0,4);//oam_id,x,y,key id,lock color
    //Door(oam_id,112,47,0,0); //oam_id,x,y,key id,door color[0-3]
    collision (oam_id,80,158,10,12, 0,0,0,0,TRUE,0,3);
  }
  if(iCurrentLevel == 3)
  {      
    levelcopie(oam_id,0); 

    //ObjectKey(oam_id+16,79,180,0,4);//oam_id,x1,x2,y1,y2,key id,key color
    //lock(oam_id+24,180,52,0,4);//oam_id,x,y,key id,lock color
    Door(oam_id,112,47,0,0); //oam_id,x,y,key id,door color[0-3]
    collision (oam_id,80,158,10,12, 0,0,0,0,TRUE,0,3);
  }
}

//** actors animation and directions facing**\\
void directions(int iStart,int iEnd)
{  
  if(runseq == iEnd )
    runseq = iStart;
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
  Left0,	Left1,	Left0,	Left1,
  Right0,	Right1,	Right0,	Right1,
  Up0,		Up1,	Up0,	Up1,
  Down0,	Down1,	Down0,	Down1,
};*/
//** What the player can do**\\

void player_action(char oam_id)
{        
  pad = pad_poll(0);
  directions(rlud,rlud+1);//animation


  if(pad&PAD_LEFT )
  {
    actor_dx[0] = -1; actor_x[0] += actor_dx[0];
    rlud = 0;
    if(runseq == 0 || runseq > 1)
      runseq = 0;   

  }if (pad&PAD_RIGHT)
  {
    actor_dx[0] = 1;actor_x[0] += actor_dx[0];
    rlud=2;   
    /*if(runseq <4 || runseq >7)
    runseq = 4;   */

  }if (pad&PAD_UP)
  {
    actor_dy[0] = -1;actor_y[0] += actor_dy[0]; 
    rlud=4; 
    /*if(runseq <8 || runseq >11)
    runseq = 8; */

  }if (pad&PAD_DOWN)
  {
    actor_dy[0]=1;actor_y[0] += actor_dy[0];
    /*if(runseq != 6 || runseq >7)
      runseq = 6;*/
    rlud=6; 

  }


  
  
  oam_id = oam_meta_spr(actor_x[0], actor_y[0],  oam_id, playerRunSeq[runseq]);
  
  if(iNPotion >=10) oam_id = oam_spr(230, 22, (iNPotion/10%10)+48, 4, oam_id);
  oam_id = oam_spr(238, 22, (iNPotion%10)+48, 4, oam_id);
  if(iPHealth >=10) oam_id = oam_spr(230, 14, (iPHealth/10%10)+48, 4, oam_id);
  oam_id = oam_spr(238, 14, (iPHealth%10)+48, 4, oam_id);


  ppu_wait_frame();
  if (oam_id!=0) oam_hide_rest(oam_id);

}//** What the player can do**\\

void enemy_action(char oam_id,int ienemyid)
{   
  collision (oam_id,actor_x[ienemyid]-8,actor_x[ienemyid]+8,actor_y[ienemyid]-8,actor_y[ienemyid]+8, 0,
             4,0,0,FALSE,0,0);
	directions2(4,4+1);
  
  actor_x[ienemyid] += actor_dx[ienemyid];//let the enemy move rl
  actor_y[ienemyid] += actor_dy[ienemyid];//let the enemy move ud
  oam_id = oam_meta_spr(actor_x[ienemyid], actor_y[ienemyid], oam_id, playerRunSeq[runseqe]);
  if (oam_id!=0) oam_hide_rest(oam_id);

}
void SaveLastLocationBF()//save last location befor fight
{

  actorlastx[0] = actor_x[0];
  actorlasty[0] = actor_y[0];
  actorlastx[1] = actor_x[1];
  actorlasty[1] = actor_y[1];  
  actorlastx[2] = actor_x[2];
  actorlasty[2] = actor_y[2];  
  actorlastx[3] = actor_x[3];
  actorlasty[3] = actor_y[3];
}

void StartMenus(char oam_id)
{  
  pad = pad_trigger(0);//get one input from player
  rlud = 4;		//type of fairy animation
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
      directions(rlud,rlud+1); //this is the animation of fairy
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
      directions(rlud,rlud+1); //this is the animation of fairy
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
      if(pad&PAD_A && iGamePath == 0)
      {iGameState = 6; MoveMap(iCurrentLevel,0);
       SetEnemy();}//go to screen update 
      if(pad&PAD_A && iGamePath == 1)
      {iGameState = 6;  MoveMap(iCurrentLevel,0);}//go to screen update 
      ppu_wait_frame();
      break;
    case 6: //Tower path 

      player_action(oam_id); //set player
      if(iCurrentLevel == 2)
      {
        enemy_action(oam_id+32,1); //set player
      }
      MapTower(oam_id+112);  //type of map set  
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
  SetPlayer();//re-setplayer
  iCurrentLevel = iNextLevel;//update level
  BackGround(WoldSeq[iNextLevelMap], WoldSeq[iNextLevelMap+1]);//change background
}

void fightaction(char oam_id)
{
  if(iELives <= 0)
  {
    actor_x[0] = actorlastx[0];
    actorlastx[0] = 0;
    actor_y[0] = actorlasty[0];
    actorlasty[0] = 0;
    actor_dx[0] = 0;
    actor_dy[0] = 0;
    setup_graphics();//set graphics
    BackGround(WoldSeq[iCurrentLevel], WoldSeq[iCurrentLevel-1]);//change background
    iGameState = 6;
  }
  if(pad&PAD_UP && iPlayerAction != 0)
  {iPlayerAction = iPlayerAction-1;iPlayerChoice = iPlayerChoice-8;
  }if(pad&PAD_DOWN && iPlayerAction != 5)
  {iPlayerAction = iPlayerAction+1;iPlayerChoice = iPlayerChoice+8;
  }
  if(pad&PAD_A&&iPlayerAction == 0&&iPMoves !=0){if(iPHealth >=1){iPHealth = iPHealth+1;iNPotion--;}iPMoves--;}
  if(pad&PAD_A&&iPlayerAction == 1&&iPMoves !=0){if(iPlayerY == iRandy && iPlayerX == iRandx)
  {iPHealth = iPHealth-iEDamage;iELives = iELives- iPDamage;bEnemyAttack = FALSE;}else{iELives = iELives- iPDamage;bEnemyAttack = FALSE;}
                                                 iPMoves = iPMoves+iPLevel;}
  if(pad&PAD_A&&iPlayerAction == 2&&iPMoves !=0){if(iPlayerY != 4){actor_y[0] = actor_y[0]+16;iPlayerY = iPlayerY+1;}iPMoves--;}
  if(pad&PAD_A&&iPlayerAction == 3&&iPMoves !=0){if(iPlayerY != 0){actor_y[0] = actor_y[0]-16;iPlayerY = iPlayerY-1;}iPMoves--;}
  if(pad&PAD_A&&iPlayerAction == 4&&iPMoves !=0){if(iPlayerX != 0){actor_x[0] = actor_x[0]-16;iPlayerX = iPlayerX-1;}iPMoves--;}
  if(pad&PAD_A&&iPlayerAction == 5&&iPMoves !=0){if(iPlayerX != 6){actor_x[0] = actor_x[0]+16;iPlayerX = iPlayerX+1;}iPMoves--;}


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

  if(iELives >=10)   oam_id = oam_spr(64, 32, (iELives/10%10)+48, 4, oam_id);
  oam_id = oam_spr(72, 32, (iELives%10)+48, 4, oam_id);

  if(iELevel >=10)  oam_id = oam_spr(64, 40, (iELevel/10%10)+48, 4, oam_id);
  oam_id = oam_spr(72, 40, (iELevel%10)+48, 4, oam_id);

  if(iEDamage >=10)   oam_id = oam_spr(64, 48, (iEDamage/10%10)+48, 4, oam_id);
  oam_id = oam_spr(72, 48, (iEDamage%10)+48, 4, oam_id);

  if(bEnemyAttack == FALSE){
    iRandx = (rand()%7);
    iRandy = (rand()%5);
    bEnemyAttack = TRUE;
  }else
    oam_id = oam_meta_spr(16+(iRandx*16), 127+(iRandy*16),  oam_id,Target);

  oam_id = oam_spr(176, 150+iPlayerChoice, 62, 4, oam_id);
  runseq = 4;  
  directions(rlud,rlud+1);//animation
  oam_id = oam_meta_spr(actor_x[0], actor_y[0],  oam_id, playerRunSeq[runseq]);  
  oam_id = oam_meta_spr(actor_x[1], actor_y[1],  oam_id, playerRunSeq[runseq]);

  if (oam_id!=0) oam_hide_rest(oam_id);
}

void setupfight()
{
  actor_x[0] = 64;
  actor_y[0] = 158;
  actor_dx[0] = 0;
  actor_dy[0] = 0;
  actor_x[1] = 64;
  actor_y[1] = 94;
  actor_dx[1] = 0;
  actor_dy[1] = 0;
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
    //go to fight sceen
    SaveLastLocationBF();
    movetofightscreen();

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
    if(iActor !=0)
    {
      actor_dy[iActor] = - actor_dy[iActor];
    }
    if(iObject == 0)
      actor_y[iActor] = actor_y[iActor]-1;


  }
  //bottom of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y2-1 && actor_y[iActor] <= y2) &&  (actor_x[iActor] >= x1 && actor_x[iActor] <=x2))
  {
    ObjectAffect(iObject,ikeys,iDoor,MoveToLevel,iNextLevelMap, iNextLevel);
    if(iActor !=0)
    {
      actor_dy[iActor] = - actor_dy[iActor];
    }
    if(iObject == 0)
      actor_y[iActor] = actor_y[iActor]+1;
  }   
  //right side of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y1 && actor_y[iActor] <= y2) &&  (actor_x[iActor] >= x1-1 && actor_x[iActor] <=x2))
  {
    ObjectAffect(iObject,ikeys,iDoor,MoveToLevel,iNextLevelMap, iNextLevel);
    if(iActor !=0)
    {
      actor_dx[iActor] = - actor_dx[iActor];
    }
    if(iObject == 0)
      actor_x[iActor] = actor_x[iActor]-1;
  }
  //left side of the collision y1 -> y2 x1 -> x2
  if((actor_y[iActor] >= y1 && actor_y[iActor] <= y2) &&  (actor_x[iActor] >= x2 && actor_x[iActor] <=x2+1))
  {
    ObjectAffect(iObject,ikeys,iDoor,MoveToLevel,iNextLevelMap, iNextLevel);
    if(iActor !=0)
    {
      actor_dx[iActor] = - actor_dx[iActor];

    }
    if(iObject == 0)
      actor_x[iActor] = actor_x[iActor]+1;
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
