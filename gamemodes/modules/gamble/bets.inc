#define BET_OFFER_SECONDS       (30)
#define BET_OFFER_COOLDOWN      (10)

enum E_BET_GAME
{
    BET_DICE,
    BET_COIN
}

// Kept on the player the offer was sent to; the sender's race guards against a different player in the same slot.
enum E_BET_OFFER
{
    boFrom,
    boFromRace,
    E_BET_GAME:boGame,
    boStake,
    bool:boFromHeads,
    boExpires
}

new g_BetOffer[MAX_PLAYERS][E_BET_OFFER];
new g_BetLastOffer[MAX_PLAYERS];

stock Bet_OnPlayerConnect(playerid)
{
    g_BetOffer[playerid][boFrom] = INVALID_PLAYER_ID;
    g_BetLastOffer[playerid] = 0;
}

// Checked when the offer is made and again when it is accepted; errors go to playerid.
static stock bool:Bet_Check(playerid, otherid, stake)
{
    if (g_Player[playerid][pLevel] < BET_MIN_LEVEL || g_Player[otherid][pLevel] < BET_MIN_LEVEL)
    {
        Msg_Error(playerid, "Opklade su dozvoljene od nivoa %d (za oba igraca).", BET_MIN_LEVEL);
        return false;
    }
    if (!Chat_CheckBeside(playerid, otherid, GIVE_CASH_RADIUS))
    {
        return false;
    }
    if (g_Player[playerid][pCash] < stake)
    {
        Msg_Error(playerid, "Nemate $%d kod sebe.", stake);
        return false;
    }
    if (g_Player[otherid][pCash] < stake)
    {
        Msg_Error(playerid, "%s nema dovoljno novca kod sebe za taj ulog.", Account_Name(otherid));
        return false;
    }
    return true;
}

static stock Bet_Offer(playerid, targetid, E_BET_GAME:game, stake, bool:heads)
{
    if (!Staff_CheckTarget(playerid, targetid, "Ne mozete se kladiti sami sa sobom."))
    {
        return;
    }
    if (stake < BET_MIN_STAKE || stake > BET_MAX_STAKE)
    {
        Msg_Error(playerid, "Ulog je od $%d do $%d.", BET_MIN_STAKE, BET_MAX_STAKE);
        return;
    }
    new now = gettime();
    if (now - g_BetLastOffer[playerid] < BET_OFFER_COOLDOWN)
    {
        Msg_Error(playerid, "Sacekajte malo pre nove ponude.");
        return;
    }
    if (g_BetOffer[targetid][boFrom] != INVALID_PLAYER_ID && g_BetOffer[targetid][boExpires] > now)
    {
        Msg_Error(playerid, "Taj igrac vec ima ponudu za opkladu. Pokusajte za malo.");
        return;
    }
    if (!Bet_Check(playerid, targetid, stake))
    {
        return;
    }

    g_BetLastOffer[playerid] = now;
    g_BetOffer[targetid][boFrom] = playerid;
    g_BetOffer[targetid][boFromRace] = g_AccountRace[playerid];
    g_BetOffer[targetid][boGame] = game;
    g_BetOffer[targetid][boStake] = stake;
    g_BetOffer[targetid][boFromHeads] = heads;
    g_BetOffer[targetid][boExpires] = now + BET_OFFER_SECONDS;

    new body[256];
    if (game == BET_DICE)
    {
        format(body, sizeof(body), C_WHITE "%s vam nudi bacanje kockica.\nUlog: " C_GREEN "$%d" C_WHITE " (pobednik uzima $%d).\n\n" C_GREY "Ko baci veci zbir, pobedjuje. Nereseno vraca uloge.",
            Account_Name(playerid), stake, stake * 2);
    }
    else
    {
        format(body, sizeof(body), C_WHITE "%s vam nudi bacanje novcica.\nUlog: " C_GREEN "$%d" C_WHITE " (pobednik uzima $%d).\n\n" C_GREY "%s bira %s, vi ste %s.",
            Account_Name(playerid), stake, stake * 2, Account_Name(playerid), heads ? ("glavu") : ("pismo"), heads ? ("pismo") : ("glava"));
    }
    ShowPlayerDialog(targetid, DIALOG_BET_OFFER, DIALOG_STYLE_MSGBOX, "Opklada", body, "Prihvati", "Odbij");
    Msg_Info(playerid, "Ponuda je poslata igracu %s. Ceka se odgovor (%d s).", Account_Name(targetid), BET_OFFER_SECONDS);
}

