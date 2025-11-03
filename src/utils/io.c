//
// Created by nemo on 14/10/25.
//
#define STBI_WRITE_NO_COMPRESS 1
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "io.h"
#include <errno.h>
#include "../../dependencies/stb_image_write.h"

void save_image_png(const pixel_colour *image, int image_width, int image_height, char *filename) {
    const int row_size_bytes = image_width * sizeof(pixel_colour);
    //printf("Image byte_stride is %d\n", row_size_bytes);

    //printf("Saving image to relative path %s\n", filename);
    stbi_write_png(filename, image_width, image_height, 3, image, row_size_bytes);
    //stbi_write_bmp(filename, image_width, image_height, 3, image);
    //printf("Saving image to relative path %s\n", filename);

}

//TODO handle printf debugging more gracefully
/**
 * Function which takes the information from a text file of a specific format and maps it into a sequence of solid_colour_sphere
 * The expected input format is
 * n
 * x y z vx vy vz rad r g b
 * Where xyz are coordinates, vxvyvz are corresponding velocities, rad is radius and rgb are 0-255 colour values
 * @param file_path File containing information about the spheres
 * @param[out] spheres The variable to write the information from the file
 */
void read_sphere_configuration(char *file_path, solid_colour_sphere **spheres) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, "File %s cannot be opened!\n", file_path);
            printf("Error %d: %s\n", errno, strerror(errno));
        fprintf(stderr, "Exiting\n");
        exit(EXIT_FAILURE);
    } else {
        printf( "File opened succesfully! \n");
    }

    int sphere_number;
    if (fscanf(file, "%d", &sphere_number) != 1) {
        fprintf(stderr, "Number of sphere is incorrectly specified! It should be a string at the start of the file\n");
        fprintf(stderr, "Exiting\n");
        exit(EXIT_FAILURE);
    } else {
        printf("The number of input spheres is %d\n", sphere_number);
    }

    *spheres = malloc(sphere_number * sizeof(solid_colour_sphere));
    if (!&(*spheres)) {
        printf("Spheres memory was not allocated!\n");
    }

    for (int i = 0; i < sphere_number; i++) {
        //printf("Mapping sphere %d\n", i);
        if (fscanf(file, "%lf %lf %lf %lf %lf %lf", &(*spheres)[i].sphere.position.x,
                                                &(*spheres)[i].sphere.position.y,
                                                &(*spheres)[i].sphere.position.z,
                                                &(*spheres)[i].sphere.velocity.x,
                                                &(*spheres)[i].sphere.velocity.y,
                                                &(*spheres)[i].sphere.velocity.z) != 6) {
                                                    fprintf(stderr, "Error - first 6 entries in each row should be doubles, denoting position and velocity.\n");
                                                    fprintf(stderr, "Exiting\n");
                                                    exit(EXIT_FAILURE);
                                                } else {
                                                    //printf("The velocity of sphere3_m is x: %.2f, y: %.2f, z: %.2f\n", spheres[i]->sphere.velocity.x,
                                                    //  spheres[i]->sphere.velocity.y, spheres[i]->sphere.velocity.z);
                                                }

        if (fscanf(file, "%lf", &(*spheres)[i].sphere.radius) != 1) {
            fprintf(stderr, "Error - the 7th entry should be the radius of the sphere.\n");
            fprintf(stderr, "Exiting\n");
            exit(EXIT_FAILURE);
        } else {
            //printf("The radius of the sphere is %.2f\n", spheres[i]->sphere.radius);
        }

        if (fscanf(file, "%lf", &(*spheres)[i].sphere.mass) != 1) {
            fprintf(stderr, "Error - the 8th entry should be the mass of the sphere\n");
            fprintf(stderr, "Exiting\n");
            exit(EXIT_FAILURE);
        } else {
            //printf("The mass of the sphere is %.2f\n", spheres[i]->sphere.mass);
        }

        if (fscanf(file, "%hhu %hhu %hhu ", &(*spheres)[i].colour.r,
                                            &(*spheres)[i].colour.g,
                                            &(*spheres)[i].colour.b) != 3) {
                                            fprintf(stderr, "Error - entries 9, 10 and 11 should be integers lower than 256, denoting rgb values\n");
                                            fprintf(stderr, "Exiting\n");
                                            exit(EXIT_FAILURE);
        } else {
            //printf("The colour of the sphere is %i %i %i\n", spheres[i]->colour.r, spheres[i]->colour.g, spheres[i]->colour.b);
        }
        //printf("Done mapping sphere %d\n", i);
    }
    // printf("Succesfully loaded %i spheres!\n", sphere_number);
    //  printf("The colour of the sphere is %i %i %i\n", spheres[0]->colour.r, spheres[0]->colour.g, spheres[0]->colour.b);
    // printf("The colour of the sphere is %.2f %.2f %.2f\n", spheres[0]->sphere.velocity.x, spheres[0]->sphere.velocity.x, spheres[0]->sphere.velocity.x);

    fclose(file);
}
