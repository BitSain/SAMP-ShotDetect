#include <open.mp>
#include <ShotDetect>

public OnPlayerUpdate(playerid)
{
	if (IsPlayerShooting(playerid))
	{
		new weaponid = SD_GetPlayerShootingWeapon(playerid);
		printf("[ShotDetect] playerid=%d weaponid=%d", playerid, weaponid);
	}
	return 1;
}
