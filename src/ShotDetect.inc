/*
    Include name: ShotDetect (SAMP)
    Last Update: 17/08/2024
    Include created by BitSain(bitsaindeveloper@gmail.com)
    Include open for modifications, but with my credits.
    
    * Include logic: First in "OnPlayerUpdate" we check if the player
    If you have a weapon in your hands, if so, we created a timer that checks from time to time --
    (interval between 1 shot from the weapon + 60 ms) if the player has spent one more ammunition, it means he is shooting;
    If he didn't spend any more, it means the player isn't shooting.
    
    * NOTE: The timer repeats until the player takes the weapon out of their hands. If the player changes weapons, we delete
    the timer for the old weapon and we created a timer for the new weapon, which also repeats between the intervals from one shot to the next.
    We also check whether the player holding the gun fired, before releasing the logic.    
    
    If there is a problem, create an issue in the include repository!
*/

#if !defined _inc_ShotDetect
	#endinput
#endif
#define _inc_ShotDetect

// ========== > Defines
#define SD_MAX_WEAPONS (47)
#define SD_DEBUG_MODE false

// ========== > Timer Functions
forward SD_LogicTimer(playerid); //"Logic Timer"

// ========== > Variables
new bool:SD_PlayerShooting[MAX_PLAYERS], //Player Shooting or not.
         SD_TPlayerShooting[MAX_PLAYERS], //Logic Timer to check whether you are shooting or not.
         SD_OldShotsFired[MAX_PLAYERS], //Old Expended Ammunition Count.
         SD_ShotsFired[MAX_PLAYERS],
         SD_PlayerCurrentWeapon[MAX_PLAYERS]; //Expended Ammunition Count.

// ========== > Weapon Info's
enum SD_E_WEAPON_INFO{
	SD_WEAPON_NAME[32], // A
    SD_WEAPON_SHOT_INTERVAL, // B
    bool:SD_WEAPON_VALID // C
};

new const SD_WeaInfo[SD_MAX_WEAPONS][SD_E_WEAPON_INFO] = {
	// A 							B 			C
    {"Fist", 					0, 		false},       // 0
    {"Brass Knuckles",  	0, 		false},       // 1
    {"Golf Club",  			0, 		false},       // 2
    {"NightStick", 			0, 		false},       // 3
    {"Knife", 					0, 		false},       // 4 
    {"Baseball Bat", 		0, 		false},       // 5
    {"Shovel", 				0, 		false},       // 6
    {"Pool Cue", 			0,		 	false},       // 7
    {"Katana",				0, 		false},       // 8
    {"Chainsaw", 			0, 		false},       // 9

    {"Purple Dildo", 		0, 		false},       // 10
    {"Dildo", 					0, 		false},       // 11
    {"Vibrator", 				0, 		false},       // 12
    {"Silver Vibrator",	0, 		false},       // 13
    {"Flowers", 				0, 		false},       // 14
    {"Cane", 					0, 		false},       // 15

    {"Grenade", 			0, 		false},      // 16
    {"Tear Gas",				0, 		false},      // 17
    {"Molotov CockTail",0, 		false},      // 18

    {"<null>", 0, false},       // 19
    {"<null>", 0, false},       // 20
    {"<null>", 0, false},       // 21

    {"9mm", 					300, 	true},      // 22
    {"Silenced 9mm", 	400, 	true},      // 23
    {"Desert Eagle",		800, 	true},      // 24

    {"Shotgun", 			1060, 	true},      // 25
    {"Sawnoff Shotgun",300, 	true},      // 26
    {"Combat Shotgun",320, 	true},      // 27

    {"Micro Uzi",			120, 	true},      // 28
    {"MP5",					100, 	true},      // 29
    {"AK-47", 				120, 	true},      // 30
    {"M4", 						120, 	true},      // 31
    {"Tec 9", 					120, 	true},      // 32

    {"Country Rifle", 		1060, 	true},     // 33
    {"Sniper", 				1060,	true},     // 34

    {"RPG", 					0, 		false},      // 35
    {"HS Socket", 			0, 		false},      // 36
    {"Flamethrower", 	0, 		false},       // 37
    {"Minigun",				20, 		true},      // 38

    {"Satchel Charge", 	0, 		false},      // 39
    {"Detonator",		 	0, 		false},      // 40
    {"Spray Paint",			0, 		false},       // 41
    {"Extinguisher", 		0, 		false},      // 42
    {"Camera", 				0, 		false},     // 43
    {"Night Vision", 		0, 		false},     // 44
    {"Thermal Vision", 	0, 		false},     // 45
    {"Parachute", 			0, 		false}      // 46
};

// ========== > STOCK ONLY FOR USE!!!
stock bool:IsPlayerShooting(playerid){
	return SD_PlayerShooting[playerid];
}

// ========== > Timer Functions
public SD_LogicTimer(playerid){ //"Logic Timer"
    if(SD_ShotsFired[playerid] > 0){
        if(SD_OldShotsFired[playerid] != SD_ShotsFired[playerid]){ //If the "Old Ammunition Spent" variable is not equal to "Current Ammunition Spent"
            if(!SD_PlayerShooting[playerid]) SD_PlayerShooting[playerid] = true; //If the "Player shooting" variable is false, set it to true.
            SD_OldShotsFired[playerid] = SD_ShotsFired[playerid]; //Set the variable "Old Expended Ammunition" with the value "Current Expended Ammunition"
            
            #if SD_DEBUG_MODE == true
           		print("[ShotDetect - DEBUG]: Player Shooting = true ( SD_LogicTimer )");
           	#endif
            return true;
        }
        else if(SD_OldShotsFired[playerid] == SD_ShotsFired[playerid]){ //Otherwise.. (Variable "Current Ammunition Spent" is equal to "Old Ammunition Spent")
            //KillTimer(SD_TPlayerShooting[playerid]); //Delete Logic Timer.
            SD_OldShotsFired[playerid] = 0; //Resets the "Old Expended Ammunition" Variable
            SD_ShotsFired[playerid] = 0; //Resets the "Current Ammunition Expended" Variable
            SD_PlayerShooting[playerid] = false; //Player is no longer shooting.

            #if SD_DEBUG_MODE == true
           		print("[ShotDetect - DEBUG]: Player Shooting = false ( SD_LogicTimer )");
           	#endif
            return false;
        }
    }
    return false;
}

