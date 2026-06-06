#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define DEFAULT_SCRAMBLE_SIZE 20

typedef struct cpiece {
  char fbcolor; // front / back color
  char lrcolor; // left / right color
  char udcolor; // up / down color
  uint8_t x; // horizotal position
  uint8_t y; // vertical position
  uint8_t z; // depth
} cpiece;

typedef struct cube {
  cpiece pieces[26];
} cube;

const char WHITE[] = "\x1B[38;5;255m██\x1B[38;5;15m";
const char YELLOW[] = "\x1B[38;5;226m██\x1B[38;5;15m";
const char BLUE[] = "\x1B[38;5;33m██\x1B[38;5;15m";
const char RED[] = "\x1B[38;5;196m██\x1B[38;5;15m";
const char GREEN[] = "\x1B[38;5;46m██\x1B[38;5;15m";
const char ORANGE[] = "\x1B[38;5;208m██\x1B[38;5;15m";

int scramble_size = DEFAULT_SCRAMBLE_SIZE;

char * get_scramble(int size){
  char * scramble_algorithm = calloc(size, sizeof(char) * 3);
  char sides[] = {'F', 'U', 'L', 'R', 'D', 'B'};
  char modif[] = {2, '\'', '2'};

  for(int i = 0; i < size * 3; i += 3){
    int side = rand() % sizeof(sides);
    int mod = rand() % sizeof(modif);

    if(scramble_algorithm[i - 3] == sides[side]){
      i -= 3;
      continue;
    }

    scramble_algorithm[i] = sides[side];
    scramble_algorithm[i + 1] = modif[mod];
    scramble_algorithm[i + 2] = ' ';
  }
  return scramble_algorithm;
}

cube * init_cube() {
  cube * c = calloc(1, sizeof(cube));

  uint8_t positions_x[] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2};
  uint8_t positions_y[] = {0, 0, 0, 1, 1, 1, 2, 2, 2, 0, 0, 0, 1, 1, 2, 2, 2, 0, 0, 0, 1, 1, 1, 2, 2, 2};
  uint8_t positions_z[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2};
  char colors_fb[] = {'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B'};
  char colors_lr[] = {'O', ' ', 'R', 'O', ' ', 'R', 'O', ' ', 'R', 'O', ' ', 'R', 'O', 'R', 'O', ' ', 'R', 'O', ' ', 'R', 'O', ' ', 'R', 'O', ' ', 'R'};
  char colors_ud[] = {'W', 'W', 'W', ' ', ' ', ' ', 'Y', 'Y', 'Y', 'W', 'W', 'W', ' ', ' ', 'Y', 'Y', 'Y', 'W', 'W', 'W', ' ', ' ', ' ', 'Y', 'Y', 'Y'};

  for(int i = 0; i < sizeof(c->pieces) / sizeof(cpiece); i++){
    c->pieces[i].x = positions_x[i];
    c->pieces[i].y = positions_y[i];
    c->pieces[i].z = positions_z[i];
    c->pieces[i].fbcolor = colors_fb[i];
    c->pieces[i].lrcolor = colors_lr[i];
    c->pieces[i].udcolor = colors_ud[i];
  }

  return c;
}

const char * color_decode(char c){
  switch(c){
    case 'W': return WHITE;
    case 'Y': return YELLOW;
    case 'B': return BLUE;
    case 'R': return RED;
    case 'G': return GREEN;
    case 'O': return ORANGE;
  }
}

