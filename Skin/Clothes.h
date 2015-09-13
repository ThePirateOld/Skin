#pragma once

#include <string>
#define PLAYER_CLOTHING_SLOTS 18

#define TORSO_CLOTHING_MAX 68
#define HAIR_CLOTHING_MAX 33
#define LEGS_CLOTHING_MAX 45
#define SHOES_CLOTHING_MAX 38
#define LEFT_LOWER_ARM_CLOTHING_MAX 3
#define LEFT_UPPER_ARM_CLOTHING_MAX 4
#define RIGHT_UPPER_ARM_CLOTHING_MAX 4
#define RIGHT_LOWER_ARM_CLOTHING_MAX 4
#define BACK_TOP_CLOTHING_MAX 7
#define LEFT_CHEST_CLOTHING_MAX 6
#define RIGHT_CHEST_CLOTHING_MAX 7
#define STOMACH_CLOTHING_MAX 7
#define LOWER_BACK_CLOTHING_MAX 6
#define CHAINS_CLOTHING_MAX 12
#define WATCHES_CLOTHING_MAX 12
#define GLASSES_CLOTHING_MAX 17
#define HATS_CLOTHING_MAX 57
#define SPECIAL_CLOTHING_MAX 9

struct SPlayerClothingType
{
	char szName [ 32 ];
};

struct SPlayerClothing
{
	char szTexture [ 32 ];
	char szModel [ 24 ];
	unsigned __int8 uiBodyPart;
	char szName [ 64 ];
};

class CClothes
{
public:
	static const int GetBodypartIdByName ( const char *szBodypartName );
	static const  SPlayerClothing  *GetClothingGroup ( unsigned char ucType );
	static const  SPlayerClothing  *GetClothingGroupByName ( const char *szBodypartName );
	static const int GetClothingGroupMax ( unsigned char ucType );
};

extern SPlayerClothingType sClothingType [ PLAYER_CLOTHING_SLOTS ];
