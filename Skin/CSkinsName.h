#pragma once
#include <vector>

#define SPECIAL_MODEL_MAX 51
#define PED_MODEL_MAX 251

struct SPedModel
{
	char szPedName [ 8 ];
	unsigned int uiPedID;
};

struct SSpecialModel
{
	char szModel [ 8 ];
	char szName [ 30 ];
};

extern SSpecialModel sSpecialModel [ SPECIAL_MODEL_MAX ];
extern SPedModel sPedModel [ PED_MODEL_MAX ];
