#ifndef VERIFICATIONS
#define VERIFICATIONS

#include "Main.h"

void VerifyIP(int argc,char **argv,Server *info);
int ContainLetter(char *argv);
char *ChooseID(char *nodelist);
int verify_id_is_used(char *nodeslist,char *ID);

#endif