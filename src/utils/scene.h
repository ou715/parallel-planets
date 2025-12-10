//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