void print_cube(cube * c){
  // position of pieces in order
  uint8_t positions_x[][9] = {{0, 1, 2, 0, 1, 2, 0, 1, 2}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 2, 0, 1, 2, 0, 1, 2}, {2, 2, 2, 2, 2, 2, 2, 2, 2}, {2, 1, 0, 2, 1, 0, 2, 1, 0}, {0, 1, 2, 0, 1, 2, 0, 1, 2}};
  uint8_t positions_y[][9] = {{0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 1, 2, 2, 2}, {0, 0, 0, 1, 1, 1, 2, 2, 2}, {0, 0, 0, 1, 1, 1, 2, 2, 2}, {0, 0, 0, 1, 1, 1, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2, 2}};
  uint8_t positions_z[][9] = {{2, 2, 2, 1, 1, 1, 0, 0, 0}, {2, 1, 0, 2, 1, 0, 2, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 2, 0, 1, 2, 0, 1, 2}, {2, 2, 2, 2, 2, 2, 2, 2, 2}, {0, 0, 0, 1, 1, 1, 2, 2, 2}};

  // find pieces in the cube according to their position
  cpiece p[6][9] = { 0 };
  for(int i = 0; i < sizeof(p) / sizeof(cpiece) / 9; i++){
    for(int j = 0; j < sizeof(p) / sizeof(cpiece) / 6; j++){
      for(int k = 0; k < sizeof(c->pieces) / sizeof(cpiece); k++){
        if(c->pieces[k].x == positions_x[i][j] &&
           c->pieces[k].y == positions_y[i][j] &&
           c->pieces[k].z == positions_z[i][j])
          p[i][j] = c->pieces[k];
      }
    }
  }

  // print the color of the pieces
  for(int i = 0; i < 9; i += 3){
    printf("       ");
    for(int j = 0; j < 3; j++) printf("%s", color_decode(p[0][i + j].udcolor));
    printf("\n");
  }
  printf("\n");
  for(int i = 0; i < 9; i += 3){
    for(int j = 0; j < 3; j++) printf("%s", color_decode(p[1][i + j].lrcolor));
    printf(" ");
    for(int j = 0; j < 3; j++) printf("%s", color_decode(p[2][i + j].fbcolor));
    printf(" ");
    for(int j = 0; j < 3; j++) printf("%s", color_decode(p[3][i + j].lrcolor));
    printf(" ");
    for(int j = 0; j < 3; j++) printf("%s", color_decode(p[4][i + j].fbcolor));
    printf("\n");
  }
  printf("\n");
  for(int i = 0; i < 9; i += 3){
    printf("       ");
    for(int j = 0; j < 3; j++) printf("%s", color_decode(p[5][i + j].udcolor));
    printf("\n");
  }
}

int get_index_from_pos(cube * c, uint8_t x, uint8_t y, uint8_t z){
  for(int i = 0; i < sizeof(c->pieces) / sizeof(cpiece); i++){
    if(c->pieces[i].x == x && c->pieces[i].y == y && c->pieces[i].z == z)
      return i;
  }
}

void fbmove(cube * c, uint8_t z, bool clockwise){
  uint8_t corners_to_switch[][2] = {{0, 0}, {2, 0}, {2, 2}, {0, 2}}; // [0] = x, [1] = y
  uint8_t centers_to_switch[][2] = {{1, 0}, {2, 1}, {1, 2}, {0, 1}};

  cpiece tmp[8] = { 0 };
  int tmpc = 0;

  if(clockwise){
    for(int i = 0; i < 4; i++){
      int sec_index = (i + 1) + ((i + 1) % 4) - (i + 1);
      int cor_index = get_index_from_pos(c, corners_to_switch[i][0], corners_to_switch[i][1], z);
      int cer_index = get_index_from_pos(c, centers_to_switch[i][0], centers_to_switch[i][1], z);

      // rotate corners
      tmp[tmpc].x = corners_to_switch[sec_index][0];
      tmp[tmpc].y = corners_to_switch[sec_index][1];
      tmp[tmpc].z = z;
      tmp[tmpc].fbcolor = c->pieces[cor_index].fbcolor;
      tmp[tmpc].lrcolor = c->pieces[cor_index].udcolor;
      tmp[tmpc].udcolor = c->pieces[cor_index].lrcolor;
      tmpc++;

      // rotate centers
      tmp[tmpc].x = centers_to_switch[sec_index][0];
      tmp[tmpc].y = centers_to_switch[sec_index][1];
      tmp[tmpc].z = z;
      tmp[tmpc].fbcolor = c->pieces[cer_index].fbcolor;
      tmp[tmpc].lrcolor = c->pieces[cer_index].udcolor;
      tmp[tmpc].udcolor = c->pieces[cer_index].lrcolor;
      tmpc++;
    }
  }else{
    for(int i = 3; i >= 0; i--){
      int sec_index = i-1;
      if(sec_index < 0) sec_index = 3;
      int cor_index = get_index_from_pos(c, corners_to_switch[i][0], corners_to_switch[i][1], z);
      int cer_index = get_index_from_pos(c, centers_to_switch[i][0], centers_to_switch[i][1], z);

      // rotate corners
      tmp[tmpc].x = corners_to_switch[sec_index][0];
      tmp[tmpc].y = corners_to_switch[sec_index][1];
      tmp[tmpc].z = z;
      tmp[tmpc].fbcolor = c->pieces[cor_index].fbcolor;
      tmp[tmpc].lrcolor = c->pieces[cor_index].udcolor;
      tmp[tmpc].udcolor = c->pieces[cor_index].lrcolor;
      tmpc++;

      // rotate centers
      tmp[tmpc].x = centers_to_switch[sec_index][0];
      tmp[tmpc].y = centers_to_switch[sec_index][1];
      tmp[tmpc].z = z;
      tmp[tmpc].fbcolor = c->pieces[cer_index].fbcolor;
      tmp[tmpc].lrcolor = c->pieces[cer_index].udcolor;
      tmp[tmpc].udcolor = c->pieces[cer_index].lrcolor;
      tmpc++;
    }
  }

  for(int i = 0; i < 8; i++)
    c->pieces[get_index_from_pos(c, tmp[i].x, tmp[i].y, tmp[i].z)] = tmp[i];

}

