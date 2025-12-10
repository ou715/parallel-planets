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

#ifndef MPI_UTILS_H
#define MPI_UTILS_H
#include <mpi/mpi.h>
#include "../ray-tracing/ray_trace.h"

MPI_Datatype create_pixel_colour_mpi_type(void);
MPI_Datatype create_vector3_mpi_type(void);
MPI_Datatype create_sphere_mpi_type(void);
MPI_Datatype create_solid_colour_sphere_mpi_type(void);
void allocate_rows_to_processes_blocks(int image_height, int *rows_to_process);
#endif //MPI_UTILS_H
