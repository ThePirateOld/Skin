#include "Clothes.h"

SPlayerClothingType sClothingType [ PLAYER_CLOTHING_SLOTS ] =
{
	{ "Torso" }, { "haircuts" }, { "Legs " }, { "Shoes" }, { "Lower left arm" },
	{ "Upper left arm" }, { "Upper right arm" },{ "Lower right arm" }, { "Back top" },
	{ "Left chest" }, { "Right chest" }, { "Stomach" }, { "Lower back" },
	{ "Necklace" }, { "Watch" }, { "Glasses" }, { "Hats" }, { "Special clothes" }
};

static SPlayerClothing sTorsoClothing [ TORSO_CLOTHING_MAX ] =
{
	{ "player_torso", "torso", 0, "Torso" }, { "vestblack", "vest", 0, "Black Tank" },
	{ "vest", "vest", 0, "White Tank" }, { "tshirt2horiz", "tshirt2", 0, "Striped T - Shirt" },
	{ "tshirtwhite", "tshirt", 0, "White T - Shirt" }, { "tshirtilovels", "tshirt", 0, "L.S.T - Shirt" },
	{ "tshirtblunts", "tshirt", 0, "Sharps T - Shirt" }, { "shirtbplaid", "shirtb", 0, "Plaid Shirt" },
	{ "shirtbcheck", "shirtb", 0, "Checkered Shirt" }, { "field", "field", 0, "Combat Jacket" },
	{ "tshirterisyell", "tshirt", 0, "Eris T - Shirt" }, { "tshirterisorn", "tshirt", 0, "Eris T - Shirt2" },
	{ "trackytop2eris", "trackytop1", 0, "Track Top" }, { "bbjackrim", "bbjack", 0, "Rimmers Jacket" },
	{ "bballjackrstar", "bbjack", 0, "R - Star Jacket" }, { "baskballdrib", "baskball", 0, "Dribblers Vest" },
	{ "baskballrim", "baskball", 0, "Saint's Shirt" }, { "sixtyniners", "tshirt", 0, "69ers T - Shirt" },
	{ "bandits", "baseball", 0, "Bandits Top" }, { "tshirtprored", "tshirt", 0, "ProLaps T - Shirt" },
	{ "tshirtproblk", "tshirt", 0, "ProLaps Black T" }, { "trackytop1pro", "trackytop1", 0, "Track Top" },
	{ "hockeytop", "sweat", 0, "Slappers Top" }, { "bbjersey", "sleevt", 0, "Baseball T" },
	{ "shellsuit", "trackytop1", 0, "Leisure Top" }, { "tshirtheatwht", "tshirt", 0, "White Heat T" },
	{ "tshirtbobomonk", "tshirt", 0, "Bobo Ape T" }, { "tshirtbobored", "tshirt", 0, "Red Bobo T" },
	{ "tshirtbase5", "tshirt", 0, "Base 5 T" }, { "tshirtsuburb", "tshirt", 0, "Suburban T" },
	{ "hoodyamerc", "hoodya", 0, "Mercury Hood" }, { "hoodyabase5", "hoodya", 0, "Base 5 Hood" },
	{ "hoodyarockstar", "hoodya", 0, "Rockstar Hood" }, { "wcoatblue", "wcoat", 0, "Vest & T - Shirt" },
	{ "coach", "coach", 0, "Grn Windbreaker" }, { "coachsemi", "coach", 0, "Blk Windbreaker" },
	{ "sweatrstar", "sweat", 0, "Rockstar Sweat" }, { "hoodyAblue", "hoodyA", 0, "Blue Hoody" },
	{ "hoodyAblack", "hoodyA", 0, "Black Hoody" }, { "hoodyAgreen", "hoodyA", 0, "Green Hoody" },
	{ "sleevtbrown", "sleevt", 0, "Brown Shirt" }, { "shirtablue", "shirta", 0, "Sky Blue Shirt" },
	{ "shirtayellow", "shirta", 0, "Yellow Shirt" }, { "shirtagrey", "shirta", 0, "Gray Shirt" },
	{ "shirtbgang", "shirtb", 0, "Green Shirt" }, { "tshirtzipcrm", "tshirt", 0, "Cream Logo T" },
	{ "tshirtzipgry", "tshirt", 0, "Gray Logo T" }, { "denimfade", "denim", 0, "Jean Jacket" },
	{ "bowling", "hawaii", 0, "Bowling shirt" }, { "hoodjackbeige", "hoodjack", 0, "Hooded Jacket" },
	{ "baskballloc", "baskball", 0, "Loc - Down Vest" }, { "tshirtlocgrey", "tshirt", 0, "Loc - Down T" },
	{ "tshirtmaddgrey", "tshirt", 0, "Madd Tagg T" }, { "tshirtmaddgrn", "tshirt", 0, "Green Tagg T" },
	{ "suit1grey", "suit1", 0, "Gray Jacket" }, { "suit1blk", "suit1", 0, "Black Jacket" },
	{ "leather", "leather", 0, "Biker Jacket" }, { "painter", "painter", 0, "Chore Coat" },
	{ "hawaiiwht", "hawaii", 0, "Hawaiian Shirt" }, { "hawaiired", "hawaii", 0, "Blue Hawaiian" },
	{ "sportjack", "trackytop1", 0, "Sports Jacket" }, { "suit1red", "suit1", 0, "Red Jacket" },
	{ "suit1blue", "suit1", 0, "Blue Jacket" }, { "suit1yellow", "suit1", 0, "Yellow Jacket" },
	{ "suit2grn", "suit2", 0, "Tweed Jacket" }, { "tuxedo", "suit2", 0, "Tuxedo Pants" },
	{ "suit1gang", "suit1", 0, "Green Jacket" }, { "letter", "sleevt", 0, "Letterman Top" }
};

