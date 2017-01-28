// X, Y, layer, Tileset, Tile
int L_SetMapImageData(lua_State* passedState){
	tileImageData* _theChosenData = GetMapImageData(lua_tonumber(passedState,1),lua_tonumber(passedState,2),lua_tonumber(passedState,3));
	_theChosenData->tileset=lua_tonumber(passedState,4);
	_theChosenData->tile=lua_tonumber(passedState,5);
	return 0;
}

// Sets an int pointer to a value.
// ARGS - 
// intpointer, value (int)
int L_SetInt(lua_State* passedState){
	int* passedInt = lua_touserdata(passedState,1);
	int toSet = lua_tonumber(passedState,2);
	(*passedInt)=toSet;
	return 0;
}
// Shows a message or a question
// ARGS - 
// message (string), isQuestion (bool)
// Return - 
// if isQuestion==true, returns true if answered yes, false if answered no.
int L_ShowMessage(lua_State* passedState){
	char* passedMessage = (char*)lua_tostring(passedState,1);
	char passedIsQuestion = lua_toboolean(passedState,2);
	char messageAnswer = ShowMessage(passedMessage,passedIsQuestion);
	if (passedIsQuestion!=0){
		lua_pushboolean(passedState,messageAnswer);
		return 1;
	}
	return 0;
}

// Shows a message or a question with a portrait
// ARGS - 
// message (string), isQuestion (bool), portrait (CrossTexture*), portraitScale (double 0 for autodetect to 200)
// Return - 
// if isQuestion==true, returns true if answered yes, false if answered no.
int L_ShowMessageWithPortrait(lua_State* passedState){
	char* passedMessage = (char*)lua_tostring(passedState,1);
	char passedIsQuestion = lua_toboolean(passedState,2);
	CrossTexture* passedPortrait = lua_touserdata(passedState,3);

	signed char passedScale = lua_tonumber(passedState,4);
	char messageAnswer = ShowMessageWithPortrait(passedMessage,passedIsQuestion,passedPortrait,passedScale);
	
	if (passedIsQuestion!=0){
		lua_pushboolean(passedState,messageAnswer);
		return 1;
	}
	return 0;
}

// Loads a PNG.
// ARGS - 
// path (string)
// Return - 
// an image pointer
int L_LoadPNG(lua_State* passedState){
	char* passedPath = (char*)lua_tostring(passedState,1);
	lua_pushlightuserdata(passedState,LoadPNG(passedPath));
	return 1;
}

// Destroys an image.
// ARGS - 
// some_imgage_pointer
int L_UnloadTexture(lua_State* passedState){
	PlzNoCrashOnDispose();
	FreeTexture(lua_touserdata(passedState,1));
	return 0;
}

// Set tileset slot to an image
// ARGS - 
// image, slot
int L_SetTileset(lua_State* passedState){
	tilesets[(int)lua_tonumber(passedState,2)]=(CrossTexture*)lua_touserdata(passedState,1);
	return 0;
}

// Destroy tileset
// ARGS
// slot
int L_DestroyTileset(lua_State* passedState){
	int passedNumber = lua_tonumber(passedState,1);

	PlzNoCrashOnDispose();
	FreeTexture(tilesets[passedNumber]);
	return 0;
}

// Adds a spell
// ARGS - 
// name, attack, magicAttack, texturePath
int L_AddAttack(lua_State* passedState){

	return 0;
}

// Returns a spell
// ARGS - 
// slot
// RETURN
// spell
int L_GetSpell(lua_State* passedState){
	lua_pushlightuserdata(passedState,&(GetSpellList(lua_tonumber(passedState,1))->theSpell));
	return 1;
}

// Adds to the end of the spell list.
// Returns
// spell
int L_AddSpell(lua_State* passedState){
	spellLinkedList* returnedSpell = AddToSpellList();
	returnedSpell->theSpell.theAnimation.texture=NULL;
	lua_pushlightuserdata(passedState, &(returnedSpell->theSpell));
	return 1;
}

