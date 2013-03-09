#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
typedef char bool;
#define false 0
#define true 1

typedef enum  {
  EAST,
  SOUTH,
  WEST,
  NORTH,
  DirectionButtom
} Direction;

typedef struct {
  size_t const width;
  size_t const height;
} SceneSize;

typedef struct {
  int x;
  int y;
  Direction foward;
} Action;

void Swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void print_actions(Action actions[], size_t len);

bool GravityMoni(SceneSize scene_size, int scene[][scene_size.height]) {
  bool changed = false;
  for (int x = 0; x < scene_size.width; x++) {
    int column[scene_size.height];
    memset(column, 0, sizeof(int) * scene_size.height);
    for (int y = 0, new_y = 0; y < scene_size.height; y++) {
      if (scene[x][y] == 0) {
        continue;
      }
      column[new_y++] = scene[x][y];
    }
    if (memcmp(column, scene[x], sizeof(int) * scene_size.height) != 0) {
      memcpy(scene[x], column, sizeof(int) * scene_size.height);
      changed = true;
    }
  }
  return true;
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

bool Eliminate(SceneSize scene_size, int scene[][scene_size.height]) {
  bool flags[scene_size.width][scene_size.height];
  struct {
    struct {
      int x;
      int y;
    } positions[MAX(scene_size.width, scene_size.height)];
    int len;
    int box_type;
  } same_boxes;
  memset(flags, 0, sizeof(bool) * scene_size.width * scene_size.height);
  for (int x = 0; x < scene_size.width; x++) {
    same_boxes.len = 0;
    same_boxes.box_type = -1;
    for (int y = 0; y < scene_size.height; y++) {
      if (scene[x][y] == same_boxes.box_type) {
        same_boxes.positions[same_boxes.len].x = x;
        same_boxes.positions[same_boxes.len].y = y;
        same_boxes.len++;
        continue;
      }
      if (same_boxes.len >= 3) {
        for (int i = 0; i < same_boxes.len; i++) {
          flags[same_boxes.positions[i].x][same_boxes.positions[i].y] = true;
        }
      }
      if (scene[x][y] == 0) {
        same_boxes.len = 0;
        continue;
      }
      same_boxes.positions[0].x = x;
      same_boxes.positions[0].y = y;
      same_boxes.len = 1;
      same_boxes.box_type = scene[x][y];
    }
    if (same_boxes.len >= 3) {
      for (int i = 0; i < same_boxes.len; i++) {
        flags[same_boxes.positions[i].x][same_boxes.positions[i].y] = true;
      }
    }
  }
  for (int y = 0; y < scene_size.height; y++) {
    same_boxes.len = 0;
    same_boxes.box_type = -1;
    for (int x = 0; x < scene_size.width; x++) {
      if (scene[x][y] == same_boxes.box_type) {
        same_boxes.positions[same_boxes.len].x = x;
        same_boxes.positions[same_boxes.len].y = y;
        same_boxes.len++;
        continue;
      }
      if (same_boxes.len >= 3) {
        for (int i = 0; i < same_boxes.len; i++) {
          flags[same_boxes.positions[i].x][same_boxes.positions[i].y] = true;
        }
      }
      if (scene[x][y] == 0) {
        same_boxes.len = 0;
        continue;
      }
      same_boxes.positions[0].x = x;
      same_boxes.positions[0].y = y;
      same_boxes.len = 1;
      same_boxes.box_type = scene[x][y];
    }
    if (same_boxes.len >= 3) {
      for (int i = 0; i < same_boxes.len; i++) {
        flags[same_boxes.positions[i].x][same_boxes.positions[i].y] = true;
      }
    }
  }

  bool changed = false;
  for (int x = 0; x < scene_size.width; x++) {
    for (int y = 0; y < scene_size.height; y++) {
      if (flags[x][y]) {
        scene[x][y] = 0;
        changed = true;
      }
    }
  }
  return changed;
}


bool Move(SceneSize scene_size, int scene[][scene_size.height],
          Action action) {
  struct {
    int x;
    int y;
  } new_position = {
    action.x,
    action.y
  };
  switch (action.foward) {
    case EAST:
      new_position.x++;
      break;
    case SOUTH:
      new_position.y--;
      break;
    case WEST:
      new_position.x--;
      break;
    case NORTH:
      new_position.y++;
      break;
    default:
      assert(0);
  }
  bool changed = false;

  Swap(&scene[action.x][action.y],
       &scene[new_position.x][new_position.y]);

  if (GravityMoni(scene_size, scene)) {
    changed = true;
  }
  if (Eliminate(scene_size, scene)) {
    changed = true;
  }
  while (true) {
    if (GravityMoni(scene_size, scene)) {
      changed = true;
    } else {
      return changed;
    }
    if (Eliminate(scene_size, scene)) {
      changed = true;
    } else {
      return changed;
    }
  }
}
  
  
bool IsFinished(SceneSize scene_size, int scene[][scene_size.height]) {
  int *p = (int *)scene;
  for (int i = 0; i < scene_size.width * scene_size.height; i++) {
    if (p[i] != 0) {
      return false;
    }
  }
  return true;
}

bool CantMove(int x, int y, Direction foward, SceneSize scene_size) {
  return ((x == 0 && foward == WEST) ||
          (y == 0 && foward == SOUTH) ||
          (x == (scene_size.width-1) && foward == EAST) ||
          (y == (scene_size.height-1) && foward == NORTH));
}

bool Solve(SceneSize scene_size, int scene[][scene_size.height],
    Action actions[], int index, size_t len) {
  if (IsFinished(scene_size, scene)) return true;
  if (index == len) return false;

  int (*new_scene)[scene_size.height];

  for (int x = 0; x < scene_size.width; x++) {
    for (int y = 0; y < scene_size.height; y++) {
      if (scene[x][y] == 0) {
        break;
      }
      for (Direction foward = EAST;
           foward < DirectionButtom;
           foward++) {
        if (CantMove(x, y, foward, scene_size)) {
          continue;
        }
        new_scene = (int (*)[scene_size.height])malloc(
              sizeof(int) * scene_size.height * scene_size.width);
        if (new_scene == NULL) {
          printf("new_scene == NULL\n");
          abort();
        }
        memcpy(new_scene, scene, scene_size.width * scene_size.height * sizeof(int));
        actions[index].x = x;
        actions[index].y = y;
        actions[index].foward = foward;
        Move(scene_size, new_scene, actions[index]);
        if (Solve(scene_size, new_scene, actions, index+1, len)) {
          free(new_scene);
          return true;
        }
        free(new_scene);
      }
    }
  }
  return false;
}

void print_actions(Action actions[], size_t len) {
  char *directions[] = {
    "EAST",
    "SOUTH",
    "WEST",
    "NORTH"
  };
  for (int i = 0; i < len; i++) {
    printf("(%d, %d): %s\n", actions[i].x, actions[i].y,
        directions[actions[i].foward]);
  }
}

void print_scene(SceneSize scene_size, int scene[][scene_size.height]) {
  for (int y = scene_size.height-1; y >= 0; y--) {
    printf("{");
    for (int x = 0; x < scene_size.width - 1; x++)
      printf("%d, ", scene[x][y]);
    printf("%d}\n", scene[scene_size.width-1][y]);
  }
}

#define SCENE_WIDTH 7
#define SCENE_HEIGHT 12
#define MAX_STEP_NUM 10
int main(void) { 
  SceneSize scene_size = { SCENE_WIDTH, SCENE_HEIGHT };
  
  int scene[SCENE_WIDTH][SCENE_HEIGHT];
  size_t step_count;
  while (1) {
    scanf("%zu", &step_count);
    for (int i = 0; i < SCENE_WIDTH; i++) {
      for (int j = 0; i < SCENE_HEIGHT; j++) {
        scanf("%d", &scene[i][j]);
        if (scene[i][j] == 0) {
          for (int k = j+1; k < SCENE_HEIGHT; k++) {
            scene[i][k] = 0;
          }
          break;
        }
      }
    }
    Action actions[MAX_STEP_NUM];
    if (Solve(scene_size, scene, actions, 0, step_count)) {
      print_actions(actions, step_count);
    } else {
      printf("Im sooooorry\n");
    }
  }

  return 0;
}
