// Every system message goes through these so the whole server shares one look:
// a colored tag, then white text. The text must be a string literal; extra arguments are formatted.
#define Msg_Usage(%0,%1)    SendClientMessage(%0, COLOR_SERVER, "KORISCENJE: " C_WHITE %1)
#define Msg_Error(%0,%1)    SendClientMessage(%0, COLOR_RED, "[GRESKA] " C_WHITE %1)
#define Msg_Success(%0,%1)  SendClientMessage(%0, COLOR_GREEN, "[USPEH] " C_WHITE %1)
#define Msg_Server(%0,%1)   SendClientMessage(%0, COLOR_SERVER, "[" SERVER_NAME "] " C_WHITE %1)
#define Msg_Info(%0,%1)     SendClientMessage(%0, COLOR_GREY, "[INFO] " C_WHITE %1)
#define Msg_Admin(%0,%1)    SendClientMessage(%0, COLOR_ADMIN, "[ADMIN] " C_WHITE %1)
#define Msg_Helper(%0,%1)   SendClientMessage(%0, COLOR_HELPER, "[HELPER] " C_WHITE %1)