// ========== > PUBLIC'S
public OnPlayerConnect(playerid){
	if(SD_TPlayerShooting[playerid]) KillTimer(SD_TPlayerShooting[playerid]); //Delete Logic Timer.
	SD_OldShotsFired[playerid] = 0; //Resets the "Old Expended Ammunition" Variable
	SD_ShotsFired[playerid] = 0; //Resets the "Current Ammunition Expended" Variable
	SD_PlayerShooting[playerid] = false; //Player is no longer shooting.
	SD_PlayerCurrentWeapon[playerid] = 0; // Reset Player Weapon Variable
	
	#if SD_DEBUG_MODE == true
		print("[ShotDetect - DEBUG]: OnPlayerConnect");
    #endif

	#if defined SD_OnPlayerConnect
		return SD_OnPlayerConnect(playerid);
	#else
	    return true;
	#endif
}

public OnPlayerDisconnect(playerid, reason){
	if(SD_TPlayerShooting[playerid]) KillTimer(SD_TPlayerShooting[playerid]); //Delete Logic Timer.
	SD_OldShotsFired[playerid] = 0; //Resets the "Old Expended Ammunition" Variable
	SD_ShotsFired[playerid] = 0; //Resets the "Current Ammunition Expended" Variable
	SD_PlayerShooting[playerid] = false; //Player is no longer shooting.
	SD_PlayerCurrentWeapon[playerid] = 0; // Reset Player Weapon Variable

	#if SD_DEBUG_MODE == true
		print("[ShotDetect - DEBUG]: OnPlayerDisconnect");
    #endif

	#if defined SD_OnPlayerDisconnect
		return SD_OnPlayerDisconnect(playerid, reason);
	#else
	    return true;
	#endif
}

public OnPlayerUpdate(playerid){
    new SD_pwea = GetPlayerWeapon(playerid);
    if((SD_pwea && SD_WeaInfo[SD_pwea][SD_WEAPON_VALID])){
        if(!SD_TPlayerShooting[playerid]){
            SD_PlayerCurrentWeapon[playerid] = SD_pwea;
            SD_TPlayerShooting[playerid] = SetTimerEx("SD_LogicTimer", (SD_WeaInfo[SD_PlayerCurrentWeapon[playerid]][SD_WEAPON_SHOT_INTERVAL] + 60), true, "d", playerid);
        	
        	#if SD_DEBUG_MODE == true
				print("[ShotDetect - DEBUG]: OnPlayerUpdate (2)");
		    #endif
        }
        else if(SD_PlayerCurrentWeapon[playerid] != SD_pwea){
        	KillTimer(SD_TPlayerShooting[playerid]);
        	SD_PlayerCurrentWeapon[playerid] = SD_pwea;
        	SD_TPlayerShooting[playerid] = SetTimerEx("SD_LogicTimer", (SD_WeaInfo[SD_PlayerCurrentWeapon[playerid]][SD_WEAPON_SHOT_INTERVAL] + 60), true, "d", playerid);

        	#if SD_DEBUG_MODE == true
				print("[ShotDetect - DEBUG]: OnPlayerUpdate (3)");
		    #endif
        }
    }
    else if(SD_TPlayerShooting[playerid]) //If the Player has no weapon in his hand, and if there is a logic timer, he will delete it.
        KillTimer(SD_TPlayerShooting[playerid]); //Deleting the timer

    // ====== ====== ====== ====== //
    
    #if SD_DEBUG_MODE == true
		print("[ShotDetect - DEBUG]: OnPlayerUpdate (1)");
    #endif

    #if defined SD_OnPlayerUpdate
		return SD_OnPlayerUpdate(playerid);
	#else
	    return true;
	#endif
}

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ){
    SD_ShotsFired[playerid]++; //Every time the player shoots, they will add +1 in spent ammunition.
    
    #if SD_DEBUG_MODE == true
		printf("[ShotDetect - DEBUG]: OnPlayerWeaponShot | ShotsFired: %d | OldShotsFired: %d", SD_ShotsFired[playerid], SD_OldShotsFired[playerid]);
    #endif

    // ====== ====== ====== ====== //
    
	#if defined SD_OnPlayerWeaponShot
		return SD_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, fX, Y, fZ);
	#else
	    return true;
	#endif
}

// ========== > OTHER'S
#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect SD_OnPlayerConnect
#if defined SD_OnPlayerConnect
    forward SD_OnPlayerConnect(playerid);
#endif

#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect SD_OnPlayerDisconnect
#if defined SD_OnPlayerDisconnect
    forward SD_OnPlayerDisconnect(playerid, reason);
#endif

#if defined _ALS_OnPlayerUpdate
	#undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif
#define OnPlayerUpdate SD_OnPlayerUpdate
#if defined SD_OnPlayerUpdate
    forward SD_OnPlayerUpdate(playerid);
#endif

#if defined _ALS_OnPlayerWeaponShot
	#undef OnPlayerWeaponShot
#else
	#define _ALS_OnPlayerWeaponShot
#endif
#define OnPlayerWeaponShot SD_OnPlayerWeaponShot
#if defined SD_OnPlayerWeaponShot
    forward SD_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
#endif

// EOF.