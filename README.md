# SAMP-ShotDetect

This is an include that aims to detect whether or not the player is shooting a weapon
because SAMP (San Andreas Multiplayer) does not have a native function that does this.

## How to Use

```pawn
if(IsPlayerShooting(playerid)) //If the return value is true, the player is shooting
{
    return SendClientMessage(playerid, -1, "You are shooting");
}
else if(!IsPlayerShooting(playerid)) //If the return value is false, the player is not shooting
{
    return SendClientMessage(playerid, -1, "You're not shooting");
}
```

## Contribution

You can contribute to me by creating issues or sending pull requests.<br>
open-source project

## Credits

©️ Full credits to BitSain (https://github.com/BitSain)

## Contact

Send me an email: bitsaindeveloper@gmail.com<br>
Or also send a message via discord: