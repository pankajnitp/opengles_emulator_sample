#include "utility.h"
#include "scene.h"

int effect_on = 0;
int object_effect_on = -1;
int scene_width = 1920;
int scene_height = 1080;

void SceneMouseClickedAt(int x, int y)
{
  effect_on = !effect_on;
  if (effect_on)
  {
    if (x < scene_width / 2)
    {
      if (y < scene_height / 2)
      {
        object_effect_on = 0;
      }
      else
      {
        object_effect_on = 2;
      }
    }
    else
    {
      if (y < scene_height / 2)
      {
        object_effect_on = 1;
      }
      else
      {
        object_effect_on = 3;
      }
    }
    ApplyEffect(1, object_effect_on);
  }
  else
  {
    ApplyEffect(0, object_effect_on);
    object_effect_on = -1;
  }
}