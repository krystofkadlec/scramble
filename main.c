#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// BUILD: gcc vsc.c -o scr -lm

#define DEFAULT_SCRAMBLE_SIZE 20
#define DEFAULT_CUBE_SIZE 3

// global variable is used instead of macro, so that the value can be changed with a parameter
int scramble_size = DEFAULT_SCRAMBLE_SIZE;
uint8_t cube_size = DEFAULT_CUBE_SIZE;

typedef struct cpiece {
  char fbcolor; // front / back color
  char lrcolor; // left / right color
  char udcolor; // up / down color
  int8_t x; // horizotal position
  int8_t y; // vertical position
  int8_t z; // depth
} cpiece;

const char WHITE[] = "\x1B[38;5;255m██\x1B[0m";
const char YELLOW[] = "\x1B[38;5;226m██\x1B[0m";
const char BLUE[] = "\x1B[38;5;33m██\x1B[0m";
const char RED[] = "\x1B[38;5;196m██\x1B[0m";
const char GREEN[] = "\x1B[38;5;46m██\x1B[0m";
const char ORANGE[] = "\x1B[38;5;208m██\x1B[0m";

const char HELP[] = "scramble: scramble [option] [value]\nOptions:\n\t-c\tSets the cube size. Supported sizes are from 2 to 19.\n\t-s\tSets the scramble size. Supported scramble sizes are from 1 up.\n\t-u\tSets custom scramble. Anything thats not a scramble step will be ignored (any separator may be used).\n\t  \tScramble step format:\n\t  \t\t[width][move][w][modifier]\n\n\t  \t\t[width]\t(Optional) Number of layers to turn. Will be ignored if exceeds size of the cube.\n\t  \t\t[move]\t(Required) F, B, L, R, U, D, M, E, S, x, y or z\n\t \t\t[w]\t(Optional) Specifies a wide turn\n\t  \t\t[modifier] (Optional) 2 or \'(prime)\n\n\t  \t\tExamples: R, F2, Dw', 3Lw2\n";

const char * color_decode(char c){
  switch(c){
    case 'W': return WHITE;
    case 'Y': return YELLOW;
    case 'B': return BLUE;
    case 'R': return RED;
    case 'G': return GREEN;
    case 'O': return ORANGE;
    default: return "";
  }
}

void cpiece_init(cpiece * c, int8_t x, int8_t y, int8_t z, char fb, char lr, char ud, int * index){
  c[*index].x = x;
  c[*index].y = y;
  c[*index].z = z;
  c[*index].fbcolor = fb;
  c[*index].lrcolor = lr;
  c[*index].udcolor = ud;
  (*index)++;
}

void cube_init_horizontal_inbetween(cpiece * c, int8_t y, int8_t z, char fb, char lr, char ud, bool even, int * index){
  for(int i = -(cube_size / 2) + 1; i <= (cube_size / 2) - 1; i++){
    if(even && i == 0) continue;
    cpiece_init(c, i, y, z, fb, lr, ud, index);
  }
}

