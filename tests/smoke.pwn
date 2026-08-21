#include <ShotDetect>

main()
{
	new weapon_class = SD_GetWeaponClass(24);
	return weapon_class == SD_WEAPON_CLASS_HANDGUN;
}
