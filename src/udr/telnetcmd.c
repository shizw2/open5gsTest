#include "telnet.h"
#include "context.h"
#include "ogs-sbi.h"
char                g_chCmdName[128] = {0};
T_pttCmdParas       g_tCmdPara[128];
void setCommands(void);

telnet_command_t g_commands[] = {
    {"shownf",      (GenericFunc)shownf,         1, {STRING}},
};
int g_numCommands = sizeof(g_commands) / sizeof(g_commands[0]);

void setCommands(void){
    set_telnet_commands(g_commands, g_numCommands);
}