static SPlayerClothing sHairClothing [ HAIR_CLOTHING_MAX ] =
{
	{ "afro", "afro", 1, "Afro" }, { "afrobeard", "afro", 1, "Afro & Beard" },
	{ "afroblond", "afro", 1, "Blonde Afro" }, { "afrogoatee", "afro", 1, "Afro & Goatee" },
	{ "afrotash", "afro", 1, "Afro & Stash" }, { "bald", "head", 1, "Bald Head" },
	{ "baldbeard", "head", 1, "Bald & Beard" }, { "baldgoatee", "head", 1, "Bald & Goatee" },
	{ "baldtash", "head", 1, "Bald & Stash" }, { "beard", "head", 1, "Cesar & Beard" },
	{ "cornrows", "cornrows", 1, "Cornrow" }, { "cornrowsb", "cornrows", 1, "Blonde Cornrow" },
	{ "elvishair", "elvishair", 1, "Elvis Hair" }, { "flattop", "flattop", 1, "FlatTop" },
	{ "goatee", "head", 1, "Cesar & Goatee" }, { "groovecut", "groovecut", 1, "Groove Cut" },
	{ "hairblond", "head", 1, "Blonde Hair" }, { "hairblue", "head", 1, "Blue Hair" },
	{ "hairgreen", "head", 1, "Green Hair" }, { "hairpink", "head", 1, "Pink Hair" },
	{ "hairred", "Head", 1, "Red Hair" }, { "highafro", "highafro", 1, "High Afro" },
	{ "highfade", "head", 1, "High Fade" }, { "jhericurl", "jheri", 1, "Jheri Curl" },
	{ "mohawk", "mohawk", 1, "Mowhawk" }, { "mohawkbeard", "mohawk", 1, "Mohawk & Beard" },
	{ "mohawkblond", "mohawk", 1, "Blonde Mowhawk" }, { "mohawkpink", "mohawk", 1, "Pink Mowhawk" },
	{ "player_face", "head", 1, "Cesar" }, { "slope", "slope", 1, "Slope" },
	{ "tash", "head", 1, "Cesar & Stash" }, { "tramline", "tramline", 1, "Detail Cut" },
	{ "wedge", "wedge", 1, "Wedge" }
};

static SPlayerClothing sLegsClothing [ LEGS_CLOTHING_MAX ] =
{
	{ "player_legs", "legs", 2, "Legs" }, { "worktrcamogrn", "worktr", 2, "Woodland Camo" },
	{ "worktrcamogry", "worktr", 2, "Urban Camo" }, { "worktrgrey", "worktr", 2, "Gray Pants" },
	{ "worktrkhaki", "worktr", 2, "Olive Pants" }, { "tracktr", "tracktr", 2, "Sweat Pants" },
	{ "tracktreris", "tracktr", 2, "Track Pants" }, { "jeansdenim", "jeans", 2, "Blue Jeans" },
	{ "legsblack", "legs", 2, "Black Boxers" }, { "legsheart", "legs", 2, "Heart Boxers" },
	{ "biegetr", "chinosb", 2, "Beige Pants" }, { "tracktrpro", "tracktr", 2, "Track Pants" },
	{ "tracktrwhstr", "tracktr", 2, "Black Track Pants" }, { "tracktrblue", "tracktr", 2, "Blue Track Pants" },
	{ "tracktrgang", "tracktr", 2, "Green Track Pants" }, { "bbshortwht", "boxingshort", 2, "Ball Shorts" },
	{ "boxshort", "boxingshort", 2, "Boxing Shorts" }, { "bbshortred", "boxingshort", 2, "Dribbler Shorts" },
	{ "shellsuittr", "tracktr", 2, "Leisure Pants" }, { "shortsgrey", "shorts", 2, "Gray Shorts" },
	{ "shortskhaki", "shorts", 2, "Olive Shorts" }, { "chongergrey", "chonger", 2, "Gray Chonglers" },
	{ "chongergang", "chonger", 2, "Green Chonglers" }, { "chongerred", "chonger", 2, "Red Chonglers" },
	{ "chongerblue", "chonger", 2, "Blue Chonglers" }, { "shortsgang", "shorts", 2, "Green Shorts" },
	{ "denimsgang", "jeans", 2, "Green Jeans" }, { "denimsred", "jeans", 2, "Red Jeans" },
	{ "chinosbiege", "chinosb", 2, "Beige Khakis" }, { "chinoskhaki", "chinosb", 2, "Olive Khakis" },
	{ "cutoffchinos", "shorts", 2, "Beige Shorts" }, { "cutoffchinosblue", "shorts", 2, "Blue Shorts" },
	{ "chinosblack", "chinosb", 2, "Black Khakis" }, { "chinosblue", "chinosb", 2, "Blue Khakis" },
	{ "leathertr", "leathertr", 2, "Leather Pants" }, { "leathertrchaps", "leathertr", 2, "Leather Chaps" },
	{ "suit1trgrey", "suit1tr", 2, "Gray Pants" }, { "suit1trblk", "suit1tr", 2, "Black Pants" },
	{ "cutoffdenims", "shorts", 2, "Jean Shorts" }, { "suit1trred", "suit1tr", 2, "Red Pants" },
	{ "suit1trblue", "suit1tr", 2, "Blue Pants" }, { "suit1tryellow", "suit1tr", 2, "Yellow Pants" },
	{ "suit1trgreen", "suit1tr", 2, "Tweed Pants" }, { "suit1trblk2", "suit1tr", 2, "Tuxedo Pants" },
	{ "suit1trgang", "suit1tr", 2, "Green Pants" }
};

