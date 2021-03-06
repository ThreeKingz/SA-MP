/****************************************************************************************************
 *                                                                                                  *
 *                                        Magic Wand Script                                         *
 *                                                                                                  *
 * Copyright � 2017 Abyss Morgan. All rights reserved.                                              *
 *                                                                                                  *
 * Download: https://github.com/AbyssMorgan/SA-MP/tree/master/include/engine                        *
 * Publication: http://forum.sa-mp.com/showthread.php?t=629050                                      *
 * Website: http://8.ct8.pl                                                                         *
 *                                                                                                  *
 * Plugins: Streamer, ColAndreas                                                                    *
 * Modules: 3DTryg                                                                                  *
 *                                                                                                  *
 * File Version: 1.1.1                                                                              *
 * SA:MP Version: 0.3.7 (REQUIRE)                                                                   *
 * Streamer Version: 2.9.1                                                                          *
 * ColAndreas Version: 1.4.0                                                                        *
 * 3DTryg Version: 4.5.2                                                                            *
 *                                                                                                  *
 * Functions:                                                                                       *
 * ToggleUseTeamMagicShoot(bool:toggle);            //default: false                                *
 * bool:IsToggleUseTeamMagicShoot();                                                                *
 * ToggleMagicShootForAll(bool:toggle);             //default: false                                *
 * bool:IsToggleMagicShootForAll();                                                                 *
 * ToggleMagicShootForPlayer(playerid,bool:toggle); //default: false                                *
 * bool:IsToggleMagicShootForPlayer(playerid);                                                      *
 * Float:GetMagicShootDamage(playerid);                                                             *
 * SetMagicShootDamage(playerid,Float:amount);                                                      *
 * Float:GetMagicShootSpeed(playerid);                                                              *
 * SetMagicShootSpeed(playerid,Float:speed);                                                        *
 * Float:GetMagicShootRange(playerid);                                                              *
 * SetMagicShootRange(playerid,Float:range);                                                        *
 * SetMagicProjectile(playerid,modelid=18693,Float:rx=90.0,Float:ry=0.0,Float:rz=0.0,               *
 *                    Float:offx=0.0,Float:offy=0.0,Float:offz=0.0);                                *
 * GetMagicProjectile(playerid,&modelid,&Float:rx=0.0,&Float:ry=0.0,&Float:rz=0.0,                  *
 *                    &Float:offx=0.0,&Float:offy=0.0,&Float:offz=0.0);                             *
 *                                                                                                  *
 ****************************************************************************************************/

#if defined _magic_shot
	#endinput
#endif
#define _magic_shot

#if (!defined GetPlayerPoolSize || !defined GetSVarInt)
	#error [ADM] This include requires SA:MP version 0.3.7 (github.com/AbyssMorgan/SA-MP/blob/master/samp/include)
#endif

#if ((defined Streamer_AppendArrayData) && (defined INVALID_STREAMER_ID) && (!defined STREAMER_TYPE_ACTOR))
	#error [ADM] You need Streamer v2.9.1 (github.com/samp-incognito/samp-streamer-plugin/releases)
#endif

#if !defined COLANDREAS
	#error [ADM] You need ColAndreas v1.4.0 (github.com/Pottus/ColAndreas/releases)
#endif

//Check Version 3DTryg.inc
#if !defined _3D_Tryg
	#error [ADM] You need 3DTryg.inc v4.5.2 (github.com/AbyssMorgan/SA-MP/blob/master/include/SAM/3DTryg.inc)
#elseif !defined Tryg3D_Version
	#error [ADM] Update you 3DTryg.inc to v4.5.2 (github.com/AbyssMorgan/SA-MP/blob/master/include/SAM/3DTryg.inc)
#elseif (Tryg3D_Version < 40502)
	#error [ADM] Update you 3DTryg.inc to v4.5.2 (github.com/AbyssMorgan/SA-MP/blob/master/include/SAM/3DTryg.inc)
#endif

#define MAGIC_SPEED 						(12.0)
#define MAGIC_MAX_RADIUS					(15.0)
#define MAGIC_REFRESH_ACTION				(1000)	//1 sec
#define MAGIC_DAMAGE						(10.0)

new MagicLockTarget[MAX_PLAYERS],
	MagicTime[MAX_PLAYERS],
	bool:MagicEnabledForPlayer[MAX_PLAYERS],
	Float:MagicShootDamage[MAX_PLAYERS],
	Float:MagicShootSpeed[MAX_PLAYERS],
	Float:MagicShootRange[MAX_PLAYERS],
	MagicP[MAX_PLAYERS][Float3D],
	bool:MagicEnabledForAll = false,
	bool:MagicUsePlayerTeam = false;

