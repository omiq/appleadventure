/*
 * Apple II adventure test
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>


/* Directions */
typedef enum {
    North,
    South,
    East,
    West,
} Direction_t;

/* Items */
typedef enum {
    NoItem,
    Key,
} Item_t;

/* Locations */
typedef enum {
  Nowhere,
  Stasis,
  Observation,
  Corridor,
} Location_t;

/* Names of directions */
char *DescriptionOfDirection[] = {
    "north", "south", "east", "west",
};

/* Names of items */
char *DescriptionOfItem[2] = {
    "",
    "Key",
};

/* Names of locations */
char *DescriptionOfLocation[4] = {
    "",
    "Stasis Room",
    "Observation Room",
    "Corridor",
};

/* DATA */

/* Inventory of what player is carrying */
Item_t Inventory[1];

/* Location of each item. Index is the item number, returns the location. 0 if item is gone */
Location_t locationOfItem[2];

/* Map. Given a location and a direction to move, returns the location it connects to, or 0 if not a valid move. Map can change during game play. */
Direction_t Move[4][4] = {
    /* N  S  E  W  */
    {  0, 0, 0, 0, }, /* 0 */
    {  3, 0, 0, 2, }, /* 1 */
    {  0, 0, 1, 0, }, /* 2 */
    {  0, 1, 0, 0, }, /* 3 */
};

/* Current location */
int currentLocation;

/* Number of turns played in game */
int turnsPlayed;

/* Set when game is over */
int gameOver;

const char *introText = "    Test Apple Adventure in C\nBy Chris Garrett 2020\n\n";
const char *helpString = "Valid commands:\ngo east/west/north/south \nlook\nuse <object>\nexamine <object>\ntake <object>\ndrop <object>\ninventory\nhelp\nquit\nYou can abbreviate commands and\ndirections to the first letter.\nType just the first letter of\na direction to move.\n";

/* Line of user input */
char buffer[40];

/* Clear the screen */
void clearScreen()
{
    clrscr();
}

/* Return 1 if carrying an item */
int carryingItem(char *item)
{
    int i;

    for (i = 0; i < 1; i++) {
        if ((Inventory[i] != 0) && (!strcasecmp(DescriptionOfItem[Inventory[i]], item)))
            return 1;
    }
    return 0;
}

/* Return 1 if item it at current location (not carried) */
int itemIsHere(char *item)
{
    int i;

    /* Find number of the item. */
    for (i = 1; i <= 1; i++) {
        if (!strcasecmp(item, DescriptionOfItem[i])) {
            /* Found it, but is it here? */
            if (locationOfItem[i] == currentLocation) {
                return 1;
            } else {
                return 0;
            }
        }
    }
    return 0;
}

/* Inventory command */
void doInventory()
{
    int i;
    int found = 0;

    printf("%s", "You are carrying:\n");
    for (i = 0; i < 1; i++) {
        if (Inventory[i] != 0) {
            printf("  %s\n", DescriptionOfItem[Inventory[i]]);
            found = 1;
        }
    }
    if (!found)
        printf("  nothing\n");
}

/* Help command */
void doHelp()
{
    printf("%s", helpString);
}

/* Look command */
void doLook()
{
    int i, loc, seen;

    printf("You are %s.\n", DescriptionOfLocation[currentLocation]);

    seen = 0;
    printf("You see:\n");
    for (i = 1; i <= 1; i++) {
        if (locationOfItem[i] == currentLocation) {
            printf("  %s\n", DescriptionOfItem[i]);
            seen = 1;
        }
    }
    if (!seen)
        printf("  nothing special\n");

    printf("You can go:");

    for (i = North; i <= West; i++) {
        loc = Move[currentLocation][i];
        if (loc != 0) {
            printf(" %s", DescriptionOfDirection[i]);
        }
    }
    printf("\n");
}

/* Quit command */
void doQuit()
{
    printf("%s", "Are you sure you want to quit (y/n)? ");
    fgets(buffer, sizeof(buffer)-1, stdin);
    if (tolower(buffer[0]) == 'y') {
        gameOver = 1;
    }
}

/* Drop command */
void doDrop()
{
    int i;
    char *sp;
    char *item;

    /* Command line should be like "D[ROP] ITEM" Item name will be after after first space. */
    sp = strchr(buffer, ' ');
    if (sp == NULL) {
        printf("Drop what?\n");
        return;
    }

    item = sp + 1;

    /* See if we have this item */
    for (i = 0; i < 1; i++) {
        if ((Inventory[i] != 0) && (!strcasecmp(DescriptionOfItem[Inventory[i]], item))) {
            /* We have it. Add to location. */
            locationOfItem[Inventory[i]] = currentLocation;
            /* And remove from inventory */
            Inventory[i] = 0;
            printf("Dropped %s.\n", item);
            ++turnsPlayed;
            return;
        }
    }
    /* If here, don't have it. */
    printf("Not carrying %s.\n", item);
}

/* Take command */
void doTake()
{
    int i, j;
    char *sp;
    char *item;

    /* Command line should be like "T[AKE] ITEM" Item name will be after after first space. */
    sp = strchr(buffer, ' ');
    if (sp == NULL) {
        printf("Take what?\n");
        return;
    }

    item = sp + 1;

    if (carryingItem(item)) {
        printf("Already carrying it.\n");
        return;
    }

    /* Find number of the item. */
    for (i = 1; i <= 1; i++) {
        if (!strcasecmp(item, DescriptionOfItem[i])) {
            /* Found it, but is it here? */
            if (locationOfItem[i] == currentLocation) {
            /* It is here. Add to inventory. */
            for (j = 0; j < 1; j++) {
                if (Inventory[j] == 0) {
                    Inventory[j] = i;
                    /* And remove from location. */
                    locationOfItem[i] = 0;
                    printf("Took %s.\n", item);
                    ++turnsPlayed;
                    return;
                }
            }

            /* Reached maximum number of items to carry */
            printf("You can't carry any more. Drop something.\n");
            return;
            }
        }
    }

    /* If here, don't see it. */
    printf("I see no %s here.\n", item);
}

