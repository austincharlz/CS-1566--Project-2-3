#ifndef _MYLIB_H_

#define _MYLIB_H_

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4;

typedef struct {
    float x;
    float y;
} vec2;

typedef struct {
    vec4 x;
    vec4 y;
    vec4 z;
    vec4 w;
} mat4;

// Insert function signatures after this line

// Vector functions
vec4 copyVec(vec4 v);
void printVec(vec4 v);
vec4 svMultiplication(float a, vec4 v);
vec4 vecAdd(vec4 v1, vec4 v2);
vec4 vecSub(vec4 v1, vec4 v2);
float magOfVec(vec4 v);
vec4 normalize(vec4 v);
float dotProduct(vec4 v1, vec4 v2);
vec4 crossProduct(vec4 v1, vec4 v2);

// Matrix Functions
void printMat(mat4 m);
mat4 scalMat(float s, mat4 m);
mat4 matAdd(mat4 m1, mat4 m2);
mat4 matSub(mat4 m1, mat4 m2);
mat4 matMult(mat4 m1, mat4 m2);
mat4 matMinor(mat4 m);
mat4 matCofactor(mat4 m);
mat4 matTrans(mat4 m);
float matDeter(mat4 m, mat4 minor);
mat4 smMult(float oD, mat4 tran);
mat4 matInv(mat4 m);
vec4 mvMult(mat4 m, vec4 v);

// New functions from lab04
mat4 scale(float Bx, float By, float Bz);
mat4 translate(float Ax, float Ay, float Az);
mat4 rotateZ(float theta);
mat4 rotateX(float theta);
mat4 rotateY(float theta);

// Do not put anything after this line

#endif