#define ToggleUseTeamMagicShoot(%0)			MagicUsePlayerTeam = (%0)
#define IsToggleUseTeamMagicShoot()			MagicUsePlayerTeam
#define ToggleMagicShootForAll(%0)			MagicEnabledForAll = (%0)
#define IsToggleMagicShootForAll()			MagicEnabledForAll
#define IsToggleMagicShootForPlayer(%0)		MagicEnabledForPlayer[(%0)]
#define GetMagicShootDamage(%0)				MagicShootDamage[(%0)]
#define SetMagicShootDamage(%0,%1)			MagicShootDamage[(%0)] = (%1)
#define GetMagicShootSpeed(%0)				MagicShootSpeed[(%0)]
#define SetMagicShootSpeed(%0,%1)			MagicShootSpeed[(%0)] = (%1)
#define GetMagicShootRange(%0)				MagicShootRange[(%0)]
#define SetMagicShootRange(%0,%1)			MagicShootRange[(%0)] = (%1)

stock SetMagicProjectile(playerid,modelid=18693,Float:rx=90.0,Float:ry=0.0,Float:rz=0.0,Float:offx=0.0,Float:offy=0.0,Float:offz=0.0){
	MagicP[playerid][T3D:INT] = modelid;
	MagicP[playerid][T3D:rX] = rx;
	MagicP[playerid][T3D:rY] = ry;
	MagicP[playerid][T3D:rZ] = rz;
	MagicP[playerid][T3D:X] = offx;
	MagicP[playerid][T3D:Y] = offy;
	MagicP[playerid][T3D:Z] = offz;
}

stock GetMagicProjectile(playerid,&modelid,&Float:rx=0.0,&Float:ry=0.0,&Float:rz=0.0,&Float:offx=0.0,&Float:offy=0.0,&Float:offz=0.0){
	modelid = MagicP[playerid][T3D:INT];
	rx = MagicP[playerid][T3D:rX];
	ry = MagicP[playerid][T3D:rY];
	rz = MagicP[playerid][T3D:rZ];
	offx = MagicP[playerid][T3D:X];
	offy = MagicP[playerid][T3D:Y];
	offz = MagicP[playerid][T3D:Z];
}

stock ToggleMagicShootForPlayer(playerid,bool:toggle){
	MagicEnabledForPlayer[playerid] = toggle;
	if(toggle){
		SetPlayerAttachedObject(playerid,7,338,6,0.080000,-0.010000,0.000000,0.000000,353.134094,65.000000,0.300000,0.300000,0.300000);
	} else {
		RemovePlayerAttachedObject(playerid,7);
	}
}

T3D:function MagicShot(playerid){
	new Float:mx, Float:my, Float:mz, Float:mrz, Float:mtx, Float:mty, Float:mtz, mvw, mint,
		Float:px, Float:py, Float:pz, mtime, pteam, Float:ldist = 20000, Float:dist;
	GetPlayerPos(playerid,mx,my,mz);
	GetPlayerFacingAngle(playerid,mrz);
	mvw = GetPlayerVirtualWorld(playerid);
	mint = GetPlayerInterior(playerid);
	GetPointInFront2D(mx,my,mrz,GetMagicShootRange(playerid),mtx,mty);
	MagicLockTarget[playerid] = INVALID_PLAYER_ID;
	pteam = GetPlayerTeam(playerid);
	
	Tryg3D::Foreach(i){
		if(i != playerid){
			if(!IsToggleUseTeamMagicShoot() || (IsToggleUseTeamMagicShoot() && (pteam != GetPlayerTeam(i) || GetPlayerTeam(i) == NO_TEAM || pteam == NO_TEAM))){
				if(Tryg3D::IsPlayerSpawned(i)){
					GetPlayerPos(i,px,py,pz);
					if(GetPlayerVirtualWorld(i) == mvw && GetPlayerInterior(i) == mint){
						if(IsPointInCylinder3D(px,py,pz,mx,my,mz,mtx,mty,mz,2.0)){
							if((dist = GetDistanceBetweenPoints3D(px,py,pz,mx,my,mz)) < ldist){
								ldist = dist;
								MagicLockTarget[playerid] = i;
							}
						}
					}
				}
			}
		}
	}
	
	ApplyAnimation(playerid,"KNIFE","knife_3",4.1,0,1,1,0,0,1);
	if(MagicLockTarget[playerid] == INVALID_PLAYER_ID){
		new STREAMER_TAG_OBJECT tmpobj = CreateDynamicObject(MagicP[playerid][T3D:INT],mx+MagicP[playerid][T3D:X],my+MagicP[playerid][T3D:Y],mz+MagicP[playerid][T3D:Z],MagicP[playerid][T3D:rX],MagicP[playerid][T3D:rY],CompRotationFloat(mrz+MagicP[playerid][T3D:rZ]),mvw,mint,-1,_,100.0);
		GetDynamicObjectPos(tmpobj,mx,my,mz);
		GetPointInFront3DCol(mx,my,mz,0.0,mrz,GetMagicShootRange(playerid),mtx,mty,mtz);		
		GetMoveTime(mx,my,mz,mtx,mty,mtz,GetMagicShootSpeed(playerid),mtime);
		MoveDynamicObject(tmpobj,mtx,mty,mtz,GetMagicShootSpeed(playerid),MagicP[playerid][T3D:rX],MagicP[playerid][T3D:rY],CompRotationFloat(mrz+MagicP[playerid][T3D:rZ]));
		SetTimerEx("MagicDestroy",mtime,0,"ddd",_:tmpobj,INVALID_PLAYER_ID,playerid);
	} else {
		GetPlayerPos(MagicLockTarget[playerid],px,py,pz);
		GetRotationFor2Point2D(mx,my,px,py,mrz);
		new STREAMER_TAG_OBJECT tmpobj = CreateDynamicObject(MagicP[playerid][T3D:INT],mx+MagicP[playerid][T3D:X],my+MagicP[playerid][T3D:Y],mz+MagicP[playerid][T3D:Z],MagicP[playerid][T3D:rX],MagicP[playerid][T3D:rY],CompRotationFloat(mrz+MagicP[playerid][T3D:rZ]),mvw,mint,-1,_,100.0);
		GetDynamicObjectPos(tmpobj,mx,my,mz);
		GetMoveTime(mx,my,mz,px,py,pz,GetMagicShootSpeed(playerid),mtime);
		MoveDynamicObject(tmpobj,px,py,pz,GetMagicShootSpeed(playerid),MagicP[playerid][T3D:rX],MagicP[playerid][T3D:rY],CompRotationFloat(mrz+MagicP[playerid][T3D:rZ]));
		SetTimerEx("MagicDestroy",mtime,0,"ddd",_:tmpobj,MagicLockTarget[playerid],playerid);
	}
	MagicTime[playerid] = GetTickCount();
	return MagicLockTarget[playerid];
}