void lrmove(cube * c, uint8_t x, bool clockwise){
  uint8_t corners_to_switch[][2] = {{0, 2}, {0, 0}, {2, 0}, {2, 2}}; // [0] = y, [1] = z
  uint8_t centers_to_switch[][2] = {{0, 1}, {1, 0}, {2, 1}, {1, 2}};

  cpiece tmp[8] = { 0 };
  int tmpc = 0;

  if(clockwise){
    for(int i = 0; i < 4; i++){
      int sec_index = (i + 1) + ((i + 1) % 4) - (i + 1);
      int cor_index = get_index_from_pos(c, x, corners_to_switch[i][0], corners_to_switch[i][1]);
      int cer_index = get_index_from_pos(c, x, centers_to_switch[i][0], centers_to_switch[i][1]);

      // rotate corners
      tmp[tmpc].x = x;
      tmp[tmpc].y = corners_to_switch[sec_index][0];
      tmp[tmpc].z = corners_to_switch[sec_index][1];
      tmp[tmpc].fbcolor = c->pieces[cor_index].udcolor;
      tmp[tmpc].lrcolor = c->pieces[cor_index].lrcolor;
      tmp[tmpc].udcolor = c->pieces[cor_index].fbcolor;
      tmpc++;

      // rotate centers
      tmp[tmpc].x = x;
      tmp[tmpc].y = centers_to_switch[sec_index][0];
      tmp[tmpc].z = centers_to_switch[sec_index][1];
      tmp[tmpc].fbcolor = c->pieces[cer_index].udcolor;
      tmp[tmpc].lrcolor = c->pieces[cer_index].lrcolor;
      tmp[tmpc].udcolor = c->pieces[cer_index].fbcolor;
      tmpc++;
    }
  }else{
    for(int i = 3; i >= 0; i--){
      int sec_index = i-1;
      if(sec_index < 0) sec_index = 3;
      int cor_index = get_index_from_pos(c, x, corners_to_switch[i][0], corners_to_switch[i][1]);
      int cer_index = get_index_from_pos(c, x, centers_to_switch[i][0], centers_to_switch[i][1]);

      // rotate corners
      tmp[tmpc].x = x;
      tmp[tmpc].y = corners_to_switch[sec_index][0];
      tmp[tmpc].z = corners_to_switch[sec_index][1];
      tmp[tmpc].fbcolor = c->pieces[cor_index].udcolor;
      tmp[tmpc].lrcolor = c->pieces[cor_index].lrcolor;
      tmp[tmpc].udcolor = c->pieces[cor_index].fbcolor;
      tmpc++;

      // rotate centers
      tmp[tmpc].x = x;
      tmp[tmpc].y = centers_to_switch[sec_index][0];
      tmp[tmpc].z = centers_to_switch[sec_index][1];
      tmp[tmpc].fbcolor = c->pieces[cer_index].udcolor;
      tmp[tmpc].lrcolor = c->pieces[cer_index].lrcolor;
      tmp[tmpc].udcolor = c->pieces[cer_index].fbcolor;
      tmpc++;
    }
  }

  for(int i = 0; i < 8; i++)
    c->pieces[get_index_from_pos(c, tmp[i].x, tmp[i].y, tmp[i].z)] = tmp[i];
}