static stock Bet_RollDice(playerid)
{
    new first = random(6) + 1, second = random(6) + 1, action[64];
    format(action, sizeof(action), "baca kockice: %d i %d (zbir %d).", first, second, first + second);
    Chat_Action(playerid, action);
    return first + second;
}

// Both stakes are taken before the roll and the winner gets both, so no money is created or lost.
static stock Bet_Play(playerid, fromid)
{
    new stake = g_BetOffer[playerid][boStake];
    Money_GiveCash(fromid, -stake, "ulog u opkladi", g_Player[playerid][pDbId]);
    Money_GiveCash(playerid, -stake, "ulog u opkladi", g_Player[fromid][pDbId]);

    new winner = INVALID_PLAYER_ID;
    if (g_BetOffer[playerid][boGame] == BET_DICE)
    {
        new from_sum = Bet_RollDice(fromid), sum = Bet_RollDice(playerid);
        if (from_sum != sum)
        {
            winner = from_sum > sum ? fromid : playerid;
        }
    }
    else
    {
        new bool:heads = random(2) == 0, action[48];
        format(action, sizeof(action), "baca novcic... pada %s.", heads ? ("glava") : ("pismo"));
        Chat_Action(fromid, action);
        winner = heads == g_BetOffer[playerid][boFromHeads] ? fromid : playerid;
    }

    if (winner == INVALID_PLAYER_ID)
    {
        Money_GiveCash(fromid, stake, "nereseno u opkladi", g_Player[playerid][pDbId]);
        Money_GiveCash(playerid, stake, "nereseno u opkladi", g_Player[fromid][pDbId]);
        Msg_Info(fromid, "Nereseno, ulog od $%d vam je vracen.", stake);
        Msg_Info(playerid, "Nereseno, ulog od $%d vam je vracen.", stake);
    }
    else
    {
        new loser = winner == fromid ? playerid : fromid, action[64];
        Money_GiveCash(winner, stake * 2, "dobitak u opkladi", g_Player[loser][pDbId]);
        format(action, sizeof(action), "uzima $%d dobitka.", stake * 2);
        Chat_Action(winner, action);
        Msg_Success(winner, "Pobedili ste i dobili $%d.", stake * 2);
        Msg_Info(loser, "Izgubili ste ulog od $%d.", stake);
    }
    Account_Save(fromid);
    Account_Save(playerid);
}

stock bool:Bet_OnDialogResponse(playerid, dialogid, response)
{
    if (dialogid != DIALOG_BET_OFFER)
    {
        return false;
    }

    new fromid = g_BetOffer[playerid][boFrom];
    g_BetOffer[playerid][boFrom] = INVALID_PLAYER_ID;
    if (fromid == INVALID_PLAYER_ID || !Account_IsLoggedIn(fromid) || g_AccountRace[fromid] != g_BetOffer[playerid][boFromRace])
    {
        Msg_Error(playerid, "Igrac koji je ponudio opkladu vise nije tu.");
        return true;
    }
    if (!response)
    {
        Msg_Info(fromid, "%s je odbio opkladu.", Account_Name(playerid));
        return true;
    }
    if (gettime() > g_BetOffer[playerid][boExpires])
    {
        Msg_Error(playerid, "Ponuda je istekla.");
        return true;
    }
    if (!Bet_Check(playerid, fromid, g_BetOffer[playerid][boStake]))
    {
        Msg_Info(fromid, "Opklada sa %s nije odigrana.", Account_Name(playerid));
        return true;
    }
    Bet_Play(playerid, fromid);
    return true;
}

cmd:kockice(playerid, params[])
{
    new targetid, stake;
    if (sscanf(params, "ud", targetid, stake))
    {
        Msg_Usage(playerid, "/kockice [id ili ime] [ulog $%d-%d] (igrac mora biti pored vas)", BET_MIN_STAKE, BET_MAX_STAKE);
        return 1;
    }
    Bet_Offer(playerid, targetid, BET_DICE, stake, false);
    return 1;
}

cmd:novcic(playerid, params[])
{
    new targetid, stake, side[8];
    if (sscanf(params, "uds[8]", targetid, stake, side) || (strcmp(side, "glava", true) && strcmp(side, "pismo", true)))
    {
        Msg_Usage(playerid, "/novcic [id ili ime] [ulog $%d-%d] [glava/pismo]", BET_MIN_STAKE, BET_MAX_STAKE);
        return 1;
    }
    Bet_Offer(playerid, targetid, BET_COIN, stake, !strcmp(side, "glava", true));
    return 1;
}