// Sets spell stuff
// Args - 
// spell, name, atk, magicAttack, texturePath, mpCost, lastMinuteFix
// PLEASE DO NOT LOAD THE ANIMATION'S IMAGE. 
// lastMinuteFix is a last minute variable fix thing I made
// special values give special results
// 1 - damage unchanged by defence
int L_SetSpell(lua_State* passedState){
	spell* passedSpell = lua_touserdata(passedState,1);
	passedSpell->name = (char*)lua_touserdata(passedState,2);
	passedSpell->attack = lua_tonumber(passedState,3);
	passedSpell->magicAttack = lua_tonumber(passedState,4);
	passedSpell->spellPath = (char*)lua_touserdata(passedState,5);
	passedSpell->mpCost = lua_tonumber(passedState,6);
	if (lua_gettop(passedState)==7){
		passedSpell->lastMinuteFix = lua_tonumber(passedState,7);
	}
	return 0;
}

// Gets a spell's animation
// Args - 
// spell
int L_GetSpellAnimation(lua_State* passedState){
	spell* passedSpell = lua_touserdata(passedState,1);
	lua_pushlightuserdata(passedState,&(passedSpell->theAnimation));
	return 1;
}

// Sets an animation's values
// ARGS - 
// animation, speed, width, height, numberOfFrames, goBackwards, drawXOffset, drawYOffset, texture(optional)
// Set numberOfFrames to -1 for autodetect
int L_SetAnimation(lua_State* passedState){
	animation* passedAnimation = ((animation*)lua_touserdata(passedState,1));

	passedAnimation->speed = lua_tonumber(passedState,2);
	passedAnimation->width=lua_tonumber(passedState,3);
	passedAnimation->height = lua_tonumber(passedState,4);
	passedAnimation->goBackwards = lua_toboolean(passedState,6);
	passedAnimation->drawXOffset = lua_tonumber(passedState,7);
	passedAnimation->drawYOffset = lua_tonumber(passedState,8);
	passedAnimation->addOnChange=1;

	if (lua_gettop(passedState)==9){
		passedAnimation->texture = lua_touserdata(passedState,9);
	}
	if (lua_tonumber(passedState,5)==-1){
		if (passedAnimation->texture!=NULL){
			passedAnimation->numberOfFrames = (GetTextureWidth(passedAnimation->texture)/passedAnimation->width);
		}else{
			BasicMessage("No will work!");
		}
	}else{
		passedAnimation->numberOfFrames = lua_tonumber(passedState,5);
	}
	return 0;
}

// GEt possible enemy
// ARGS -
// slot
// RETURNS -
// stats* happy
int L_GetPossibleEnemies(lua_State* passedState){
	// Possible enemy they want
	int want = lua_tonumber(passedState,1);
	lua_pushlightuserdata(passedState,&(possibleEnemies[want]));
	return 1;
}

// Set party member data
// ARGS -
// partyMember* passedMember
// stats* passedStats
int L_SetPartyMember(lua_State* passedState){
	partyMember* passedMember = lua_touserdata(passedState,1);
	stats* passedStats = lua_touserdata(passedState,2);
	passedMember->fighterStats = *passedStats;
	return 0;
}


// Set stats data
// ARGS -
// stats* passedStats
// char level;
// short maxHp;
// short maxMp;
// unsigned char attack;
// unsigned char defence;
// unsigned char magicAttack;
// unsigned char magicDefence;
// unsigned char speed;
// short exp;
// char* name
int L_SetStats(lua_State* passedState){
	stats* passedMember = lua_touserdata(passedState,1);
	char level = lua_tonumber(passedState,2);
	short maxHp = lua_tonumber(passedState,3);
	short maxMp = lua_tonumber(passedState,4);
	unsigned char attack = lua_tonumber(passedState,5);
	unsigned char defence = lua_tonumber(passedState,6);
	unsigned char magicAttack = lua_tonumber(passedState,7);
	unsigned char magicDefence = lua_tonumber(passedState,8);
	unsigned char speed = lua_tonumber(passedState,9);
	short exp = lua_tonumber(passedState,10);

	passedMember->level=level;
	passedMember->maxHp=maxHp;
	passedMember->maxMp=maxMp;
	passedMember->attack=attack;
	passedMember->defence=defence;
	passedMember->magicAttack=magicAttack;
	passedMember->magicDefence=magicDefence;
	passedMember->speed=speed;
	passedMember->exp=exp;
	if (lua_gettop(passedState)==11){
		passedMember->name = (char*)lua_touserdata(passedState,11);
	}
	return 0;
}