static SPlayerClothing sShoesClothing [ SHOES_CLOTHING_MAX ] =
{
	{ "foot", "feet", 3, "Shoes" }, { "cowboyboot2", "biker", 3, "Cowboy Boots" },
	{ "bask2semi", "bask1", 3, "Hi - Top Kicks" }, { "bask1eris", "bask1", 3, "Hi - Top Sneaks" },
	{ "sneakerbincgang", "sneaker", 3, "Green Low - Tops" }, { "sneakerbincblu", "sneaker", 3, "Blue Low - Tops" },
	{ "sneakerbincblk", "sneaker", 3, "Black Low - Tops" }, { "sandal", "flipflop", 3, "Sandals" },
	{ "sandalsock", "flipflop", 3, "Sandal & Socks" }, { "flipflop", "flipflop", 3, "Flip - Flops" },
	{ "hitop", "bask1", 3, "Mid - Top Sneaker" }, { "convproblk", "conv", 3, "Black Hi - Tops" },
	{ "convproblu", "conv", 3, "Blue Hi - Tops" }, { "convprogrn", "conv", 3, "Green Hi - Tops" },
	{ "sneakerprored", "sneaker", 3, "Red Sneakers" }, { "sneakerproblu", "sneaker", 3, "Blue Sneakers" },
	{ "sneakerprowht", "sneaker", 3, "White Sneakers" }, { "bask1prowht", "bask1", 3, "White Mid - Tops" },
	{ "bask1problk", "bask1", 3, "Black Mid - Tops" }, { "boxingshoe", "biker", 3, "Boxing Shoes" },
	{ "convheatblk", "conv", 3, "Black Hi - Tops" }, { "convheatred", "conv", 3, "Red Hi - Tops" },
	{ "convheatorn", "conv", 3, "Orange Hi - Tops" }, { "sneakerheatwht", "sneaker", 3, "White Low - Tops" },
	{ "sneakerheatgry", "sneaker", 3, "Gray Low - Tops" }, { "sneakerheatblk", "sneaker", 3, "Black Low - Tops" },
	{ "bask2heatwht", "bask1", 3, "White Hi - Tops" }, { "bask2heatband", "bask1", 3, "Strap Sneakers" },
	{ "timbergrey", "bask1", 3, "Gray Boots" }, { "timberred", "bask1", 3, "Red Boots" },
	{ "timberfawn", "bask1", 3, "Brown Boots" }, { "timberhike", "bask1", 3, "Hiking Boots" },
	{ "cowboyboot", "biker", 3, "Cowboy Boots" }, { "biker", "biker", 3, "Biker Boots" },
	{ "snakeskin", "biker", 3, "Snake Skin" }, { "shoedressblk", "shoe", 3, "Black Shoes" },
	{ "shoedressbrn", "shoe", 3, "Brown Shoes" }, { "shoespatz", "shoe", 3, "Spats" }
};

static SPlayerClothing sLeftLowerArmClothing [ LEFT_UPPER_ARM_CLOTHING_MAX ] =
{
	{ "4weed", "4WEED", 4, "Weed" }, { "4rip", "4RIP", 4, "Grave" },
	{ "4spider", "4SPIDER", 4, "Spider" }
};

static SPlayerClothing sLeftUpperArmClothing [ LEFT_UPPER_ARM_CLOTHING_MAX ] =
{
	{ "5gun", "5GUN", 5, "Gun" }, { "5cross", "5CROSS", 5, "Cross" },
	{ "5cross2", "5CROSS2", 5, "Rose" }, { "5cross3", "5CROSS3", 5, "Clown" }
};

static SPlayerClothing sRightUpperArmClothing [ RIGHT_UPPER_ARM_CLOTHING_MAX ] =
{
	{ "6aztec", "6AZTEC", 6, "Aztec" }, { "6crown", "6CROWN", 6, "Web" },
	{ "6clown", "6CLOWN", 6, "Clown" }, { "6africa", "6AFRICA", 6, "Africa" }
};

static SPlayerClothing sRightLowerArmClothing [ RIGHT_LOWER_ARM_CLOTHING_MAX ] =
{
	{ "7cross", "7CROSS", 7, "Cross" }, { "7cross2", "7CROSS2", 7, "Cross" },
	{ "7cross3", "7CROSS3", 7, "Cross" }, { "7mary", "7MARY", 7, "Mary" }
};

static SPlayerClothing sBackTopClothing [ BACK_TOP_CLOTHING_MAX ] =
{
	{ "8sa", "8SA", 8, "Grove St ." }, { "8sa2", "8SA2", 8, "San Andreas" },
	{ "8sa3", "8SA3", 8, "San Fierro" }, { "8westside", "8WESTSD", 8, "Westside" },
	{ "8santos", "8SANTOS", 8, "Los Santos" }, { "8poker", "8POKER", 8, "Card" },
	{ "8gun", "8GUN", 8, "Gun" }
};

static SPlayerClothing sLeftChestClothing [ LEFT_CHEST_CLOTHING_MAX ] =
{
	{ "9crown", "9CROWN", 9, "Crown" }, { "9gun", "9GUN", 9, "Gun" },
	{ "9gun2", "9GUN2", 9, "Gun" }, { "9homeboy", "9HOMBY", 9, "Homeboy" },
	{ "9bullet", "9BULLT", 9, "Bullet" }, { "9rasta", "9RASTA", 9, "Rasta" }
};

