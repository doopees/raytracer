#include "scenes/lab.h"

int main()
{
    auto [world, cam] = generate_scene();
    cam.render(world, "lab.png");
    return 0;
}