// Get possible enemy animation
// ARGS
// slot
// animation wanted 1 for idle else for attack
int L_GetPossibleEnemyAnimation(lua_State* passedState){
	int slot = lua_tonumber(passedState,1);
	if (lua_tonumber(passedState,2)==1){
		lua_pushlightuserdata(passedState,&(possibleEnemiesIdleAnimation[slot]));
	}else{
		lua_pushlightuserdata(passedState,&(possibleEnemiesAttackAnimation[slot]));
	}
	return 1;
}

// Changes the map
// ARGS
// mapfilename
int L_ChangeMap(lua_State* passedState){
	char* passedFilename = (char*)lua_tostring(passedState,1);
	ChangeMap(passedFilename);
	return 0;
}

// Adds spell to passed stats
// ARGS
// stats* passedMember
// int passedSpellId
int L_AddSpellToStats(lua_State* passedState){
	stats* passedMember = lua_touserdata(passedState,1);
	int passedSpellId = lua_tonumber(passedState,2);

	AddSpellToStats(passedMember,passedSpellId);
	return 0;
}

/*
Returns the party's size
RETURNS
party's size
*/
int L_GetPartySize(lua_State* passedState){
	lua_pushnumber(passedState,partySize);
	return 1;
}

// Sets the party's size
// ARGS
// newsize
int L_SetPartySize(lua_State* passedState){
	partySize=lua_tonumber(passedState,1);
	return 0;
}

/*
Returns a party member's stats
ARGS
int partyMemberId

// OR, if we're talking about the struct, pass a partyMember*
*/
int L_GetPartyMemberStats(lua_State* passedState){
	if (lua_isnumber(passedState,1)){
		int passedId = lua_tonumber(passedState,1);
		lua_pushlightuserdata(passedState,&(party[passedId].fighterStats));
	}else{
		partyMember* passedMember = lua_touserdata(passedState,1);
		lua_pushlightuserdata(passedState,&(passedMember->fighterStats));
	}	
	return 1;
}

// Starts a wild battle
int L_StartWildBattle(lua_State* passedState){
	InitWildBattle();
	BattleInit();
	place=3;
	BattleLop(1);
	return 0;
}

// Pass string and will malloc and put that string in that memory and return
// light user data to that string
int L_MallocString(lua_State* passedState){
	const char* passedString = lua_tostring(passedState,1);
	char* returnString = malloc(strlen(passedString)+1);
	strcpy(returnString,passedString);
	lua_pushlightuserdata(passedState,returnString);
	return 1;
}

// Call before disposing images..actually, no.
// you don't really need this.
int L_PlzNoCrashOnDispose(lua_State* passedState){
	PlzNoCrashOnDispose();
	return 0;
}

// GETS A PARTY MEMBER'S ANIMATION
// ARGS -
// slot, whichone
// whichone is 1 for idle and 2 for attack
int L_GetPartyMemberAnimation(lua_State* passedState){
	int whichone = lua_tonumber(passedState,2);
	int passedSlot = lua_tonumber(passedState,1);

	if (whichone==1){
		lua_pushlightuserdata(passedState,&(partyIdleAnimation[passedSlot]));
	}else{
		lua_pushlightuserdata(passedState,&(partyAttackAnimation[passedSlot]));
	}

	return 1;
}

// Quick and ez debug message that doesn't display map in background
int L_DebugMsg(lua_State* passedState){
	BasicMessage((char*)lua_tostring(passedState,1));
	return 0;
}

// Restores party member's health
// ARGS
// slot
int L_RestorePartyMember(lua_State* passedState){
	int passedSlot = lua_tonumber(passedState,1);
	party[passedSlot].hp=party[passedSlot].fighterStats.maxHp;
	party[passedSlot].mp=party[passedSlot].fighterStats.maxMp;
	return 0;
}