cpiece * cube_init(){
  cpiece * c = calloc(pow(cube_size, 3) - pow(cube_size - 2, 3), sizeof(cpiece));

  bool even = cube_size % 2 == 0;
  int index = 0;

  // front face
  cpiece_init(c, -cube_size / 2, -cube_size / 2, -cube_size / 2, 'G', 'O', 'W', &index);
  cube_init_horizontal_inbetween(c, -cube_size / 2, -cube_size / 2, 'G', ' ', 'W', even, &index);
  cpiece_init(c, cube_size / 2, -cube_size / 2, -cube_size / 2, 'G', 'R', 'W', &index);

  for(int i = -(cube_size / 2) + 1; i <= (cube_size / 2) - 1; i++){
    if(even && i == 0) continue;
    cpiece_init(c, -cube_size / 2, i, -cube_size / 2, 'G', 'O', ' ', &index);
    cube_init_horizontal_inbetween(c, i, -cube_size / 2, 'G', ' ', ' ', even, &index);
    cpiece_init(c, cube_size / 2, i, -cube_size / 2, 'G', 'R', ' ', &index);
  }

  cpiece_init(c, -cube_size / 2, cube_size / 2, -cube_size / 2, 'G', 'O', 'Y', &index);
  cube_init_horizontal_inbetween(c, cube_size / 2, -cube_size / 2, 'G', ' ', 'Y', even, &index);
  cpiece_init(c, cube_size / 2, cube_size / 2, -cube_size / 2, 'G', 'R', 'Y', &index);

  // next faces
  for(int i = -(cube_size / 2) + 1; i <= (cube_size / 2) - 1; i++){
    if(even && i == 0) continue;

    cpiece_init(c, -cube_size / 2, -cube_size / 2, i, ' ', 'O', 'W', &index);
    cube_init_horizontal_inbetween(c, -cube_size / 2, i, ' ', ' ', 'W', even, &index);
    cpiece_init(c, cube_size / 2, -cube_size / 2, i, ' ', 'R', 'W', &index);

    for(int j = -(cube_size / 2) + 1; j <= (cube_size / 2) - 1; j++){
      if(even && j == 0) continue;
      cpiece_init(c, -cube_size / 2, j, i, ' ', 'O', ' ', &index);
      cpiece_init(c, cube_size / 2, j, i, ' ', 'R', ' ', &index);
    }

    cpiece_init(c, -cube_size / 2, cube_size / 2, i, ' ', 'O', 'Y', &index);
    cube_init_horizontal_inbetween(c, cube_size / 2, i, ' ', ' ', 'Y', even, &index);
    cpiece_init(c, cube_size / 2, cube_size / 2, i, ' ', 'R', 'Y', &index);
  }

  // back face
  cpiece_init(c, -cube_size / 2, -cube_size / 2, cube_size / 2, 'B', 'O', 'W', &index);
  cube_init_horizontal_inbetween(c, -cube_size / 2, cube_size / 2, 'B', ' ', 'W', even, &index);
  cpiece_init(c, cube_size / 2, -cube_size / 2, cube_size / 2, 'B', 'R', 'W', &index);

  for(int i = -(cube_size / 2) + 1; i <= (cube_size / 2) - 1; i++){
    if(even && i == 0) continue;
    cpiece_init(c, -cube_size / 2, i, cube_size / 2, 'B', 'O', ' ', &index);
    cube_init_horizontal_inbetween(c, i, cube_size / 2, 'B', ' ', ' ', even, &index);
    cpiece_init(c, cube_size / 2, i, cube_size / 2, 'B', 'R', ' ', &index);
  }

  cpiece_init(c, -cube_size / 2, cube_size / 2, cube_size / 2, 'B', 'O', 'Y', &index);
  cube_init_horizontal_inbetween(c, cube_size / 2, cube_size / 2, 'B', ' ', 'Y', even, &index);
  cpiece_init(c, cube_size / 2, cube_size / 2, cube_size / 2, 'B', 'R', 'Y', &index);

  return c;
}

int get_index_from_pos(cpiece * c, int8_t x, int8_t y, int8_t z){
  for(int i = 0; i < pow(cube_size, 3) - pow(cube_size - 2, 3); i++){
    if(c[i].x == x && c[i].y == y && c[i].z == z) return i;
  }
  return -1;
}

void rotatefb(cpiece * c, int8_t layer){
  cpiece tmp[cube_size * cube_size];

  int index = 0;
  bool even = cube_size % 2 == 0;
  for(int i = -cube_size / 2; i <= cube_size / 2; i++){
    if(even && i == 0) continue;
    for(int j = -cube_size / 2; j <= cube_size / 2; j++)
      if(!(even && j == 0)) cpiece_init(tmp, j, i, layer, ' ', ' ', ' ', &index);
  }

  int cpiece_indexes[cube_size * cube_size];
  for(int i = 0; i < sizeof(cpiece_indexes) / sizeof(int); i++)
    cpiece_indexes[i] = get_index_from_pos(c, tmp[i].x, tmp[i].y, tmp[i].z);

  for(int i = 0; i < cube_size * cube_size; i++){
    float x = cos(90.0 * (M_PI / 180.0)) * c[cpiece_indexes[i]].x - sin(90.0 * (M_PI / 180.0)) * c[cpiece_indexes[i]].y;
    float y = sin(90.0 * (M_PI / 180.0)) * c[cpiece_indexes[i]].x + cos(90.0 * (M_PI / 180.0)) * c[cpiece_indexes[i]].y;

    c[cpiece_indexes[i]].x = x;
    c[cpiece_indexes[i]].y = y;

    char tmp_c = c[cpiece_indexes[i]].lrcolor;
    c[cpiece_indexes[i]].lrcolor = c[cpiece_indexes[i]].udcolor;
    c[cpiece_indexes[i]].udcolor = tmp_c;
  }
}