static SPlayerClothing sRightChestClothing [ RIGHT_CHEST_CLOTHING_MAX ] =
{
	{ "10ls", "10LS", 10, "Los Santos" }, { "10ls2", "10LS2", 10, "Los Santos" },
	{ "10ls3", "10LS3", 10, "Los Santos" }, { "10ls4", "10LS4", 10, "Los Santos" },
	{ "10ls5", "10LS5", 10, "Los Santos" }, { "10og", "10OG", 10, "O.G." },
	{ "10weed", "10WEED", 10, "Grove" }
};

static SPlayerClothing sStomachClothing [ STOMACH_CLOTHING_MAX ] =
{
	{ "11grove", "11GROVE", 11, "Grove" }, { "11grove2", "11GROV2", 11, "Grove" },
	{ "11grove3", "11GROV3", 11, "Grove" }, { "11dice", "11DICE", 11, "Dice" },
	{ "11dice2", "11DICE2", 11, "Dice" }, { "11jail", "11JAIL", 11, "Jail" },
	{ "11godsgift", "11GGIFT", 11, "God's Gift" }
};

static SPlayerClothing sLowerBackClothing [ LOWER_BACK_CLOTHING_MAX ] =
{
	{ "12angels", "12ANGEL", 12, "Angel" }, { "12mayabird", "12MAYBR", 12, "Mayan Bird" },
	{ "12dagger", "12DAGER", 12, "Dagger" }, { "12bandit", "12BNDIT", 12, "Masks" },
	{ "12cross7", "12CROSS", 12, "Cross" }, { "12mayaface", "12MYFAC", 12, "Mayan Face" }
};

static SPlayerClothing sChainsClothing [ CHAINS_CLOTHING_MAX ] =
{
	{ "dogtag", "neck", 13, "Dogtags" }, { "neckafrica", "neck", 13, "Africa Pendant" },
	{ "stopwatch", "neck", 13, "Stop Watch" }, { "necksaints", "neck", 13, "Saints Chain" },
	{ "neckhash", "neck", 13, "Leaf Chain" }, { "necksilver", "neck2", 13, "Silver Cuban" },
	{ "neckgold", "neck2", 13, "Gold Cuban" }, { "neckropes", "neck2", 13, "Silver Chain" },
	{ "neckropeg", "neck2", 13, "Gold Chain" }, { "neckls", "neck", 13, "L.S.Chain" },
	{ "neckdollar", "neck", 13, "Dollar Chain" }, { "neckcross", "neck", 13, "Cross Chain" }
};

static SPlayerClothing sWatchesClothing [ WATCHES_CLOTHING_MAX ] =
{
	{ "watchpink", "watch", 14, "Pink Watch" }, { "watchyellow", "watch", 14, "Yellow Watch" },
	{ "watchpro", "watch", 14, "Pro - Laps White" }, { "watchpro2", "watch", 14, "Pro - Laps Black" },
	{ "watchsub1", "watch", 14, "Face Watch" }, { "watchsub2", "watch", 14, "Face Black" },
	{ "watchzip1", "watch", 14, "Zip Blue" }, { "watchzip2", "watch", 14, "Zip Gold" },
	{ "watchgno", "watch", 14, "Gold Gnocchi" }, { "watchgno2", "watch", 14, "Silver Gnocchi" },
	{ "watchcro", "watch", 14, "Gold Crowex" }, { "watchcro2", "watch", 14, "Silver Crowex" }
};

static SPlayerClothing sGlassesClothing [ GLASSES_CLOTHING_MAX ] =
{
	{ "groucho", "grouchos", 15, "Joke Glasses" }, { "zorro", "zorromask", 15, "Joke Mask" },
	{ "eyepatch", "eyepatch", 15, "Eyepatch" }, { "glasses01", "glasses01", 15, "Black Shades" },
	{ "glasses04", "glasses04", 15, "Brown Shades" }, { "bandred3", "bandmask", 15, "Red Rag" },
	{ "bandblue3", "bandmask", 15, "Blue Rag" }, { "bandgang3", "bandmask", 15, "Green Rag" },
	{ "bandblack3", "bandmask", 15, "Black Rag" }, { "glasses01dark", "glasses01", 15, "Aviators" },
	{ "glasses04dark", "glasses04", 15, "Sun Glasses" }, { "glasses03", "glasses03", 15, "Green Tint" },
	{ "glasses03red", "glasses03", 15, "Red Tint" }, { "glasses03blue", "glasses03", 15, "Blue Tint" },
	{ "glasses03dark", "glasses03", 15, "Black Shades" }, { "glasses05dark", "glasses03", 15, "Black Shades" },
	{ "glasses05", "glasses03", 15, "Black Rim" },
};