// Return map stuff
// ARGS
// x, y
// RETURN
// tileimg, tileset, isSolid, blockevent
int L_GetMap(lua_State* passedState){
	int x = lua_tonumber(passedState,1);
	int y = lua_tonumber(passedState,2);
	int layer = lua_tonumber(passedState,3);
	lua_pushnumber(passedState,GetMapImageData(x,y,layer)->tile);
	lua_pushnumber(passedState,GetMapImageData(x,y,layer)->tileset);
	lua_pushboolean(passedState,GetMapSpotData(x,y)->isSolid);
	lua_pushnumber(passedState,GetMapSpotData(x,y)->event);
	return 4;
}

// Set map stuff
// ARGS
// isSolid, blockEvent
int L_SetMapOtherData(lua_State* passedState){
	int x = lua_tonumber(passedState,1);
	int y = lua_tonumber(passedState,2);
	GetMapSpotData(x,y)->isSolid=lua_toboolean(passedState,3);
	GetMapSpotData(x,y)->event=lua_tonumber(passedState,4);
	return 0;
}

// Sets the player's position
// ARGS
// x, y
// position is tiles
int L_SetPlayerPosition(lua_State* passedState){
	int x = lua_tonumber(passedState,1);
	int y = lua_tonumber(passedState,2);
	playerObject->x=x*32;
	playerObject->y=y*32;
	return 0;
}

// Wait a number of miliseconds
// ARGS
// miliseconds
int L_Wait(lua_State* passedState){
	int miliseconds = lua_tonumber(passedState,1);
	Wait(miliseconds);
	return 0;
}

// Redraws the map.
// Honestly, this was just for torches part
int L_RedrawMap(lua_State* passedState){
	UpdateCameraValues(playerObject);

	// Drawing
	StartDrawing();

	DrawMapThings();
	//DrawText(60,60,"Hello world",2);

	EndDrawing();
	return 0;
}

// Set's teh encounter rate
// ARGS
// theAmount
int L_SetEncounterRate(lua_State* passedState){
	encounterRate = lua_tonumber(passedState,1);
	nextEncounter = encounterRate+RandBetween(0,5);
	return 0;
}


// Allocate memory.
// ARGS
// memorySize
// ALT ARG
// true, presetId
// Presets will use certian sizes for you.
// presetId list
// 0 - animation
// 1 - stats
// 2 - partyMember
// RETURN
// void*
// This actually uses calloc and not malloc.
int L_Malloc(lua_State* passedState){
	if (lua_gettop(passedState)==2){
		if (lua_tonumber(passedState,2)==0){
			lua_pushlightuserdata(passedState,calloc(1,sizeof(animation)));
		}else if (lua_tonumber(passedState,2)==1){
			lua_pushlightuserdata(passedState,calloc(1,sizeof(stats)));
		}else if (lua_tonumber(passedState,2)==2){
			lua_pushlightuserdata(passedState,calloc(1,sizeof(partyMember)));
		}
	}else if (lua_gettop(passedState)==1){
		lua_pushlightuserdata(passedState,calloc(1,lua_tonumber(passedState,1)));
	}else{
		BasicMessage("Broken L_Malloc call, wrong args");
	}
	return 1;
}


// Free malloc memory
// ARGS
// void*
int L_Free(lua_State* passedState){
	free(lua_touserdata(passedState,1));
	return 0;
}

// Frees an animation's image
int L_FreeAnimationImage(lua_State* passedState){
	animation* passedAnimation = lua_touserdata(passedState,1);
	if (passedAnimation->texture!=NULL){
		FreeTexture(passedAnimation->texture);
		passedAnimation->texture=NULL;
	}
	return 0;
}

int L_Level(lua_State* passedState){
	int passedMemberId = lua_tonumber(passedState,1);
	int numLevels = lua_tonumber(passedState,2);
	int i=0;
	for (i=0;i<numLevels;i++){
		
		party[passedMemberId].fighterStats.level++;
		
		party[passedMemberId].fighterStats.attack+=3;
		party[passedMemberId].fighterStats.magicAttack+=3;
		party[passedMemberId].fighterStats.defence+=3;
		party[passedMemberId].fighterStats.magicDefence+=3;
		party[passedMemberId].fighterStats.speed+=3;
		party[passedMemberId].fighterStats.maxHp+=3;
		party[passedMemberId].fighterStats.maxMp+=3;
		party[passedMemberId].hp=party[passedMemberId].fighterStats.maxHp;
		party[passedMemberId].mp=party[passedMemberId].fighterStats.maxMp;
		
	
	}
	return 0;
}