void rotatelr(cpiece * c, int8_t layer){
  cpiece tmp[cube_size * cube_size];

  int index = 0;
  bool even = cube_size % 2 == 0;
  for(int i = -cube_size / 2; i <= cube_size / 2; i++){
    if(even && i == 0) continue;
    for(int j = cube_size / 2; j >= -cube_size / 2; j--)
      if(!(even && j == 0)) cpiece_init(tmp, layer, i, j, ' ', ' ', ' ', &index);
  }

  int cpiece_indexes[cube_size * cube_size];
  for(int i = 0; i < sizeof(cpiece_indexes) / sizeof(int); i++)
    cpiece_indexes[i] = get_index_from_pos(c, tmp[i].x, tmp[i].y, tmp[i].z);

  for(int i = 0; i < cube_size * cube_size; i++){
    float z = cos(-90.0 * (M_PI / 180.0)) * c[cpiece_indexes[i]].z - sin(-90.0 * (M_PI / 180.0)) * c[cpiece_indexes[i]].y;
    float y = sin(-90.0 * (M_PI / 180.0)) * c[cpiece_indexes[i]].z + cos(-90.0 * (M_PI / 180.0)) * c[cpiece_indexes[i]].y;

    c[cpiece_indexes[i]].z = z;
    c[cpiece_indexes[i]].y = y;

    char tmp_c = c[cpiece_indexes[i]].fbcolor;
    c[cpiece_indexes[i]].fbcolor = c[cpiece_indexes[i]].udcolor;
    c[cpiece_indexes[i]].udcolor = tmp_c;
  }
}

void rotateud(cpiece * c, int8_t layer){
  cpiece tmp[cube_size * cube_size];

  int index = 0;
  bool even = cube_size % 2 == 0;
  for(int i = cube_size / 2; i >= -cube_size / 2; i--){
    if(even && i == 0) continue;
    for(int j = -cube_size / 2; j <= cube_size / 2; j++)
      if(!(even && j == 0)) cpiece_init(tmp, j, layer, i, ' ', ' ', ' ', &index);
  }

  int cpiece_indexes[cube_size * cube_size];
  for(int i = 0; i < sizeof(cpiece_indexes) / sizeof(int); i++)
    cpiece_indexes[i] = get_index_from_pos(c, tmp[i].x, tmp[i].y, tmp[i].z);

  for(int i = 0; i < cube_size * cube_size; i++){
    float x = cos(-90.0 * (M_PI / 180.0)) * c[cpiece_indexes[i]].x - sin(-90.0 * (M_PI / 180.0)) * c[cpiece_indexes[i]].z;
    float z = sin(-90.0 * (M_PI / 180.0)) * c[cpiece_indexes[i]].x + cos(-90.0 * (M_PI / 180.0)) * c[cpiece_indexes[i]].z;

    c[cpiece_indexes[i]].x = x;
    c[cpiece_indexes[i]].z = z;

    char tmp_c = c[cpiece_indexes[i]].fbcolor;
    c[cpiece_indexes[i]].fbcolor = c[cpiece_indexes[i]].lrcolor;
    c[cpiece_indexes[i]].lrcolor = tmp_c;
  }
}

void triangle_pattern(int8_t * arr, bool starting_down){
  int index = 0;
  if(starting_down){
    for(int j = 0; j < cube_size; j++) for(int i = -cube_size / 2; i <= cube_size / 2; i++){
      if(cube_size % 2 == 0 && i == 0) continue;
      arr[index] = i;
      index++;
    }
  }else{
    for(int j = 0; j < cube_size; j++) for(int i = cube_size / 2; i >= -cube_size / 2; i--){
      if(cube_size % 2 == 0 && i == 0) continue;
      arr[index] = i;
      index++;
    }
  }
}

