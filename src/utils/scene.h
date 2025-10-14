//
// Created by nemo on 14/10/25.
//

#ifndef SCENE_H
#define SCENE_H

#include "../ray-tracing/geometry.h"

typedef struct scene {
    double viewscreen_height;
    double viewscreen_width;
    vector3 eye_position;
    vector3 viewscreen_first_pixel_location;
    vector3 viewscreen_delta_u;
    vector3 viewscreen_delta_v;
} scene;

scene scene_init(int image_width, int image_height);

#endif //SCENE_H