// Starts a battle with the enemies you want
// ARGS
// numberOfEnemies, enemyPartyMember 0, enemyIdleAnimation 0, enemyAttackAnimation 0, enemyStats 1, enemyIdleAnimation 1, enemyAttackAnimation 1, etc...
// Max is 4 enemies
// You may ommit args for enemies unused.
// RETURN
// didwin(bool)
int L_StartSpecificBattle(lua_State* passedState){ 
	int i=0;
	// Current arg we're using. Start at 2 for first stats
	int argI=2;
	for (i=0;i<lua_tonumber(passedState,1);i++){
		enemies[i]=*((partyMember*)lua_touserdata(passedState,argI));
		enemies[i].hp=enemies[i].fighterStats.maxHp;
		enemies[i].mp=enemies[i].fighterStats.maxMp;
		argI++;
		enemyIdleAnimation[i]=(lua_touserdata(passedState,argI));
		argI++;
		enemyAttackAnimation[i]=(lua_touserdata(passedState,argI));
		argI++;
	}

	numberOfEnemies=lua_tonumber(passedState,1);

	BattleInit();
	place=3;
	lua_pushboolean(passedState,BattleLop(0));
	return 1;
}

// Sets spells
// ARGS
// stats*, spell 1, spell 2, etc
int L_SetStatsSpells(lua_State* passedState){
	stats* passedStats = lua_touserdata(passedState,1);
	int i=0;
	for (i=2;i<=lua_gettop(passedState);i++){
		passedStats->spells[i-2]=lua_tonumber(passedState,i);
	}
	return 0;
}

int L_GetLevel(lua_State* passedState){
	int passedMemberId = lua_tonumber(passedState,1);
	lua_pushnumber(passedState,party[passedMemberId].fighterStats.level);
	return 1;
}

int L_EndGame(lua_State* passedState){
	//ThanksForPlaying
	place=2;
	if (LANGUAGE==LANG_ENGLISH){
		ShowMessage("Thanks for playing!",0);
	}else if (LANGUAGE==LANG_SPANISH){
		ShowMessage("Gracias por jugar.",0);
	}
/*
	FILE * fp;
	// I don't know. You unlock the ability to save your language when you win??
   fp = fopen ("savedata0:didWin.nathan", "w+");
   fprintf(fp, "%d", LANGUAGE);
   fclose(fp);
   */
   return 0;
}

int L_ToBlack(lua_State* passedState){
	playerObject->x=1000;
	playerObject->y=600;
	int x,y,i;
	for (i=0;i<numberOfLayers;i++){
		for (y=0;y<tileOtherData.height;y++){
			for (x=0;x<tileOtherData.width;x++){
				GetMapImageData(x,y,i)->tile=1;
			}
		}
	}
	return 0;
}

// Makes Lua usefull.
void PushCFunctions(){
	lua_pushcfunction(L,L_SetMapImageData);
	lua_setglobal(L,"SetMapImageData");
	//
	lua_pushcfunction(L,L_SetInt);
	lua_setglobal(L,"SetInt");
	//
	lua_pushcfunction(L,L_ShowMessage);
	lua_setglobal(L,"ShowMessage");
	//
	lua_pushcfunction(L,L_LoadPNG);
	lua_setglobal(L,"LoadPNG");
	//
	lua_pushcfunction(L,L_UnloadTexture);
	lua_setglobal(L,"UnloadTexture");
	//
	lua_pushcfunction(L,L_SetTileset);
	lua_setglobal(L,"SetTileset");
	//
	lua_pushcfunction(L,L_GetSpell);
	lua_setglobal(L,"GetSpell");
	//
	lua_pushcfunction(L,L_AddSpell);
	lua_setglobal(L,"AddSpell");
	//
	lua_pushcfunction(L,L_SetSpell);
	lua_setglobal(L,"SetSpell");
	//
	lua_pushcfunction(L,L_SetAnimation);
	lua_setglobal(L,"SetAnimation");
	//
	lua_pushcfunction(L,L_GetSpellAnimation);
	lua_setglobal(L,"GetSpellAnimation");
	//
	lua_pushcfunction(L,L_GetPossibleEnemies);
	lua_setglobal(L,"GetMapEnemy");
	//
	lua_pushcfunction(L,L_GetPossibleEnemyAnimation);
	lua_setglobal(L,"GetMapEnemyAnimation");
	//
	lua_pushcfunction(L,L_SetPartyMember);
	lua_setglobal(L,"SetPartyMember");
	//
	lua_pushcfunction(L,L_SetStats);
	lua_setglobal(L,"SetStats");
	//
	lua_pushcfunction(L,L_ShowMessageWithPortrait);
	lua_setglobal(L,"ShowMessageWithPortrait");
	//
	lua_pushcfunction(L,L_ChangeMap);
	lua_setglobal(L,"ChangeMap");
	//
	lua_pushcfunction(L,L_AddSpellToStats);
	lua_setglobal(L,"AddSpellToStats");
	//
	lua_pushcfunction(L,L_GetPartyMemberStats);
	lua_setglobal(L,"GetPartyMemberStats");
	//
	lua_pushcfunction(L,L_GetPartySize);
	lua_setglobal(L,"GetPartySize");
	//
	lua_pushcfunction(L,L_StartWildBattle);
	lua_setglobal(L,"StartWildBattle");
	//
	lua_pushcfunction(L,L_DestroyTileset);
	lua_setglobal(L,"FreeTileset");
	//
	lua_pushcfunction(L,L_MallocString);
	lua_setglobal(L,"MallocString");
	//
	lua_pushcfunction(L,L_PlzNoCrashOnDispose);
	lua_setglobal(L,"PlzNoCrashOnDispose");
	//
	lua_pushcfunction(L,L_SetPartySize);
	lua_setglobal(L,"SetPartySize");
	//
	lua_pushcfunction(L,L_GetPartyMemberAnimation);
	lua_setglobal(L,"GetPartyMemberAnimation");
	//
	lua_pushcfunction(L,L_DebugMsg);
	lua_setglobal(L,"DebugMsg");
	//
	lua_pushcfunction(L,L_RestorePartyMember);
	lua_setglobal(L,"RestorePartyMember");
	//
	lua_pushcfunction(L,L_GetMap);
	lua_setglobal(L,"GetMap");
	//
	lua_pushcfunction(L,L_SetMapOtherData);
	lua_setglobal(L,"SetMapOtherData");
	//
	lua_pushcfunction(L,L_SetPlayerPosition);
	lua_setglobal(L,"SetPlayerPosition");
	//
	lua_pushcfunction(L,L_Wait);
	lua_setglobal(L,"Wait");
	//
	lua_pushcfunction(L,L_RedrawMap);
	lua_setglobal(L,"RedrawMap");
	//
	lua_pushcfunction(L,L_SetEncounterRate);
	lua_setglobal(L,"SetEncounterRate");
	//
	lua_pushcfunction(L,L_Malloc);
	lua_setglobal(L,"Malloc");
	//
	lua_pushcfunction(L,L_Free);
	lua_setglobal(L,"Free");
	//
	lua_pushcfunction(L,L_FreeAnimationImage);
	lua_setglobal(L,"FreeAnimationImage");
	//
	lua_pushcfunction(L,L_StartSpecificBattle);
	lua_setglobal(L,"StartSpecificBattle");
	//
	lua_pushcfunction(L,L_SetStatsSpells);
	lua_setglobal(L,"SetStatsSpells");
	//
	lua_pushcfunction(L,L_Level);
	lua_setglobal(L,"Level");
	//
	lua_pushcfunction(L,L_GetLevel);
	lua_setglobal(L,"GetLevel");
	//
	lua_pushcfunction(L,L_EndGame);
	lua_setglobal(L,"ThanksForPlaying");
	//
	lua_pushcfunction(L,L_ToBlack);
	lua_setglobal(L,"ToBlack");
}