T3D:function MagicDestroy(STREAMER_TAG_OBJECT objectid,targetid,playerid){
	if(IsValidDynamicObject(objectid)) DestroyDynamicObject(objectid);
	if(targetid != INVALID_PLAYER_ID){
		Tryg3D::GivePlayerDamage(targetid,GetMagicShootDamage(playerid),playerid,7,3);
		ApplyAnimation(targetid,"ped","SHOT_partial",4.1,0,1,1,1,1,1);
	}
	return 1;
}

T3D:function MagicInit(){
	for(new i = 0; i < MAX_PLAYERS; i++){
		SetMagicShootDamage(i,MAGIC_DAMAGE);
		SetMagicShootSpeed(i,MAGIC_SPEED);
		SetMagicShootRange(i,MAGIC_MAX_RADIUS);
		SetMagicProjectile(i);
	}
}

new bool:CRC_MagicInit = true;

//Hook: OnPlayerKeyStateChange
public OnPlayerKeyStateChange(playerid,newkeys,oldkeys){
	if(IsToggleMagicShootForAll() || IsToggleMagicShootForPlayer(playerid)){
		if(GetTickCount()-MagicTime[playerid] >= MAGIC_REFRESH_ACTION){
			if(GetPlayerWeapon(playerid) == 0 && Tryg3D::KeyPressed(KEY_FIRE) && !IsPlayerInAnyVehicle(playerid)){
				MagicShot(playerid);
			}
		}
	}
	#if defined Magic_OnPlayerKeyStateChange
		Magic_OnPlayerKeyStateChange(playerid,newkeys,oldkeys);
	#endif
	return 1;
}

#if defined _ALS_OnPlayerKeyStateChange
	#undef OnPlayerKeyStateChange
#else
	#define _ALS_OnPlayerKeyStateChange
#endif
#define OnPlayerKeyStateChange Magic_OnPlayerKeyStateChange
#if defined Magic_OnPlayerKeyStateChange
	forward Magic_OnPlayerKeyStateChange(playerid,newkeys,oldkeys);
#endif

//Hook: OnFilterScriptInit
public OnFilterScriptInit(){
	if(CRC_MagicInit){
		CRC_MagicInit = false;
		MagicInit();
	}
	#if defined Magic_OnFilterScriptInit
		Magic_OnFilterScriptInit();
	#endif
	return 1;
}

#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif
#define OnFilterScriptInit Magic_OnFilterScriptInit
#if defined Magic_OnFilterScriptInit
	forward Magic_OnFilterScriptInit();
#endif

//Hook: OnGameModeInit
public OnGameModeInit(){
	if(CRC_MagicInit){
		CRC_MagicInit = false;
		MagicInit();
	}
	#if defined Magic_OnGameModeInit
		Magic_OnGameModeInit();
	#endif
	return 1;
}

#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif
#define OnGameModeInit Magic_OnGameModeInit
#if defined Magic_OnGameModeInit
	forward Magic_OnGameModeInit();
#endif

//EOF