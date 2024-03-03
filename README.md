# SAMP-ShotDetect

Esta é uma include que tem de objetivo detectar se o jogador está ou não atirando com uma arma
pois o SAMP (San Andreas Multiplayer) não tem uma função nativa que faz isso.

## Modo de Uso

```pawn
if(IsPlayerShooting(playerid)) //If the return value is true, the player is shooting
{
    return SendClientMessage(playerid, -1, "Você está Atirando");
}
else if(!IsPlayerShooting(playerid)) //If the return value is false, the player is not shooting
{
    return SendClientMessage(playerid, -1, "Você não esta Atirando");
}
```

## Contribuição

Você pode contribuir comigo criando issues ou enviando solicitações de pull.<br>
Projeto open-source

## Creditos

©️ Créditos totais ao BitSain (https://github.com/BitSain)

## Contato

Me envie um e-mail: bitsaindeveloper@gmail.com<br>
Ou envie mensagem também pelo discord: XXXX
