#pragma once
#include <vector>

#define SPECIAL_ACTOR_MAX 51
#define SKIN_NORMAL_MAX 251

struct PED_MODEL
{
	char szPedName[20];
	unsigned int uiPedID;
};

struct SPECIAL_ACTOR
{
	char szModel[24];
	char szName[64];
};

extern SPECIAL_ACTOR SpecialActor[SPECIAL_ACTOR_MAX];
extern PED_MODEL SkinNormal[SKIN_NORMAL_MAX];