/* Go command */
void doGo()
{
    char *sp;
    char dirChar;
    Direction_t dir;

    /* Command line should be like "G[O] N[ORTH]" Direction will be
       the first letter after a space. Or just a single letter
       direction N S E W U D or full directon NORTH etc. */

    sp = strrchr(buffer, ' ');
    if (sp != NULL) {
        dirChar = *(sp+1);
    } else {
        dirChar = buffer[0];
    }
    dirChar = tolower(dirChar);

    if (dirChar == 'n') {
        dir = North;
    } else if (dirChar == 's') {
        dir = South;
    } else if (dirChar == 'e') {
        dir = East;
    } else if (dirChar == 'w') {
        dir = West;
    } else {
        printf("Go where?\n");
        return;
    }

    if (Move[currentLocation][dir] == 0) {
        printf("You can't go %s from here.\n", DescriptionOfDirection[dir]);
        return;
    }

    /* We can move */
    currentLocation = Move[currentLocation][dir];
    printf("You are %s.\n", DescriptionOfLocation[currentLocation]);
    ++turnsPlayed;
}

/* Examine command */
void doExamine()
{
    char *sp;
    char *item;

    /* Command line should be like "E[XAMINE] ITEM" Item name will be after after first space. */
    sp = strchr(buffer, ' ');
    if (sp == NULL) {
        printf("Examine what?\n");
        return;
    }

    item = sp + 1;
    ++turnsPlayed;

  
    /* Make sure item is being carried or is in the current location */
    if (!carryingItem(item) && !itemIsHere(item)) {
        printf("I don't see it here.\n");
        return;
    }

 
   /* Nothing special about this item */
   printf("You see nothing special about it.\n");
}

/* Use command */
void doUse()
{
    char *sp;
    char *item;

    /* Command line should be like "U[SE] ITEM" Item name will be after after first space. */
    sp = strchr(buffer, ' ');
    if (sp == NULL) {
        printf("Use what?\n");
        return;
    }

    item = sp + 1;

    /* Make sure item is being carried or is in the current location */
    if (!carryingItem(item) && !itemIsHere(item)) {
        printf("I don't see it here.\n");
        return;
    }

    ++turnsPlayed;

    /* Use key */
    if (!strcasecmp(item, "key") && (currentLocation == Stasis)) {
        printf("You insert the key in the door and it\nopens, revealing a corridor.\n");
        currentLocation = Corridor;
        return;
    }


    /* Default */
    printf("Nothing happens\n");
}

/* Prompt user and get a line of input */
void prompt()
{
    printf("? ");
    fgets(buffer, sizeof(buffer)-1, stdin);

    /* Remove trailing newline */
    buffer[strlen(buffer)-1] = '\0';
}

/* Do special things unrelated to command typed. */
void doActions()
{
   
    /* wolfState values:  0 - wolf attacking 1 - wolf gone, Matthew in tree. 2 - Matthew safe, you won. Game over. */
    if (currentLocation == Corridor) {
        
      printf("Congratulations! You succeeded and won\nthe game.\n");
      gameOver = 1;
      return;

    }
}

/* Set variables to values for start of game */
void initialize()
{
    currentLocation = Stasis;
    turnsPlayed = 0;
    gameOver= 0;

    /* These doors can get changed during game and may need to be reset O*/
    // Move[17][North] = 0;


    /* Set inventory to default */
    memset(Inventory, 0, sizeof(Inventory[0])*1);

    /* Put items in their default locations */
    locationOfItem[0]  = 0;                /* NoItem */
    locationOfItem[1]  = Observation;        /* Key */
}

/* Main program (obviously) */
int main(void)
{
    while (1) {
        initialize();
        clearScreen();
        printf("%s", introText);

        while (!gameOver) {
            prompt();
            if (buffer[0] == '\0') {
            } else if (tolower(buffer[0]) == 'h') {
                doHelp();
            } else if (tolower(buffer[0]) == 'i') {
                doInventory();
            } else if ((tolower(buffer[0]) == 'g')
                       || !strcasecmp(buffer, "n") || !strcasecmp(buffer, "s")
                       || !strcasecmp(buffer, "e") || !strcasecmp(buffer, "w")
                       || !strcasecmp(buffer, "north") || !strcasecmp(buffer, "south")
                       || !strcasecmp(buffer, "east") || !strcasecmp(buffer, "west"))
	    {
                doGo();
            } else if (tolower(buffer[0]) == 'l') {
                doLook();
            } else if (tolower(buffer[0]) == 't') {
                doTake();
            } else if (tolower(buffer[0]) == 'e') {
                doExamine();
            } else if (tolower(buffer[0]) == 'u') {
                doUse();
            } else if (tolower(buffer[0]) == 'd') {
                doDrop();
            } else if (tolower(buffer[0]) == 'q') {
                doQuit();
            } else if (!strcasecmp(buffer, "xyzzy")) {
                printf("Nice try, but that won't work here.\n");
            } else {
                printf("I don't understand. Try 'help'.\n");
            }

            /* Handle special actions. */
            doActions();
        }

        printf("Game over after %d turns.\n", turnsPlayed);
        printf("%s", "Do you want to play again (y/n)? ");
        fgets(buffer, sizeof(buffer)-1, stdin);
        if (tolower(buffer[0]) == 'n') {
            break;
        }
    }
    return 0;
}