static SPlayerClothing sHatsClothing [ HATS_CLOTHING_MAX ] =
{
	{ "bandred", "bandana", 16, "Red Rag Back" }, { "bandblue", "bandana", 16, "Blue Rag Back" },
	{ "bandgang", "bandana", 16, "Green Rag Back" }, { "bandblack", "bandana", 16, "Black Rag Back" },
	{ "bandred2", "bandknots", 16, "Red Rag Front" }, { "bandblue2", "bandknots", 16, "Blue Rag Front" },
	{ "bandblack2", "bandknots", 16, "Black Rag Front" }, { "bandgang2", "bandknots", 16, "Green Rag Front" },
	{ "capknitgrn", "capknit", 16, "Watch Cap" }, { "captruck", "captruck", 16, "Trucker Hat" },
	{ "cowboy", "cowboy", 16, "Cowboy Hat" }, { "hattiger", "cowboy", 16, "Leopard Cowboy" },
	{ "helmet", "helmet", 16, "Fullface Helmet" }, { "moto", "moto", 16, "MotoX Helmet" },
	{ "boxingcap", "boxingcap", 16, "Boxing Helmet" }, { "hockey", "hockeymask", 16, "Hockey Mask" },
	{ "capgang", "cap", 16, "Green Cap" }, { "capgangback", "capback", 16, "Green Cap(Back)" },
	{ "capgangside", "capside", 16, "Green Cap(Side)" }, { "capgangover", "capovereye", 16, "Grn Cap(Tilt)" },
	{ "capgangup", "caprimup", 16, "Grn Cap(Up)" }, { "bikerhelmet", "bikerhelmet", 16, "Helmut" },
	{ "capred", "cap", 16, "Red Cap" }, { "capredback", "capback", 16, "Red Cap(Back)" },
	{ "capredside", "capside", 16, "Red Cap(Side)" }, { "capredover", "capovereye", 16, "Red Cap(Tilt)" },
	{ "capredup", "caprimup", 16, "Red Cap(Up)" }, { "capblue", "cap", 16, "Blue Cap" },
	{ "capblueback", "capback", 16, "Blue Cap(Back)" }, { "capblueside", "capside", 16, "Blue Cap(Side)" },
	{ "capblueover", "capovereye", 16, "Blue Cap(Tilt)" }, { "capblueup", "caprimup", 16, "Blue Cap(Up)" },
	{ "skullyblk", "skullycap", 16, "Black Skully" }, { "skullygrn", "skullycap", 16, "Green Skully" },
	{ "hatmancblk", "hatmanc", 16, "Black Sun Hat" }, { "hatmancplaid", "hatmanc", 16, "Plaid Sun Hat" },
	{ "capzip", "cap", 16, "Cap" }, { "capzipback", "capback", 16, "Cap(Back)" },
	{ "capzipside", "capside", 16, "Cap(Side)" }, { "capzipover", "capovereye", 16, "Cap Tilted" },
	{ "capzipup", "caprimup", 16, "Cap Rim Up" }, { "beretred", "beret", 16, "Red Beret" },
	{ "beretblk", "beret", 16, "Black Beret" }, { "capblk", "cap", 16, "Black Cap" },
	{ "capblkback", "capback", 16, "Black Cap(Back)" }, { "capblkside", "capside", 16, "Black Cap(Side)" },
	{ "capblkover", "capovereye", 16, "Black Cap(Tilt)" }, { "capblkup", "caprimup", 16, "Black Cap(Up)" },
	{ "trilbydrk", "trilby", 16, "Dark Trilby" }, { "trilbylght", "trilby", 16, "Light Trilby" },
	{ "bowler", "bowler", 16, "Black Derby" }, { "bowlerred", "bowler", 16, "Red Derby" },
	{ "bowlerblue", "bowler", 16, "Blue Derby" }, { "bowleryellow", "bowler", 16, "Yellow Derby" },
	{ "boater", "boater", 16, "Gray Boater" }, { "bowlergang", "bowler", 16, "Green Derby" },
	{ "boaterblk", "boater", 16, "Black Boater" }
};

