// Copyright Descultu Cristian Petrisor cristicrisx@gmail.com
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define N 256
#define null_value 0
typedef struct {
    char name[N];
    int x, y, hp, stamina, dmg, id, score;
} PLAYERS;

typedef struct {
    int hight, gloves, id;
} SURFACE;

// function to read the height of the surface and what gloves we find in
// each cell
SURFACE** read_surface(int radius, FILE *fin) {
    SURFACE **tmpsurface = (SURFACE **)calloc(2 * radius + 1,
    sizeof(SURFACE *));
    for (int i = 0 ; i < 2 * radius + 1; ++i) {
        tmpsurface[i] = (SURFACE *)calloc(2 * radius + 1, sizeof(SURFACE));
    }

    for (int i = 0; i < 2 * radius + 1; ++i) {
        for (int j = 0; j < 2 * radius + 1; ++j) {
            fscanf(fin, "%d %d", &tmpsurface[i][j].hight,
            &tmpsurface[i][j].gloves);
            tmpsurface[i][j].id = -1;
        }
    }

    return tmpsurface;
}
// function to read players and printing if one of them missed the glacier
PLAYERS *read_players(int *players, int init_players, FILE *fin, FILE *fout,
    int radius, SURFACE **surface) {
    PLAYERS *tmplayers = (PLAYERS *)malloc(init_players * sizeof(PLAYERS));
    for (int i = 0; i < init_players; ++i) {
        tmplayers[i].id = i;
        tmplayers[i].score = 0;
        fscanf(fin, "%s %d %d %d %d", tmplayers[i].name, &tmplayers[i].x,
        &tmplayers[i].y, &tmplayers[i].hp, &tmplayers[i].stamina);
        if (pow(tmplayers[i].x - radius, 2) + pow(tmplayers[i].y - radius, 2) >
            radius * radius) {
            fprintf(fout, "%s has missed the glacier.\n", tmplayers[i].name);
            (*players)--;
            tmplayers[i].hp = 0;
        } else {
            surface[tmplayers[i].x][tmplayers[i].y].id = i;
            tmplayers[i].dmg = surface[tmplayers[i].x][tmplayers[i].y].gloves;
            surface[tmplayers[i].x][tmplayers[i].y].gloves = 0;
        }
    }
    return tmplayers;
}
// function for fighting; the one with higher stamina strikes first and the
// second player's hp decreses by dmg he took. Then same thing it happens when
// the latter strikes.
void fight(SURFACE **surface, PLAYERS *v_players, int id1, int id2, FILE* fout,
    int *players) {
    if (v_players[id1].stamina >= v_players[id2].stamina) {
        while (N) {
        v_players[id2].hp -= v_players[id1].dmg;
        if (v_players[id2].hp <= 0) {
            fprintf(fout, "%s sent %s back home.\n", v_players[id1].name,
            v_players[id2].name);
            v_players[id1].score++;
            v_players[id2].hp = 0;
            (*players)--;
            v_players[id1].stamina += v_players[id2].stamina;
            surface[v_players[id1].x][v_players[id1].y].id = id1;
            return;
        }
        v_players[id1].hp -= v_players[id2].dmg;
        if (v_players[id1].hp <= 0) {
            fprintf(fout, "%s sent %s back home.\n", v_players[id2].name,
            v_players[id1].name);
            v_players[id1].hp = 0;
            v_players[id2].score++;
            (*players)--;
            surface[v_players[id2].x][v_players[id2].y].id = id2;
            v_players[id2].stamina += v_players[id1].stamina;
            return;
        }
       }
    }
    if (v_players[id1].stamina < v_players[id2].stamina) {
       while (1) {
        v_players[id1].hp -= v_players[id2].dmg;
        if (v_players[id1].hp <= 0) {
            fprintf(fout, "%s sent %s back home.\n", v_players[id2].name,
            v_players[id1].name);
            v_players[id1].hp = 0;
            v_players[id2].score++;
            (*players)--;
            v_players[id2].stamina += v_players[id1].stamina;
            surface[v_players[id2].x][v_players[id2].y].id = id2;
            return;
        }
        v_players[id2].hp -= v_players[id1].dmg;
        if (v_players[id2].hp <= 0) {
            fprintf(fout, "%s sent %s back home.\n", v_players[id1].name,
            v_players[id2].name);
            v_players[id2].hp = 0;
            (*players)--;
            v_players[id1].score++;
            v_players[id1].stamina += v_players[id2].stamina;
            surface[v_players[id1].x][v_players[id1].y].id = id1;
            return;
        }
       }
    }
}
// if a player finds better gloves in a cell, he drops his and picks the better
// ones. This function is called in moving function, happening right after his
// movement
void change_gloves(SURFACE **surface, PLAYERS *v_players, int id) {
    if (surface[v_players[id].x][v_players[id].y].gloves >
        v_players[id].dmg) {
        int aux = v_players[id].dmg;
        v_players[id].dmg =
        surface[v_players[id].x][v_players[id].y].gloves;
        surface[v_players[id].x][v_players[id].y].gloves = aux;
        }
}
// prints specified message if a player falls off the glacier due his reckless
// movement. Called right after moving
void falling_off_glacier(PLAYERS *v_players, int *players, FILE *fout, int id) {
        fprintf(fout, "%s fell off the glacier.\n",
        v_players[id].name);
        v_players[id].hp = 0;
        (*players)--;
}
// moves player to the right
void moving_RIGHT(SURFACE **surface, PLAYERS *v_players, int id, int R,
    FILE *fout, int *players) {
    int dif;
    dif =  abs(surface[v_players[id].x][v_players[id].y].hight -
            surface[v_players[id].x][v_players[id].y + 1].hight);
            if (v_players[id].stamina >= dif) {
                v_players[id].y++;
                v_players[id].stamina -= dif;
                if (pow(v_players[id].x - R, 2) +
                    pow(v_players[id].y - R, 2) > R * R) {
                    falling_off_glacier(v_players, players, fout, id);
                } else {
                    change_gloves(surface, v_players, id);
                    if (surface[v_players[id].x][v_players[id].y].id != -1) {
                        int id2 = surface[v_players[id].x][v_players[id].y].id;
                        fight(surface, v_players, id, id2, fout, players);
                    } else {
                        surface[v_players[id].x][v_players[id].y].id = id;
                    }
                    surface[v_players[id].x][v_players[id].y - 1].id = -1;
                }
            }
}
// moves player down
void moving_DOWN(SURFACE **surface, PLAYERS *v_players, int id, int R,
    FILE *fout, int *players) {
    int dif;
    dif =  abs(surface[v_players[id].x][v_players[id].y].hight -
        surface[v_players[id].x + 1][v_players[id].y].hight);
        if (v_players[id].stamina >= dif) {
            v_players[id].x++;
            v_players[id].stamina -= dif;
            if (pow(v_players[id].x - R, 2) +
                pow(v_players[id].y - R, 2) > R * R) {
                falling_off_glacier(v_players, players, fout, id);
                } else {
                    change_gloves(surface, v_players, id);
                    if (surface[v_players[id].x][v_players[id].y].id != -1) {
                        int id2 = surface[v_players[id].x][v_players[id].y].id;
                        fight(surface, v_players, id, id2, fout, players);
                    } else {
                        surface[v_players[id].x][v_players[id].y].id = id;
                    }
                    surface[v_players[id].x - 1][v_players[id].y].id = -1;
                }
        }
}
// moves player to the left
void moving_LEFT(SURFACE **surface, PLAYERS *v_players, int id, int R,
    FILE *fout, int *players) {
    int dif;
    dif = abs(surface[v_players[id].x][v_players[id].y].hight -
    surface[v_players[id].x][v_players[id].y - 1].hight);
    if (v_players[id].stamina >= dif) {
        v_players[id].y--;
        v_players[id].stamina -= dif;
        if (pow(v_players[id].x - R, 2) +
            pow(v_players[id].y - R, 2) > R * R) {
            falling_off_glacier(v_players, players, fout, id);
        } else {
            change_gloves(surface, v_players, id);
            if (surface[v_players[id].x][v_players[id].y].id != -1) {
                int id2 = surface[v_players[id].x][v_players[id].y].id;
                fight(surface, v_players, id, id2, fout, players);
            } else {
                surface[v_players[id].x][v_players[id].y].id = id;
                }
            surface[v_players[id].x][v_players[id].y + 1].id = -1;
            }
        }
}
// moves player up
void moving_UP(SURFACE **surface, PLAYERS *v_players, int id, int R,
    FILE *fout, int *players) {
    int dif;
    dif =  abs(surface[v_players[id].x][v_players[id].y].hight -
    surface[v_players[id].x - 1][v_players[id].y].hight);
    if (v_players[id].stamina >= dif) {
        v_players[id].x--;
        v_players[id].stamina -= dif;
        if (pow(v_players[id].x - R, 2) +
            pow(v_players[id].y - R, 2) > R * R) {
            falling_off_glacier(v_players, players, fout, id);
            } else {
                change_gloves(surface, v_players, id);
                if (surface[v_players[id].x][v_players[id].y].id != -1) {
                    int id2 = surface[v_players[id].x][v_players[id].y].id;
                    fight(surface, v_players, id, id2, fout, players);
                } else {
                    surface[v_players[id].x][v_players[id].y].id = id;
                    }
                surface[v_players[id].x + 1][v_players[id].y].id = -1;
                }
            }
}
// Calls all moving functions
void moving_elf(SURFACE **surface, PLAYERS* v_players, int R,
    int id, char move[20], int *players, FILE *fout) {
    int length = strlen(move);
    for (int j = 0; j < length; ++j) {
      if (v_players[id].hp > 0) {
        if (move[j] == 'R' && v_players[id].y != 2 * R) {
            moving_RIGHT(surface, v_players, id, R, fout, players);
         }
        if (move[j] == 'L' && v_players[id].y != 0) {
            moving_LEFT(surface, v_players, id, R, fout, players);
        }
        if (move[j] == 'D' && v_players[id].x != 2 * R) {
            moving_DOWN(surface, v_players, id, R, fout, players);
        }
        if (move[j] == 'U' && v_players[id].x != 0) {
            moving_UP(surface, v_players, id, R, fout, players);
        }
    }
  }
}
// spawns a snowstorm
// K is a 32 bits number which means X, Y, radius, and damage has 8 bits each
// to find X, Y, radius and dmg we start by finding out last 8 bits(X value) and
// then shifting K by 8 bits to the right to remove those bits we found
void snowstorm(SURFACE **surface, PLAYERS *v_players, FILE* fout, FILE* fin,
    int *players, int P) {
    int K, x , y, r, dmg;
    fscanf(fin, "%d", &K);
    x = K & 255;
    K >>= 8;
    y = K & 255;
    K >>= 8;
    r = K & 255;
    K >>= 8;
    dmg = K & 255;
    for (int i = 0; i < P; ++i) {
        if (v_players[i].hp > 0) {
            if (pow(v_players[i].x - x, 2) + pow(v_players[i].y - y,
                2) <= r*r) {
                v_players[i].hp -= dmg;
                if (v_players[i].hp <= 0) {
                    fprintf(fout, "%s was hit by snowstorm.\n",
                    v_players[i].name);
                    v_players[i].hp = 0;
                    (*players)--;
                    surface[v_players[i].x][v_players[i].y].id = -1;
             }
           }
        }
    }
}
// melting down the glacier and checks if a player falls off glacier due to this
// In this case, prints specified message. It creates a temporary surface in
// which i saved players' new coordinates and then send it content to the main
// surface.
SURFACE** meltdown(SURFACE **surface, PLAYERS *v_players, FILE* fout, FILE* fin,
    int *players, int init_players, int *R, int dim) {
    int add_stamina = null_value;
    fscanf(fin, "%d", &add_stamina);
    (*R)--;
    SURFACE **tmpsurface = (SURFACE**)calloc(2 * *R + 1, sizeof(SURFACE*));
    for (int i = 0 ; i < 2 * *R + 1; ++i) {
        tmpsurface[i] = (SURFACE*)calloc(2 * *R + 1, sizeof(SURFACE));
    }
    for (int i = 0; i < init_players; ++i) {
        if (v_players[i].hp > 0) {
            v_players[i].x--;
            v_players[i].y--;
            v_players[i].stamina += add_stamina;
            if (pow(v_players[i].x - *R, 2) + pow(v_players[i].y - *R, 2) >
            *R * *R) {
                fprintf(fout, "%s got wet because of global warming.\n",
                v_players[i].name);
                v_players[i].hp = 0;
                (*players)--;
          }
        }
    }
  for (int i = 0; i < 2 * *R + 1; ++i) {
    for (int j = 0; j < 2 * *R + 1; ++j) {
        memcpy(&tmpsurface[i][j], &surface[i + 1][j + 1], sizeof(SURFACE));
    }
  }
  for (int i = 0; i < dim; ++i) {
    free(surface[i]);
  }
  free(surface);
  return tmpsurface;
}
// prints the scoreboard
void scoreboard(PLAYERS *v_players, int init_players, FILE *fout) {
    int dry[N], wet[N], dry1 = null_value, wet1 = null_value;
    fprintf(fout, "SCOREBOARD:\n");
    // in a vector I saved all players' IDs that are dry and in another one
    // to those who are wet so I can sort them later on easily
    for (int i = 0; i < init_players; ++i) {
        if (v_players[i].hp > 0) {
            dry[dry1] = v_players[i].id;
            dry1++;
        }
        if (v_players[i].hp <= 0) {
            wet[wet1] = v_players[i].id;
            wet1++;
        }
    }
    // sorts players ascendent by their score
    for (int i = 0; i < dry1 - 1; ++i) {
        for (int j = i + 1; j < dry1; ++j) {
            if (v_players[dry[i]].score < v_players[dry[j]].score) {
                int aux = dry[i];
                dry[i] = dry[j];
                dry[j] = aux;
            }
        }
    } // sorts players alphabetically if their score is equal
    for (int i = 0; i < dry1 - 1; ++i) {
        for (int j = i + 1; j < dry1; ++j) {
            if (v_players[dry[i]].score == v_players[dry[j]].score) {
                if (strcmp(v_players[dry[i]].name, v_players[dry[j]].name) >
                0) {
                    int aux = dry[i];
                    dry[i] = dry[j];
                    dry[j] = aux;
               }
            }
        }
    }   // ascendent sort
        for (int i = 0; i < wet1 - 1; ++i) {
        for (int j = i + 1; j < wet1; ++j) {
            if (v_players[wet[i]].score < v_players[wet[j]].score) {
                int aux = wet[i];
                wet[i] = wet[j];
                wet[j] = aux;
            }
        }
    } // alphabetical sort
    for (int i = 0; i < wet1 - 1; ++i) {
        for (int j = i + 1; j < wet1; ++j) {
            if (v_players[wet[i]].score == v_players[wet[j]].score) {
                if (strcmp(v_players[wet[i]].name, v_players[wet[j]].name) >
                0) {
                    int aux = wet[i];
                    wet[i] = wet[j];
                    wet[j] = aux;
               }
            }
        }
    }
    for (int i = 0; i < dry1; ++i) {
        fprintf(fout, "%s\tDRY\t%d\n", v_players[dry[i]].name,
        v_players[dry[i]].score);
    }
    for (int i = 0; i < wet1; ++i) {
        fprintf(fout, "%s\tWET\t%d\n", v_players[wet[i]].name,
        v_players[wet[i]].score);
    }
}
int main() {
    int radius, players, init_players;
    FILE *fin, *fout;
    char command[N];
    int ID_ELF_TO_MOVE;
    char moving_sequence[N];
    fin = fopen("snowfight.in", "rt");
    fout = fopen("snowfight.out", "wt");
    fscanf(fin, "%d %d", &radius, &players);
    init_players = players;
    SURFACE **surface;
    PLAYERS *v_players;
    surface = read_surface(radius, fin);
    v_players = read_players(&players, init_players, fin, fout, radius,
    surface);
    // if the current number of players is 1, this means that there is only 1
    // player left so he wins
    if (players == 1) {
        for (int i = 0; i < init_players; ++i) {
            if (v_players[i].hp > 0) {
                fprintf(fout, "%s has won.\n", v_players[i].name);
                break;
            }
        }
        for (int i = 0; i < 2 * radius + 1; ++i) {
                free(surface[i]);
                }
                free(surface);
                fclose(fin);
                fclose(fout);
                free(v_players);
                return 0;
    }
    // reading line by line from the input file as long as we didn't reach it's
    // end
    while (fscanf(fin, "%s", command) != EOF && players != 1) {
            if (strcmp(command, "MOVE") == 0) {
                fscanf(fin, "%d", &ID_ELF_TO_MOVE);
                fscanf(fin, "%s", moving_sequence);
                moving_elf(surface, v_players, radius, ID_ELF_TO_MOVE,
                moving_sequence, &players, fout);
            }
            if (strcmp(command, "SNOWSTORM") == 0) {
                snowstorm(surface, v_players, fout, fin, &players,
                init_players);
            }
            if (strcmp(command, "MELTDOWN") == 0) {
                int dim2 = 2 * radius + 1;
                surface = meltdown(surface, v_players, fout, fin, &players,
                init_players, &radius, dim2);
            }
            if (strcmp(command, "PRINT_SCOREBOARD") == 0) {
                scoreboard(v_players, init_players, fout);
            }
    }
    if (players == 1) {
        for (int i = 0; i < init_players; ++i) {
            if (v_players[i].hp > 0) {
                fprintf(fout, "%s has won.\n", v_players[i].name);
                break;
            }
        }
    }
    for (int i = 0; i < 2 * radius + 1; ++i) {
        free(surface[i]);
    }
    free(surface);
    fclose(fin);
    fclose(fout);
    free(v_players);
    return 0;
}
