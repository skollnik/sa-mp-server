// A dialog in front of a command with many options: every entry runs an existing command (with the same checks and logs),
// optionally after asking for the missing value, so no command logic is written twice.
#define MAX_MENU_ITEMS          (40)
#define MENU_COMMAND_LENGTH     (48)
#define MENU_PROMPT_LENGTH      (72)

enum E_MENU_ITEM
{
    // Lowest admin level that sees the entry; 0 for everyone.
    miLevel,
    miLabel[40],
    // Command without the slash; %s is replaced with the menu's subject (a player, door or vehicle ID).
    miCommand[MENU_COMMAND_LENGTH],
    // Asked for and added at the end of the command; empty runs it right away.
    miPrompt[MENU_PROMPT_LENGTH]
}

new g_MenuCommand[MAX_PLAYERS][MAX_MENU_ITEMS][MENU_COMMAND_LENGTH];
new g_MenuPrompt[MAX_PLAYERS][MAX_MENU_ITEMS][MENU_PROMPT_LENGTH];
new g_MenuCount[MAX_PLAYERS];
new g_MenuSelected[MAX_PLAYERS];

stock Menu_Show(playerid, const title[], const items[][E_MENU_ITEM], count, const subject[] = "")
{
    new body[2048], level = g_Player[playerid][pAdminLevel], shown;
    for (new i = 0; i < count && shown < MAX_MENU_ITEMS; i++)
    {
        if (items[i][miLevel] > level)
        {
            continue;
        }
        format(g_MenuCommand[playerid][shown], MENU_COMMAND_LENGTH, items[i][miCommand], subject);
        strcopy(g_MenuPrompt[playerid][shown], items[i][miPrompt], MENU_PROMPT_LENGTH);
        strcat(body, items[i][miLabel]);
        strcat(body, "\n");
        shown++;
    }
    g_MenuCount[playerid] = shown;
    if (shown == 0)
    {
        Msg_Error(playerid, "Nemate dozvolu ni za jednu opciju ovog menija.");
        return;
    }
    ShowPlayerDialog(playerid, DIALOG_CMD_MENU, DIALOG_STYLE_LIST, title, body, "Izaberi", "Zatvori");
}

// A single command that only needs one value, without a list in front of it.
stock Menu_AskInput(playerid, const command[], const prompt[])
{
    strcopy(g_MenuCommand[playerid][0], command, MENU_COMMAND_LENGTH);
    strcopy(g_MenuPrompt[playerid][0], prompt, MENU_PROMPT_LENGTH);
    g_MenuCount[playerid] = 1;
    g_MenuSelected[playerid] = 0;
    ShowPlayerDialog(playerid, DIALOG_CMD_MENU_INPUT, DIALOG_STYLE_INPUT, "Unos", prompt, "U redu", "Odustani");
}

static stock Menu_Run(playerid, const command[], const value[] = "")
{
    new text[MENU_COMMAND_LENGTH + 130];
    if (value[0])
    {
        format(text, sizeof(text), "/%s %s", command, value);
    }
    else
    {
        format(text, sizeof(text), "/%s", command);
    }
    PC_EmulateCommand(playerid, text);
}

stock bool:Menu_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    switch (dialogid)
    {
        case DIALOG_CMD_MENU:
        {
            if (!response || listitem < 0 || listitem >= g_MenuCount[playerid])
            {
                return true;
            }
            if (!g_MenuPrompt[playerid][listitem][0])
            {
                Menu_Run(playerid, g_MenuCommand[playerid][listitem]);
                return true;
            }
            g_MenuSelected[playerid] = listitem;
            ShowPlayerDialog(playerid, DIALOG_CMD_MENU_INPUT, DIALOG_STYLE_INPUT, "Unos", g_MenuPrompt[playerid][listitem], "U redu", "Odustani");
        }
        case DIALOG_CMD_MENU_INPUT:
        {
            new item = g_MenuSelected[playerid];
            if (!response || item < 0 || item >= g_MenuCount[playerid])
            {
                return true;
            }
            if (isnull(inputtext))
            {
                ShowPlayerDialog(playerid, DIALOG_CMD_MENU_INPUT, DIALOG_STYLE_INPUT, "Unos", g_MenuPrompt[playerid][item], "U redu", "Odustani");
                return true;
            }
            Menu_Run(playerid, g_MenuCommand[playerid][item], inputtext);
        }
        default:
        {
            return false;
        }
    }
    return true;
}