static SPlayerClothing sSpecialClothing [ SPECIAL_CLOTHING_MAX ] =
{
	{ "gimpleg", "gimpleg", 17, "Gimp Suit" }, { "valet", "valet", 17, "Valet Uniform" },
	{ "countrytr", "countrytr", 17, "Rural Clothes" }, { "croupier", "valet", 17, "Croupier" },
	{ "policetr", "policetr", 17, "Cop" }, { "balaclava", "balaclava", 17, "Ski Mask" },
	{ "pimptr", "pimptr", 17, "Pimp Suit" }, { "garageleg", "garagetr", 17, "Racing Suit" },
	{ "medictr", "medictr", 17, "Medic Uniform" }
};

const int CClothes::GetBodypartIdByName ( const char *szBodypartName )
{
	for ( size_t i = 0; i < PLAYER_CLOTHING_SLOTS; i++ )
	{
		if ( !strcmp ( szBodypartName, sClothingType [ i ].szName ) )
			return i;
	}

	return -1;
}

const SPlayerClothing  *CClothes::GetClothingGroupByName ( const char *szBodypartName )
{
	for ( size_t i = 0; i < PLAYER_CLOTHING_SLOTS; i++ )
	{
		if ( !strcmp(szBodypartName , sClothingType [ i ].szName) )
			return GetClothingGroup ( i );
	}
	return NULL;
}

const SPlayerClothing *CClothes::GetClothingGroup ( unsigned char ucType )
{
	if ( ucType < PLAYER_CLOTHING_SLOTS )
	{
		switch ( ucType )
		{
			case 0: return sTorsoClothing;         case 1: return sHairClothing;
			case 2: return sLegsClothing;          case 3: return sShoesClothing;
			case 4: return sLeftLowerArmClothing;  case 5: return sLeftUpperArmClothing;
			case 6: return sRightUpperArmClothing; case 7: return sRightLowerArmClothing;
			case 8: return sBackTopClothing;       case 9: return sLeftChestClothing;
			case 10: return sRightChestClothing;   case 11: return sStomachClothing;
			case 12: return sLowerBackClothing;    case 13: return sChainsClothing;
			case 14: return sWatchesClothing;      case 15: return sGlassesClothing;
			case 16: return sHatsClothing;         case 17: return sSpecialClothing;
		}
	}

	return 0;
}

const int CClothes::GetClothingGroupMax ( unsigned char ucType )
{
	if ( ucType < PLAYER_CLOTHING_SLOTS )
	{
		switch ( ucType )
		{
			case 0: return TORSO_CLOTHING_MAX;              case 1: return HAIR_CLOTHING_MAX;
			case 2: return LEGS_CLOTHING_MAX;               case 3: return SHOES_CLOTHING_MAX;
			case 4: return LEFT_LOWER_ARM_CLOTHING_MAX;     case 5: return LEFT_UPPER_ARM_CLOTHING_MAX;
			case 6: return RIGHT_UPPER_ARM_CLOTHING_MAX;    case 7: return RIGHT_LOWER_ARM_CLOTHING_MAX;
			case 8: return BACK_TOP_CLOTHING_MAX;           case 9: return LEFT_CHEST_CLOTHING_MAX;
			case 10: return RIGHT_CHEST_CLOTHING_MAX;       case 11: return STOMACH_CLOTHING_MAX;
			case 12: return LOWER_BACK_CLOTHING_MAX;        case 13: return CHAINS_CLOTHING_MAX;
			case 14: return WATCHES_CLOTHING_MAX;           case 15: return GLASSES_CLOTHING_MAX;
			case 16: return HATS_CLOTHING_MAX;              case 17: return SPECIAL_CLOTHING_MAX;
		}
	}

	return 0;
}