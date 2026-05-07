#include <graphintrin.h>

static void draw_diagonal(int size) {
  graph_set_color(0x00ff00);
  for (int i = 0; i < size; ++i)
    graph_draw_pixel(i, i);
}

int main(void) {
  graph_init(64, 64);
  graph_set_color(0xff0000);
  for (int x = 4; x < 20; ++x)
    graph_draw_pixel(x, 8);
  draw_diagonal(16);
  graph_flush();
  return 0;
}