void udmove(cube * c, uint8_t y, bool clockwise){
  uint8_t corners_to_switch[][2] = {{0, 2}, {2, 2}, {2, 0}, {0, 0}}; // [0] = x, [1] = z
  uint8_t centers_to_switch[][2] = {{1, 2}, {2, 1}, {1, 0}, {0, 1}};

  cpiece tmp[8] = { 0 };
  int tmpc = 0;

  if(clockwise){
    for(int i = 0; i < 4; i++){
      int sec_index = (i + 1) + ((i + 1) % 4) - (i + 1);
      int cor_index = get_index_from_pos(c, corners_to_switch[i][0], y, corners_to_switch[i][1]);
      int cer_index = get_index_from_pos(c, centers_to_switch[i][0], y, centers_to_switch[i][1]);

      // rotate corners
      tmp[tmpc].x = corners_to_switch[sec_index][0];
      tmp[tmpc].y = y;
      tmp[tmpc].z = corners_to_switch[sec_index][1];
      tmp[tmpc].fbcolor = c->pieces[cor_index].lrcolor;
      tmp[tmpc].lrcolor = c->pieces[cor_index].fbcolor;
      tmp[tmpc].udcolor = c->pieces[cor_index].udcolor;
      tmpc++;

      // rotate centers
      tmp[tmpc].x = centers_to_switch[sec_index][0];
      tmp[tmpc].y = y;
      tmp[tmpc].z = centers_to_switch[sec_index][1];
      tmp[tmpc].fbcolor = c->pieces[cer_index].lrcolor;
      tmp[tmpc].lrcolor = c->pieces[cer_index].fbcolor;
      tmp[tmpc].udcolor = c->pieces[cer_index].udcolor;
      tmpc++;
    }
  }else{
    for(int i = 3; i >= 0; i--){
      int sec_index = i-1;
      if(sec_index < 0) sec_index = 3;
      int cor_index = get_index_from_pos(c, corners_to_switch[i][0], y, corners_to_switch[i][1]);
      int cer_index = get_index_from_pos(c, centers_to_switch[i][0], y, centers_to_switch[i][1]);

      // rotate corners
      tmp[tmpc].x = corners_to_switch[sec_index][0];
      tmp[tmpc].y = y;
      tmp[tmpc].z = corners_to_switch[sec_index][1];
      tmp[tmpc].fbcolor = c->pieces[cor_index].lrcolor;
      tmp[tmpc].lrcolor = c->pieces[cor_index].fbcolor;
      tmp[tmpc].udcolor = c->pieces[cor_index].udcolor;
      tmpc++;

      // rotate centers
      tmp[tmpc].x = centers_to_switch[sec_index][0];
      tmp[tmpc].y = y;
      tmp[tmpc].z = centers_to_switch[sec_index][1];
      tmp[tmpc].fbcolor = c->pieces[cer_index].lrcolor;
      tmp[tmpc].lrcolor = c->pieces[cer_index].fbcolor;
      tmp[tmpc].udcolor = c->pieces[cer_index].udcolor;
      tmpc++;
    }
  }

  for(int i = 0; i < 8; i++)
    c->pieces[get_index_from_pos(c, tmp[i].x, tmp[i].y, tmp[i].z)] = tmp[i];
}

void parse_scramble_alorithm(cube * c, char * scramble_algorithm){
  for(int i = 0; i < scramble_size * 3; i += 3){
    bool clockwise = 1;
    uint8_t repeat = 1;
    uint8_t layer = 0;

    if(*(scramble_algorithm + i + 1) == '\'') clockwise = !clockwise;
    else if(*(scramble_algorithm + i + 1) == '2') repeat = 2;

    if(*(scramble_algorithm + i) == 'B' || *(scramble_algorithm + i) == 'R' || *(scramble_algorithm + i) == 'D'){
      clockwise = !clockwise;
      layer = 2;
    }

    for(int j = 0; j < repeat; j++){
      switch(*(scramble_algorithm + i)){
        case 'F':
        case 'B':
          fbmove(c, layer, clockwise);
          break;
        case 'L':
        case 'R':
          lrmove(c, layer, clockwise);
          break;
        case 'U':
        case 'D':
          udmove(c, layer, clockwise);
          break;
      }
    }
  }
}

int main(int argc, char * argv[]){
  if(argc == 2){
    int pos = 0;
    while(argv[1][pos] != '\0'){
      if((argv[1][pos] == '0' && pos == 0) || (argv[1][pos] < '0' || argv[1][pos] > '9')){
        printf("Invalid scramble length argument!\n");
        return 1;
      }
      pos++;
    }

    scramble_size = 0;
    for(int i = --pos; i >= 0; i--){
      scramble_size += (argv[1][i] - '0') * pow(10, pos - i);
    }
  }

  srand(time(NULL));

  char * scramble_algorithm = get_scramble(scramble_size);
  printf("%s\n", scramble_algorithm);

  cube * c3x3 = init_cube();

  parse_scramble_alorithm(c3x3, scramble_algorithm);

  print_cube(c3x3);

  return 0;
}