void diagonal_pattern(int8_t * arr, bool going_up){
  int index = 0;
  if(going_up){
    for(int i = -cube_size / 2; i <= cube_size / 2; i++){
      if(cube_size % 2 == 0 && i == 0) continue;
      for(int j = 0; j < cube_size; j++){
        arr[index] = i;
        index++;
      }
    }
  }else{
    for(int i = cube_size / 2; i >= -cube_size / 2; i--){
      if(cube_size % 2 == 0 && i == 0) continue;
      for(int j = 0; j < cube_size; j++){
        arr[index] = i;
        index++;
      }
    }
  }
}

void constant_pattern(int8_t * arr, int constant){
  for(int i = 0; i < cube_size * cube_size; i++) arr[i] = constant;
}

void print_cube(cpiece * c){
  int8_t position_x[6][cube_size * cube_size];
  int8_t position_y[6][cube_size * cube_size];
  int8_t position_z[6][cube_size * cube_size];

  bool even = cube_size % 2 == 0;

  triangle_pattern(position_x[0], true);
  constant_pattern(position_y[0], -cube_size / 2);
  diagonal_pattern(position_z[0], false);

  constant_pattern(position_x[1], -cube_size / 2);
  diagonal_pattern(position_y[1], true);
  triangle_pattern(position_z[1], false);

  triangle_pattern(position_x[2], true);
  diagonal_pattern(position_y[2], true);
  constant_pattern(position_z[2], -cube_size / 2);

  constant_pattern(position_x[3], cube_size / 2);
  diagonal_pattern(position_y[3], true);
  triangle_pattern(position_z[3], true);

  triangle_pattern(position_x[4], false);
  diagonal_pattern(position_y[4], true);
  constant_pattern(position_z[4], cube_size / 2);

  triangle_pattern(position_x[5], true);
  constant_pattern(position_y[5], cube_size / 2);
  diagonal_pattern(position_z[5], true);

  // pieces in correct order for print-out
  cpiece sorted_pieces[6][cube_size * cube_size];
  for(int i = 0; i < 6; i++) for(int j = 0; j < cube_size * cube_size; j++)
    sorted_pieces[i][j] = c[get_index_from_pos(c, position_x[i][j], position_y[i][j], position_z[i][j])];

  // print the color of the pieces
  for(int i = 0; i < cube_size * cube_size; i += cube_size){
    for(int j = 0; j < cube_size; j++) printf("  ");
    printf(" ");
    for(int j = 0; j < cube_size; j++) printf("%s", color_decode(sorted_pieces[0][i + j].udcolor));
    printf("\n");
  }
  printf("\n");
  for(int i = 0; i < cube_size * cube_size; i += cube_size){
    for(int j = 0; j < cube_size; j++) printf("%s", color_decode(sorted_pieces[1][i + j].lrcolor));
    printf(" ");
    for(int j = 0; j < cube_size; j++) printf("%s", color_decode(sorted_pieces[2][i + j].fbcolor));
    printf(" ");
    for(int j = 0; j < cube_size; j++) printf("%s", color_decode(sorted_pieces[3][i + j].lrcolor));
    printf(" ");
    for(int j = 0; j < cube_size; j++) printf("%s", color_decode(sorted_pieces[4][i + j].fbcolor));
    printf("\n");
  }
  printf("\n");
  for(int i = 0; i < cube_size * cube_size; i += cube_size){
    for(int j = 0; j < cube_size; j++) printf("  ");
    printf(" ");
    for(int j = 0; j < cube_size; j++) printf("%s", color_decode(sorted_pieces[5][i + j].udcolor));
    printf("\n");
  }
}

