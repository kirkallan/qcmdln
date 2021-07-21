/*****************************************************************************
 *
 *   File Name:      qcmdln.c
 *
 *****************************************************************************/
/*****************************************************************************
 *                                                                           *
 * Copyright (C) 2017-2021 Unpublished Work of SUSE. All Rights Reserved.
 *
 * THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL, PROPRIETARY
 * AND TRADE SECRET INFORMATION OF SUSE, INC. ACCESS TO THIS WORK IS
 * RESTRICTED TO (I) SUSE. EMPLOYEES WHO HAVE A NEED TO KNOW HOW TO
 * PERFORM TASKS WITHIN THE SCOPE OF THEIR ASSIGNMENTS AND (II) ENTITIES
 * OTHER THAN SUSE. WHO HAVE ENTERED INTO APPROPRIATE LICENSE
 * AGREEMENTS. NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
 * COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED,
 * EXPANDED, COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED OR ADAPTED
 * WITHOUT THE PRIOR WRITTEN CONSENT OF SUSE. ANY USE OR EXPLOITATION
 * OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO
 * CRIMINAL AND CIVIL LIABILITY.
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#define __CDECL __cdecl
#else
#define __CDECL
#endif

int eat_white_space(FILE *fp)
{
    int c;

    do {
        c = fgetc(fp);
    } while (c <= ' ' && c != EOF);

    if (c != EOF) {
        ungetc(c, fp);
    }
    return c;
}

int get_arg(FILE *fp, char *arg)
{
    int c;
    int i;

    eat_white_space(fp);
    i = 0;
    do {
        c = fgetc(fp);
        if (c > ' ') {
            *arg++ = (char)c;
        }

    } while (c > ' ');

    *arg = '\0';
    return c;
}

void process_chardev(FILE *fp, char *arg, char *file_name)
{
    char param[512];
    size_t len;
    size_t i;
    int processed_path;

    get_arg(fp, param);
    if (strstr(param, "id=charmonitor") != NULL) {
        /* stripping this chardev out so just return */
        return;
    } else if (strstr(param, "id=charserial") != NULL) {
        printf("%s ", arg);
        len = strlen(param);
        processed_path = 0;
        i = 0;
        while (i < len) {
            if (processed_path == 0) {
                if (i + 6 < len && strncmp(&param[i], ",path=", 6) == 0) {
                    processed_path = 1;
                    printf(",path=/tmp/%s", file_name);

                    /* skip over the old path */
                    i += 6;
                    while (i < len && param[i] != ',' && param[i] != ' ') {
                        i++;
                    }
                    if (i < len && param[i] == ',') {
                        printf(",");
                    }
                } else if (i < len) {
                    printf("%c", param[i]);
                }
            } else {
                printf("%c", param[i]);
            }
            i++;
        }
        printf(" ");
    } else {
        printf("%s ", arg);
        printf("%s ", param);
    }
}

void process_netdev(FILE *fp, char *arg)
{
    char param[512];
    size_t len;
    size_t i;

    printf("%s ", arg);
    get_arg(fp, param);

    len = strlen(param);
    i = 0;
    while (i < len) {
        if (i + 4 < len && strncmp(&param[i], ",fd=", 4) == 0) {
            printf(",script=/vms/qemu-ifup.br0");
            i += 4;
            while (i < len && param[i] != ',' && param[i] != ' ') {
                i++;
            }
        } else if (i + 8 < len && strncmp(&param[i], ",vhostfd=", 8) == 0) {
            i += 8;
            while (i < len && param[i] != ',' && param[i] != ' ') {
                i++;
            }
        } else {
            printf("%c", param[i]);
            i++;
        }
    }
    printf(" ");
}
void process_device(FILE *fp, char *arg, int no_usb)
{
    char param[512];

    get_arg(fp, param);
    if (no_usb == 1) {
        if (strstr(param, "piix3-usb-uhci") != NULL
                || strstr(param, "usb-tablet") != NULL) {
            return;
        }
    }
    printf("%s ", arg);
    printf("%s ", param);
}

void process_add_fd(FILE *fp, char *arg)
{
    char param[512];

    get_arg(fp, param);
    if (strstr(param, "set=") == NULL) {
        printf("%s ", arg);
        printf("%s ", param);
    }
}

int __CDECL main(int argc, char *argv[])
{
    char arg[512];
    int no_usb;
    int i;
    int c;
    FILE *fp;

    if (argc < 2) {
        printf("usage: %s <file_name> [no-usb]\n", argv[0]);
        return 1;
    }

    no_usb = 0;
    for (i = 2; i < argc; i++) {
        if (strcmp(argv[i], "no-usb") == 0) {
            no_usb = 1;
        }
    }

    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Failed to open file: %s\n", argv[1]);
        return 1;
    }

    do {
        c = get_arg(fp, arg);
        if (c != EOF) {
            if (strcmp(arg, "-S") == 0) {
                /* Remove and replace */
                printf("-monitor stdio ");
            } else if (strcmp(arg, "-object") == 0) {
                /* Remove */
                c = get_arg(fp, arg);
            } else if (strcmp(arg, "-chardev") == 0) {
                /* Handle all chardevs.  Use input file name for serial
                 * output file name. */
                process_chardev(fp, arg, argv[1]);
            } else if (strcmp(arg, "-mon") == 0) {
                /* Remove */
                c = get_arg(fp, arg);
            } else if (strcmp(arg, "-netdev") == 0) {
                /* Fix up netdev */
                process_netdev(fp, arg);
            } else if (strcmp(arg, "-device") == 0) {
                /* Fix up devices */
                process_device(fp, arg, no_usb);
            } else if (strcmp(arg, "-add-fd") == 0) {
                /* Consume the add-fd */
                process_add_fd(fp, arg);
            } else if (strcmp(arg, "-no-shutdown") == 0) {
                /* Do nothing */
            } else if (strcmp(arg, "-no-hpet") == 0) {
                /* Do nothing */
            } else if (strcmp(arg, "\\") == 0) {
                /* Do nothing */
            } else {
                /* Just print it back out */
                printf("%s ", arg);
            }
        }
    } while (c != EOF);
    fclose (fp);
    return 0;
}
