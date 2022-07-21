
#include <scene.h>
#include <types.h>

int main() {
  extern const Scene *entry;

  Scene *current = Scene_GetCurrent();
  Scene_ReplaceWith(current, entry);

  while (true) {
    Scene_Play(current);
  }
}
