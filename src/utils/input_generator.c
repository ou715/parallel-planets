// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main(int argc, char **argv) {

    int n = atoi(argv[1]);
    char *output_path = argv[2];

    FILE *output_file = fopen(output_path, "w");
    printf("==================================================\n");
    printf("Generating inputs for %i bodies\n", n);
    printf("Writing outputs to file %s\n", output_path);

    if (!output_file) {
        fprintf(stderr, "File %s cannot be opened!\n", output_path);
        printf("ERROR");

        fprintf(stderr, "Exiting\n");
        exit(EXIT_FAILURE);
    } else {
        srand(1137);

        fprintf(output_file, "%i\n", n);
        //TODO Make the input ranges configurable
        //TODO positions should be set to be at least some minimum distance away
        for (int i = 0; i < n; i++) {
            double x =((60 + n * 0.5 ) * (double)rand() / RAND_MAX) - 30;
            double y =((60 + n * 0.5 ) * (double)rand() / RAND_MAX) - 30;
            double z =((60 + n * 0.5 ) * (double)rand() / RAND_MAX) - 30;

            double v_x = 0.01 * (double)rand() / RAND_MAX;
            double v_y = 0.01 * (double)rand() / RAND_MAX;
            double v_z = 0.01 * (double)rand() / RAND_MAX;

            double radius = 5 * (double)rand() / RAND_MAX;
            double mass = 10 * (double)rand() / RAND_MAX;

            uint8_t red = (uint8_t)(rand() / (RAND_MAX / 256));
            uint8_t green = (uint8_t)(rand() / (RAND_MAX / 256));
            uint8_t blue = (uint8_t)( rand() / (RAND_MAX / 256));

            fprintf(output_file, "%f %f %f %f %f %f %f %f %hhu %hhu %hhu\n", x,
                                  y, z, v_x, v_y, v_z, radius, mass, red, green, blue);

        }

        fclose(output_file);
        printf("\nFinished generating inputs for %i bodies\n", n);
        printf("==================================================\n");
        return 0;
    }
}