char * get_scramble(int size){
  uint8_t stepsize = 6;
  char * scramble_algorithm = calloc(size, sizeof(char) * stepsize);
  char sides[] = {'F', 'U', 'L', 'R', 'D', 'B'};
  char modif[] = {0, '\'', '2'};

  int index = 0;
  int laststeplen = 0;
  for(int i = 0; i < size; i++){
    int side = rand() % sizeof(sides);
    int mod = rand() % sizeof(modif);

    // make sure sides don't repeat in the scramble
    if(scramble_algorithm[index - laststeplen] == sides[side] || scramble_algorithm[index - laststeplen + 1] == sides[side]){
      i--;
      continue;
    }

    int indexbef = index;
    bool wide = rand() % 101 <= 40;
    if(wide && cube_size >= 6) {
      int width = (rand() % (cube_size / 2 - 1)) + 2;
      if(width != 2){
        char str[3] = { 0 };
        sprintf(str, "%d", width);
        for(int j = 0; j < 3; j++)
          if(str[j] >= '0' && str[j] <= '9') scramble_algorithm[index++] = str[j];
      }
    }
    scramble_algorithm[index++] = sides[side];
    if(wide && cube_size >= 4) scramble_algorithm[index++] = 'w';
    if(modif[mod] != 0) scramble_algorithm[index++] = modif[mod];
    scramble_algorithm[index++] = ' ';
    laststeplen = index - indexbef;
  }

  return scramble_algorithm;
}

bool is_classic_move(char c){
  char sides[] = {'F', 'U', 'L', 'R', 'D', 'B'};
  for(int i = 0; i < sizeof(sides); i++)
    if(c == sides[i]) return true;

  return false;
}

bool is_slice_move(char c){
  char sides[] = {'M', 'E', 'S'};
  for(int i = 0; i < sizeof(sides); i++)
    if(c == sides[i]) return true;

  return false;
}

bool is_axis_move(char c){
  char sides[] = {'x', 'y', 'z'};
  for(int i = 0; i < sizeof(sides); i++)
    if(c == sides[i]) return true;

  return false;
}

void parse_classic_move(cpiece * c, char side, char modif, int layershift){
  switch(side){
    case 'F':
      if(modif == '\''){
        for(int i = 0; i < 3; i++) rotatefb(c, (-cube_size / 2) + layershift);
        break;
      }
      else if(modif == '2')rotatefb(c, (-cube_size / 2) + layershift);
      rotatefb(c, (-cube_size / 2) + layershift);
      break;
    case 'B':
      if(modif == 0){
        for(int i = 0; i < 3; i++) rotatefb(c, (cube_size / 2) - layershift);
        break;
      }
      else if(modif == '2') rotatefb(c, (cube_size / 2) - layershift);
      rotatefb(c, (cube_size / 2) - layershift);
      break;
    case 'L':
      if(modif == '\''){
        for(int i = 0; i < 3; i++) rotatelr(c, (-cube_size / 2) + layershift);
        break;
      }
      else if(modif == '2')rotatelr(c, (-cube_size / 2) + layershift);
      rotatelr(c, (-cube_size / 2) + layershift);
      break;
    case 'R':
      if(modif == 0){
        for(int i = 0; i < 3; i++) rotatelr(c, (cube_size / 2) - layershift);
        break;
      }
      else if(modif == '2') {
        rotatelr(c, (cube_size / 2) - layershift);
      }
      rotatelr(c, (cube_size / 2) - layershift);
      break;
    case 'U':
      if(modif == '\''){
        for(int i = 0; i < 3; i++) rotateud(c, (-cube_size / 2) + layershift);
        break;
      }
      else if(modif == '2')rotateud(c, (-cube_size / 2) + layershift);
      rotateud(c, (-cube_size / 2) + layershift);
      break;
    case 'D':
      if(modif == 0){
        for(int i = 0; i < 3; i++) rotateud(c, (cube_size / 2) - layershift);
        break;
      }
      else if(modif == '2') rotateud(c, (cube_size / 2) - layershift);
      rotateud(c, (cube_size / 2) - layershift);
      break;
  }
}

void parse_slice_move(cpiece * c, char side, char modif){
  int s = -cube_size / 2 + 1;
  int e = cube_size / 2 - 1;
  if(cube_size % 2 != 0){
    s = 0;
    e = 0;
  }

  switch(side){
    case 'M':
      for(int j = s; j <= e; j++){
        rotatelr(c, j);
        if(modif == '2') rotatelr(c, j);
        else if(modif == '\'')
          for(int i = 0; i < 2; i++) rotatelr(c, j);
      }
      break;
    case 'E':
      for(int j = s; j <= e; j++){
        rotateud(c, j);
        if(modif == 0)
          for(int i = 0; i < 2; i++) rotateud(c, j);
        else if(modif == '2') rotateud(c, j);
      }
      break;
    case 'S':
      for(int j = s; j <= e; j++){
        rotatefb(c, j);
        if(modif == '2') rotatefb(c, j);
        else if(modif == '\'')
          for(int i = 0; i < 2; i++) rotatefb(c, j);
      }
      break;
  }
}

