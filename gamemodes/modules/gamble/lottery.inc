// Half past every hour, so the draw never lands on the same minute as the payday.
#define LOTTERY_DRAW_MINUTE     (30)
#define COLOR_LOTTERY           (0x7FD17FFF)

new g_LotteryJackpot;
new g_LotteryLastHour = -1;
// Set from the draw query until its cleanup is queued, so no ticket is bought into a draw that is being settled.
new bool:g_LotteryDrawing;
new bool:g_LotteryPending[MAX_PLAYERS];
new g_LotteryTimer;

stock Lottery_Init()
{
    // One row per account: the primary key is what limits a player to a single ticket per draw.
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `lottery_tickets` (" \
        "`account_id` INT UNSIGNED NOT NULL PRIMARY KEY," \
        "`number` TINYINT UNSIGNED NOT NULL," \
        "`bought_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `server_vars` (" \
        "`name` VARCHAR(32) NOT NULL PRIMARY KEY," \
        "`value` INT NOT NULL DEFAULT 0" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    mysql_tquery(g_SQL, "SELECT `value` FROM `server_vars` WHERE `name` = 'lottery_jackpot'", "OnLotteryJackpotLoaded");

    new hour, minute, second;
    gettime(hour, minute, second);
    // A restart at half past must not draw a second time in the same hour.
    if (minute >= LOTTERY_DRAW_MINUTE)
    {
        g_LotteryLastHour = hour;
    }
    g_LotteryTimer = SetTimer("Lottery_MinuteTick", 60 * 1000, true);
}

stock Lottery_Exit()
{
    KillTimer(g_LotteryTimer);
}

stock Lottery_OnPlayerConnect(playerid)
{
    g_LotteryPending[playerid] = false;
}

forward OnLotteryJackpotLoaded();
public OnLotteryJackpotLoaded()
{
    if (cache_num_rows() > 0)
    {
        cache_get_value_name_int(0, "value", g_LotteryJackpot);
    }
    printf("[LOTTERY] Fond: $%d", g_LotteryJackpot);
    return 1;
}

static stock Lottery_SaveJackpot()
{
    new query[160];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `server_vars` (`name`, `value`) VALUES ('lottery_jackpot', %d) ON DUPLICATE KEY UPDATE `value` = %d",
        g_LotteryJackpot, g_LotteryJackpot);
    mysql_tquery(g_SQL, query);
}

// The winner may be offline: then the bank balance is raised in the database, with the same log row a payment online gets.
static stock Lottery_Pay(account_id, amount)
{
    new playerid = Account_FindOnlineByDbId(account_id);
    if (playerid != INVALID_PLAYER_ID)
    {
        Money_GiveBank(playerid, amount, "dobitak na lutriji");
        Account_Save(playerid);
        Msg_Success(playerid, "Dobili ste $%d na lutriji! Novac je na vasem racunu u banci.", amount);
        return;
    }

    new query[256];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `bank` = `bank` + %d WHERE `id` = %d", amount, account_id);
    mysql_tquery(g_SQL, query);
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `money_logs` (`account_id`, `kind`, `amount`, `balance`, `reason`) " \
        "SELECT `id`, 'bank', %d, `bank`, 'dobitak na lutriji' FROM `accounts` WHERE `id` = %d",
        amount, account_id);
    mysql_tquery(g_SQL, query);
}

static stock Lottery_Draw()
{
    g_LotteryDrawing = true;
    new number = random(LOTTERY_MAX_NUMBER) + 1;
    mysql_tquery(g_SQL,
        "SELECT t.`account_id`, t.`number`, a.`name` FROM `lottery_tickets` t JOIN `accounts` a ON a.`id` = t.`account_id`",
        "OnLotteryDraw", "d", number);
}

forward OnLotteryDraw(number);
public OnLotteryDraw(number)
{
    new rows = cache_num_rows(), winners, ticket, account_id, name[MAX_PLAYER_NAME + 1];
    new winner_ids[MAX_PLAYERS], names[256];
    for (new i = 0; i < rows; i++)
    {
        cache_get_value_name_int(i, "number", ticket);
        if (ticket != number || winners >= sizeof(winner_ids))
        {
            continue;
        }
        cache_get_value_name_int(i, "account_id", account_id);
        cache_get_value_name(i, "name", name);
        winner_ids[winners++] = account_id;
        if (strlen(names) + strlen(name) + 2 < sizeof(names))
        {
            if (names[0]) strcat(names, ", ");
            strcat(names, name);
        }
    }

    mysql_tquery(g_SQL, "DELETE FROM `lottery_tickets`");
    g_LotteryDrawing = false;

    if (rows == 0 && g_LotteryJackpot == 0)
    {
        return 1;
    }

    new message[192];
    if (winners == 0)
    {
        format(message, sizeof(message), "[LUTRIJA] Izvucen je broj %d. Niko nije pogodio, fond od $%d se prenosi na sledece izvlacenje (/loto).", number, g_LotteryJackpot);
        Chat_SendToAll(COLOR_LOTTERY, message);
        return 1;
    }

    // Split evenly; the remainder of the division stays in the fund.
    new share = g_LotteryJackpot / winners;
    for (new i = 0; i < winners; i++)
    {
        Lottery_Pay(winner_ids[i], share);
    }
    g_LotteryJackpot -= share * winners;
    Lottery_SaveJackpot();

    format(message, sizeof(message), "[LUTRIJA] Izvucen je broj %d! Dobitak od $%d osvaja: %s.", number, share, names);
    Chat_SendToAll(COLOR_LOTTERY, message);
    return 1;
}

forward Lottery_MinuteTick();
public Lottery_MinuteTick()
{
    // A draw whose query never answered must not block ticket sales for good.
    g_LotteryDrawing = false;
    new hour, minute, second;
    gettime(hour, minute, second);
    if (minute == LOTTERY_DRAW_MINUTE && hour != g_LotteryLastHour)
    {
        g_LotteryLastHour = hour;
        Lottery_Draw();
    }
    return 1;
}

static stock Lottery_NextDrawText(text[], size = sizeof(text))
{
    new hour, minute, second;
    gettime(hour, minute, second);
    if (minute >= LOTTERY_DRAW_MINUTE)
    {
        hour = (hour + 1) % 24;
    }
    format(text, size, "%02d:%02d", hour, LOTTERY_DRAW_MINUTE);
}

forward OnLotteryTicketShown(playerid, race);
public OnLotteryTicketShown(playerid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    new next[8], ticket[48];
    Lottery_NextDrawText(next);
    if (cache_num_rows() > 0)
    {
        new number;
        cache_get_value_name_int(0, "number", number);
        format(ticket, sizeof(ticket), "vas broj je %d", number);
    }
    else
    {
        strcopy(ticket, "nemate tiket");
    }
    Msg_Info(playerid, "Lutrija: fond $%d, sledece izvlacenje u %s, %s. Tiket: /loto [1-%d] ($%d) u prodavnici 24/7.", g_LotteryJackpot, next, ticket, LOTTERY_MAX_NUMBER, LOTTERY_TICKET_PRICE);
    return 1;
}

forward OnLotteryTicketBought(playerid, race, account_id, number);
public OnLotteryTicketBought(playerid, race, account_id, number)
{
    new bool:bought = cache_affected_rows() > 0;
    if (race != g_AccountRace[playerid])
    {
        // The player left while the ticket was being written; a refused ticket still owes them the price.
        if (!bought)
        {
            new query[128];
            mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `cash` = `cash` + %d WHERE `id` = %d", LOTTERY_TICKET_PRICE, account_id);
            mysql_tquery(g_SQL, query);
        }
        else
        {
            g_LotteryJackpot += LOTTERY_TICKET_PRICE * LOTTERY_POOL_PERCENT / 100;
            Lottery_SaveJackpot();
        }
        return 1;
    }
    g_LotteryPending[playerid] = false;

    if (!bought)
    {
        Money_GiveCash(playerid, LOTTERY_TICKET_PRICE, "lutrija - tiket vracen");
        Msg_Error(playerid, "Vec imate tiket za ovo izvlacenje. Jedan igrac, jedan tiket.");
        return 1;
    }

    g_LotteryJackpot += LOTTERY_TICKET_PRICE * LOTTERY_POOL_PERCENT / 100;
    Lottery_SaveJackpot();
    Account_Save(playerid);

    new next[8];
    Lottery_NextDrawText(next);
    Msg_Success(playerid, "Kupili ste tiket sa brojem %d. Izvlacenje je u %s, fond je $%d.", number, next, g_LotteryJackpot);
    return 1;
}

cmd:loto(playerid, params[])
{
    new number;
    if (sscanf(params, "d", number))
    {
        new query[96];
        mysql_format(g_SQL, query, sizeof(query), "SELECT `number` FROM `lottery_tickets` WHERE `account_id` = %d", g_Player[playerid][pDbId]);
        mysql_tquery(g_SQL, query, "OnLotteryTicketShown", "dd", playerid, g_AccountRace[playerid]);
        return 1;
    }
    if (number < 1 || number > LOTTERY_MAX_NUMBER)
    {
        Msg_Usage(playerid, "/loto [broj 1-%d] (bez broja: fond i vas tiket)", LOTTERY_MAX_NUMBER);
        return 1;
    }
    if (!Point_Require(playerid, POINT_SHOP))
    {
        return 1;
    }
    if (g_LotteryDrawing || g_LotteryPending[playerid])
    {
        Msg_Error(playerid, "Izvlacenje je u toku, pokusajte za trenutak.");
        return 1;
    }
    if (!Money_Take(playerid, LOTTERY_TICKET_PRICE, "lutrija - tiket"))
    {
        return 1;
    }

    // Paid first; the insert is ignored when the player already has a ticket, and then the price goes back.
    g_LotteryPending[playerid] = true;
    new query[160];
    mysql_format(g_SQL, query, sizeof(query), "INSERT IGNORE INTO `lottery_tickets` (`account_id`, `number`) VALUES (%d, %d)", g_Player[playerid][pDbId], number);
    mysql_tquery(g_SQL, query, "OnLotteryTicketBought", "dddd", playerid, g_AccountRace[playerid], g_Player[playerid][pDbId], number);
    return 1;
}
