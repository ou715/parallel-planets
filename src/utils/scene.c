//
// Created by nemo on 14/10/25.
//

#include "scene.h"

/**
 *
 * @param image_width
 * @param image_height
 * @return
 */
scene scene_init(int image_width, int image_height) {
    //TODO have more scene parameters configurable
    scene scene;

    //The units are not pixels; this is in world coordinates, image variables relate to resolution that fits into this size
    //Changing these settings changes the field of vision
    const double viewscreen_height = 2.0; //eyes could be rectangles
    const double viewscreen_width = viewscreen_height * ((double) image_width / image_height);

    const double eye_to_viewscreen = 1.0;

    vector3 eye_position = {.x = 0.0, .y = 0.0, .z = 20.0};

    vector3 viewscreen_u = {.x = viewscreen_width, .y = 0, .z = 0.0};
    vector3 viewscreen_v = {.x = 0, .y = -viewscreen_height, .z = 0.0};

    vector3 half_viewscreen_u = vector3_multiply_by_scalar(viewscreen_u, 0.5);
    vector3 half_viewscreen_v = vector3_multiply_by_scalar(viewscreen_v, 0.5);

    vector3 viewscreen_centre = {.x = 0.0, .y = 0.0, .z = eye_to_viewscreen};

    //should probably add more utility functions for vector operations; I really miss operator overloading

    //eye_position - viewscreen_centre - 0.5 * viewscreen_u - 0.5 * viewscreen_v
    vector3 viewscreen_upper_left = subtract_second_vector3_from_first(
                                        subtract_second_vector3_from_first(
                                            subtract_second_vector3_from_first(eye_position,
                                                                                    viewscreen_centre),
                                                                                        half_viewscreen_u),
                                                                                            half_viewscreen_v);


    //Distance between pixels in viewscreen coordinates; u aligns with x and v aligns with -y
    vector3 viewscreen_delta_u = vector3_multiply_by_scalar(viewscreen_u, 1.0 / image_width);
    vector3 viewscreen_delta_v = vector3_multiply_by_scalar(viewscreen_v, 1.0 / image_height);

    vector3 half_viewscreen_delta_u = vector3_multiply_by_scalar(viewscreen_delta_u, 0.5);
    vector3 half_viewscreen_delta_v = vector3_multiply_by_scalar(viewscreen_delta_v, 0.5);

    vector3 viewscreen_first_pixel_location = add_vector3(viewscreen_upper_left,
        add_vector3(half_viewscreen_delta_u, half_viewscreen_delta_v));

    scene.eye_position = eye_position;
    scene.viewscreen_height = viewscreen_height;
    scene.viewscreen_width = viewscreen_width;
    scene.viewscreen_first_pixel_location = viewscreen_first_pixel_location;
    scene.viewscreen_delta_u = viewscreen_delta_u;
    scene.viewscreen_delta_v = viewscreen_delta_v;



    return scene;
}