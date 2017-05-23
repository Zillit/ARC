// #ifndef CAMERA_CPP
// #define CAMERA_CPP

// //---------------------Globals------------------------------------

// #include "defines.h"
// #include <GL/gl.h>
// #include <GL/glext.h>
// #include "MicroGlut.h"
// #include "GL_utilities.h"
// #include "VectorUtils3.h"
// using namespace OPENGL;

// #ifdef __cplusplus
// extern "C" {
// #endif

// extern mat4 *_cameraMatrix;

// int mouseClicked;

// extern mat4 tMat, tmpMat1, tmpMat2, rot, trans;
// mat4 rotMat_env, rotMat_sky;
// static vec3 camera, lookAtPoint,
//     up, camera_go_up, camera_go_side, camera_go_straight, tmpSkyBoxCoord;

// static GLfloat angle_y = 0.0, angle_x = 0.0, rel_angle;
// static int lastX = 0, lastY = 0;

// void updateCameraMatrix() {
//     *_cameraMatrix = lookAtv(camera, lookAtPoint, up);
// }
// void cameraManipulationInput(unsigned char key, int x, int y) {
//     camera_go_side = Normalize(CrossProduct(camera_go_straight, camera_go_up));
//     switch (key) {
//         case 'a':
//             camera.x -= camera_go_side.x;
//             lookAtPoint.x -= camera_go_side.x;
//             camera.y -= camera_go_side.y;
//             lookAtPoint.y -= camera_go_side.y;
//             camera.z -= camera_go_side.z;
//             lookAtPoint.z -= camera_go_side.z;
//             break;
//         case 'd':
//             camera.x += camera_go_side.x;
//             lookAtPoint.x += camera_go_side.x;
//             camera.y += camera_go_side.y;
//             lookAtPoint.y += camera_go_side.y;
//             camera.z += camera_go_side.z;
//             lookAtPoint.z += camera_go_side.z;
//             break;
//         case 'w':
//             camera.x += camera_go_straight.x;
//             lookAtPoint.x += camera_go_straight.x;
//             camera.y += camera_go_straight.y;
//             lookAtPoint.y += camera_go_straight.y;
//             camera.z += camera_go_straight.z;
//             lookAtPoint.z += camera_go_straight.z;
//             break;
//         case 's':
//             camera.x -= camera_go_straight.x;
//             lookAtPoint.x -= camera_go_straight.x;
//             camera.y -= camera_go_straight.y;
//             lookAtPoint.y -= camera_go_straight.y;
//             camera.z -= camera_go_straight.z;
//             lookAtPoint.z -= camera_go_straight.z;
//             break;
//         case 'q':
//             camera.y -= 1;
//             lookAtPoint.y -= 1;
//             break;
//         case 'e':
//             camera.y += 1;
//             lookAtPoint.y += 1;
//             break;
//     }
//     updateCameraMatrix();
// }

// void cameraManipulationMouseInput(int button, int state, int x, int y) {
//     if(state == GLUT_DOWN) {
//         mouseClicked = 1;
//     } else {
//         mouseClicked = 0;
//     }
// }
// void mouse(int x, int y)
// {
//     if (mouseClicked == 0)
//     {
// 	lastX = x;
// 	lastY = y;
//     }
// }

// void cameraManipulationMouseFunc(int x, int y) {
//     angle_x = (y - lastY)/200.0 * 3.1415926/4.0;
//     angle_y = (x - lastX)/200.0 * 3.1415926/4.0;


//     // To fix the rotation problem around the x-axis
//     rel_angle = (MultVec3(T(-camera.x, -camera.y, -camera.z), lookAtPoint).z/Norm(MultVec3(T(-camera.x, -camera.y, -camera.z), lookAtPoint)));//(DotProduct(lookAtPoint, SetVector(1,0,0)));
//     if (rel_angle < 0) {
//         rotMat_env = Mult(Rx(angle_x),Ry(angle_y));
//     } else {
//         rotMat_env = Mult(Rx(-angle_x),Ry(angle_y));
//     }
//     rotMat_sky = Mult(Ry(-angle_y),IdentityMatrix());
//     skyBoxCoord = Mult(rotMat_sky, skyBoxCoord);
//     rotMat_sky = Mult(Rx(-angle_x),IdentityMatrix());
//     skyBoxCoord = Mult(rotMat_sky, skyBoxCoord);

//     camera_go_straight = MultVec3(rotMat_env, camera_go_straight);
//     camera_go_side = Normalize(CrossProduct(camera_go_straight, camera_go_up));
//     lookAtPoint = MultVec3(Mult(rotMat_env, T(-camera.x, -camera.y, -camera.z)), lookAtPoint);
//     lookAtPoint = (MultVec3(T(camera.x, camera.y, camera.z), lookAtPoint));

//     updateCameraMatrix();
//     lastX = x;
//     lastY = y;
// }
// #ifdef __cplusplus
// }
// #endif
// #endif