void parse_axis_move(cpiece * c, char side, char modif){
  switch(side){
    case 'x':
      for(int j = -cube_size / 2; j <= cube_size / 2; j++){
        rotatelr(c, j);
        if(modif == 0)
          for(int i = 0; i < 2; i++) rotatelr(c, j);
        else if(modif == '2') rotatelr(c, j);
      }
      break;
    case 'y':
      for(int j = -cube_size / 2; j <= cube_size / 2; j++){
        rotateud(c, j);
        if(modif == '2') rotateud(c, j);
        else if(modif == '\'')
          for(int i = 0; i < 2; i++) rotateud(c, j);
      }
      break;
    case 'z':
      for(int j = -cube_size / 2; j <= cube_size / 2; j++){
        rotatefb(c, j);
        if(modif == '2') rotatefb(c, j);
        else if(modif == '\'')
          for(int i = 0; i < 2; i++) rotatefb(c, j);
      }
      break;
  }
}

void parse_scramble(char * s, cpiece * c){
  for(int i = 0; i < strlen(s); i++){
    if(is_classic_move(s[i])){
      char modif = 0;
      if(s[i + 1] == 'w' && (s[i + 2] == '2' || s[i + 2] == '\'')) modif = s[i + 2];
      else if(s[i + 1] == '2' || s[i + 1] == '\'') modif = s[i + 1];
      parse_classic_move(c, s[i], modif, 0);

      if(s[i + 1] == 'w') {
        parse_classic_move(c, s[i], modif, 1);
        char width[] = {s[i - 1], '\0'};
        if(s[i - 1] >= '0' && s[i - 1] <= '9' && atoi(width) <= cube_size){
          printf("%c", s[i - 1]);
          for(int j = 2; j < (s[i - 1] - '0'); j++)
            parse_classic_move(c, s[i], modif, j);
        }
      }

      printf("%c", s[i]);
      if(s[i + 1] == 'w') printf("w");
      if(modif == 0) modif = ' ';
      printf("%c%c", modif, modif == ' ' ? '\0' : ' ');
    }else if(is_slice_move(s[i])){
      char modif = 0;
      if(s[i + 1] == '2' || s[i + 1] == '\'') modif = s[i + 1];
      parse_slice_move(c, s[i], modif);
      if(modif == 0) modif = ' ';
      printf("%c%c%c", s[i], modif, modif == ' ' ? '\0' : ' ');
    }else if(is_axis_move(s[i])){
      char modif = 0;
      if(s[i + 1] == '2' || s[i + 1] == '\'') modif = s[i + 1];
      parse_axis_move(c, s[i], modif);
      if(modif == 0) modif = ' ';
      printf("%c%c%c", s[i], modif, modif == ' ' ? '\0' : ' ');
    }
  }
  printf("\n");
}

int main(int argc, char * argv[]){
  char * scramble_algorithm = 0;

  for(int i = 1; i < argc; i++){
    if(!strcmp(argv[i], "--help")){
      printf("%s", HELP);
      return 0;
    }
    if(argv[i][0] == '-'){
      switch(argv[i][1]){
        case 'c':
          cube_size = atoi(argv[i + 1]);
          if(cube_size < 2 || cube_size > 19){
            printf("Unsupported cube size.\nSupported sizes are in range 2 - 19.\n");
            return 1;
          }
          break;
        case 's':
          scramble_size = atoi(argv[i + 1]);
          if(scramble_size < 1){
            printf("Unsupported scramble size.\nScramble size must be number bigger than 0.\n");
            return 1;
          }
          break;
        case 'u':
          scramble_algorithm = argv[i + 1];
          break;
        default:
          printf("Unknown argument: %s", argv[i]);
          return 1;
      }
    }
  }

  srand(time(NULL));

  if(scramble_algorithm == 0) scramble_algorithm = get_scramble(scramble_size);

  cpiece * cube = cube_init();

  parse_scramble(scramble_algorithm, cube);

  print_cube(cube);

  free(cube);
  return 0;
}
