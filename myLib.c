#include "myLib.h"
#include <stdio.h>
#include <math.h>

void printVec(vec4 v) {
    printf("[ %7.4f %7.4f %7.4f %7.4f ]\n", v.x, v.y, v.z, v.w);
}

vec4 copyVec(vec4 v){
    vec4 v2;
    float x = v.x;
    float y = v.y;
    float z = v.z;
    float w = v.w;
    v2.x = x;
    v2.y = y;
    v2.z = z;
    v2.w = w;
    return v2;
}

// Scalar-Vector
vec4 svMultiplication(float a, vec4 v){
    
    // Creates a new vec4 to store the new data in
    vec4 v1;

    // Sets the new vec4's data to the old vector's data multiplied by the scalar
    v1.x = v.x*a;
    v1.y = v.y*a;
    v1.z = v.z*a;
    v1.w = v.w*a;

    // Return the new vec to the main program
    return v1;
}

vec4 vecAdd(vec4 v1, vec4 v2){
    vec4 v3;

    v3.x = v1.x + v2.x;
    v3.y = v1.y + v2.y;
    v3.z = v1.z + v2.z;
    v3.w = v1.w + v2.w;

    return v3;
}

vec4 vecSub(vec4 v1, vec4 v2){
    vec4 v3;

    v3.x = v1.x - v2.x;
    v3.y = v1.y - v2.y;
    v3.z = v1.z - v2.z;
    v3.w = v1.w - v2.w;

    return v3;
}

float magOfVec(vec4 v){
    return (float)sqrt(pow(v.x,2.00) + pow(v.y,2.00) + pow(v.z,2.00) + pow(v.w,2.00));
}

vec4 normalize(vec4 v){
	// adding a case to handle zero vectors so nothing nans out
	if (v.x == 0 && v.y == 0 && v.z == 0 && v.w == 0) {
		return (vec4) {0, 0, 0, 0};
	}

    // Formula is 1/|v| * vector column
    float mag = magOfVec(v);
    // printf("\nWithin normalize mag:%f\n",mag);
    return svMultiplication((float)(1.00/mag), v);

}

float dotProduct(vec4 v1, vec4 v2){
    // multiply each vectors corresponding values with each other than add them all up
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
}

// Crosses the first vector with the second vector
vec4 crossProduct(vec4 v1, vec4 v2){
    vec4 v3;
    
    v3.x = (v1.y * v2.z) - (v1.z * v2.y);
    v3.y = (v1.z * v2.x) - (v1.x * v2.z);
    v3.z = (v1.x * v2.y) - (v1.y * v2.x);
    v3.w = 0;

    return v3;
}

// *------------------------- Matrix Functions -------------------------* //

void printMat(mat4 m){
    printf("| %7.4f %7.4f %7.4f %7.4f|\n| %7.4f %7.4f %7.4f %7.4f|\n| %7.4f %7.4f %7.4f %7.4f|\n| %7.4f %7.4f %7.4f %7.4f|\n", m.x.x, m.y.x, m.z.x, m.w.x, m.x.y, m.y.y, m.z.y, m.w.y, m.x.z, m.y.z, m.z.z, m.w.z, m.x.w, m.y.w, m.z.w, m.w.w);
}

mat4 scalMat(float s, mat4 m){
    mat4 o;

    o.x = svMultiplication(s, m.x);
    o.y = svMultiplication(s, m.y);
    o.z = svMultiplication(s, m.z);
    o.w = svMultiplication(s, m.w);

    return o;
}

mat4 matAdd(mat4 m1, mat4 m2){
    mat4 m3;

    // First columns added together (X)
    m3.x = vecAdd(m1.x, m2.x);

    // Add second columns (Y)
    m3.y = vecAdd(m1.y, m2.y);

    // Add third columns (Z)
    m3.z = vecAdd(m1.z, m2.z);

    // Add fourth columns (W)
    m3.w = vecAdd(m1.w, m2.w);

    return m3;
}

mat4 matSub(mat4 m1, mat4 m2){

    mat4 m;

    // X columns
    m.x = vecSub(m1.x, m2.x);

    // Y columns
    m.y = vecSub(m1.y, m2.y);

    // Z Columns
    m.z = vecSub(m1.z, m2.z);

    // W columns
    m.w = vecSub(m1.w, m2.w);

    return m;
}

// m1 = a
// m2 = b
mat4 matMult(mat4 m1, mat4 m2){

    mat4 m;

    // Column 1
    m.x.x = (m1.x.x * m2.x.x) + (m1.y.x * m2.x.y) + (m1.z.x * m2.x.z) + (m1.w.x * m2.x.w);
    m.x.y = (m1.x.y * m2.x.x) + (m1.y.y * m2.x.y) + (m1.z.y * m2.x.z) + (m1.w.y * m2.x.w);
    m.x.z = (m1.x.z * m2.x.x) + (m1.y.z * m2.x.y) + (m1.z.z * m2.x.z) + (m1.w.z * m2.x.w);
    m.x.w = (m1.x.w * m2.x.x) + (m1.y.w * m2.x.y) + (m1.z.w * m2.x.z) + (m1.w.w * m2.x.w);

    // Column 2
    m.y.x = (m1.x.x * m2.y.x) + (m1.y.x * m2.y.y) + (m1.z.x * m2.y.z) + (m1.w.x * m2.y.w);
    m.y.y = (m1.x.y * m2.y.x) + (m1.y.y * m2.y.y) + (m1.z.y * m2.y.z) + (m1.w.y * m2.y.w);
    m.y.z = (m1.x.z * m2.y.x) + (m1.y.z * m2.y.y) + (m1.z.z * m2.y.z) + (m1.w.z * m2.y.w);
    m.y.w = (m1.x.w * m2.y.x) + (m1.y.w * m2.y.y) + (m1.z.w * m2.y.z) + (m1.w.w * m2.y.w);

    // Column 3
    m.z.x = (m1.x.x * m2.z.x) + (m1.y.x * m2.z.y) + (m1.z.x * m2.z.z) + (m1.w.x * m2.z.w);
    m.z.y = (m1.x.y * m2.z.x) + (m1.y.y * m2.z.y) + (m1.z.y * m2.z.z) + (m1.w.y * m2.z.w);
    m.z.z = (m1.x.z * m2.z.x) + (m1.y.z * m2.z.y) + (m1.z.z * m2.z.z) + (m1.w.z * m2.z.w);
    m.z.w = (m1.x.w * m2.z.x) + (m1.y.w * m2.z.y) + (m1.z.w * m2.z.z) + (m1.w.w * m2.z.w);

    // Column 4
    m.w.x = (m1.x.x * m2.w.x) + (m1.y.x * m2.w.y) + (m1.z.x * m2.w.z) + (m1.w.x * m2.w.w);
    m.w.y = (m1.x.y * m2.w.x) + (m1.y.y * m2.w.y) + (m1.z.y * m2.w.z) + (m1.w.y * m2.w.w);
    m.w.z = (m1.x.z * m2.w.x) + (m1.y.z * m2.w.y) + (m1.z.z * m2.w.z) + (m1.w.z * m2.w.w);
    m.w.w = (m1.x.w * m2.w.x) + (m1.y.w * m2.w.y) + (m1.z.w * m2.w.z) + (m1.w.w * m2.w.w);

    // Return the new matrx 'm'
    return m;
}

mat4 matMinor(mat4 m){
    mat4 minor;
    
    // Go through and change each every single element into its determinant so need to set each element
    minor.x.x = (m.y.y * m.z.z * m.w.w) + (m.z.y * m.w.z * m.y.w) + (m.w.y * m.y.z * m.z.w) - (m.y.w * m.z.z * m.w.y) - (m.z.w * m.w.z * m.y.y) - (m.w.w * m.y.z * m.z.y);
    minor.x.y = (m.y.x * m.z.z * m.w.w) + (m.z.x * m.w.z * m.y.w) + (m.w.x * m.y.z * m.z.w) - (m.y.w * m.z.z * m.w.x) - (m.z.w * m.w.z * m.y.x) - (m.w.w * m.y.z * m.z.x);
    minor.x.z = (m.y.x * m.z.y * m.w.w) + (m.z.x * m.w.y * m.y.w) + (m.w.x * m.y.y * m.z.w) - (m.y.w * m.z.y * m.w.x) - (m.z.w * m.w.y * m.y.x) - (m.w.w * m.y.y * m.z.x);
    minor.x.w = (m.y.x * m.z.y * m.w.z) + (m.z.x * m.w.y * m.y.z) + (m.w.x * m.y.y * m.z.z) - (m.y.z * m.z.y * m.w.x) - (m.z.z * m.w.y * m.y.x) - (m.w.z * m.y.y * m.z.x);

    // Second column
    minor.y.x = (m.x.y * m.z.z * m.w.w) + (m.z.y * m.w.z * m.x.w) + (m.w.y * m.x.z * m.z.w) - (m.x.w * m.z.z * m.w.y) - (m.z.w * m.w.z * m.x.y) - (m.w.w * m.x.z * m.z.y);
    minor.y.y = (m.x.x * m.z.z * m.w.w) + (m.z.x * m.w.z * m.x.w) + (m.w.x * m.x.z * m.z.w) - (m.x.w * m.z.z * m.w.x) - (m.z.w * m.w.z * m.x.x) - (m.w.w * m.x.z * m.z.x);
    minor.y.z = (m.x.x * m.z.y * m.w.w) + (m.z.x * m.w.y * m.x.w) + (m.w.x * m.x.y * m.z.w) - (m.x.w * m.z.y * m.w.x) - (m.z.w * m.w.y * m.x.x) - (m.w.w * m.x.y * m.z.x);
    minor.y.w = (m.x.x * m.z.y * m.w.z) + (m.z.x * m.w.y * m.x.z) + (m.w.x * m.x.y * m.z.z) - (m.x.z * m.z.y * m.w.x) - (m.z.z * m.w.y * m.x.x) - (m.w.z * m.x.y * m.z.x);

    // Third Column
    minor.z.x = (m.x.y * m.y.z * m.w.w) + (m.y.y * m.w.z * m.x.w) + (m.w.y * m.x.z * m.y.w) - (m.x.w * m.y.z * m.w.y) - (m.y.w * m.w.z * m.x.y) - (m.w.w * m.x.z * m.y.y); 
    minor.z.y = (m.x.x * m.y.z * m.w.w) + (m.y.x * m.w.z * m.x.w) + (m.w.x * m.x.z * m.y.w) - (m.x.w * m.y.z * m.w.x) - (m.y.w * m.w.z * m.x.x) - (m.w.w * m.x.z * m.y.x);
    minor.z.z = (m.x.x * m.y.y * m.w.w) + (m.y.x * m.w.y * m.x.w) + (m.w.x * m.x.y * m.y.w) - (m.x.w * m.y.y * m.w.x) - (m.y.w * m.w.y * m.x.x) - (m.w.w * m.x.y * m.y.x);
    minor.z.w = (m.x.x * m.y.y * m.w.z) + (m.y.x * m.w.y * m.x.z) + (m.w.x * m.x.y * m.y.z) - (m.x.z * m.y.y * m.w.x) - (m.y.z * m.w.y * m.x.x) - (m.w.z * m.x.y * m.y.x);

    // Fourth Column
    minor.w.x = (m.x.y * m.y.z * m.z.w) + (m.y.y * m.z.z * m.x.w) + (m.z.y * m.x.z * m.y.w) - (m.x.w * m.y.z * m.z.y) - (m.y.w * m.z.z * m.x.y) - (m.z.w * m.x.z * m.y.y);
    minor.w.y = (m.x.x * m.y.z * m.z.w) + (m.y.x * m.z.z * m.x.w) + (m.z.x * m.x.z * m.y.w) - (m.x.w * m.y.z * m.z.x) - (m.y.w * m.z.z * m.x.x) - (m.z.w * m.x.z * m.y.x);
    minor.w.z = (m.x.x * m.y.y * m.z.w) + (m.y.x * m.z.y * m.x.w) + (m.z.x * m.x.y * m.y.w) - (m.x.w * m.y.y * m.z.x) - (m.y.w * m.z.y * m.x.x) - (m.z.w * m.x.y * m.y.x);
    minor.w.w = (m.x.x * m.y.y * m.z.z) + (m.y.x * m.z.y * m.x.z) + (m.z.x * m.x.y * m.y.z) - (m.x.z * m.y.y * m.z.x) - (m.y.z * m.z.y * m.x.x) - (m.z.z * m.x.y * m.y.x);

    return minor;
}

// Checkered board of minuses and pluses
mat4 matCofactor(mat4 m){

    mat4 cf;

    // First Column
    cf.x.x = m.x.x;
    cf.x.y = (-1.00) * m.x.y;
    if(cf.x.y == -0){
        cf.x.y = 0.00;
    }
    cf.x.z = m.x.z;
    cf.x.w = (-1.00) * m.x.w;
    if(cf.x.w == -0){
        cf.x.w = 0.00;
    }

    // Second Column
    cf.y.x = (-1.00) * m.y.x;
    if(cf.y.x == -0){
        cf.y.x = 0.00;
    }
    cf.y.y = m.y.y;
    cf.y.z = (-1.00) * m.y.z;
    if(cf.y.z == -0){
        cf.y.z = 0.00;
    }
    cf.y.w = m.y.w;

    // Third Column
    cf.z.x = m.z.x;
    cf.z.y = (-1.00) * m.z.y;
    if(cf.z.y == -0){
        cf.z.y = 0.00;
    }
    cf.z.z = m.z.z;
    cf.z.w = (-1.00) * m.z.w;
    if(cf.z.w == -0){
        cf.z.w =0.00;
    }

    // Fourth Column
    cf.w.x = (-1.00) * m.w.x;
    if(cf.w.x == -0){
        cf.w.x = 0.00;
    }
    cf.w.y = m.w.y;
    cf.w.z = (-1.00) * m.w.z;
    if(cf.w.z == -0){
        cf.w.z = 0.00;
    }
    cf.w.w = m.w.w;

    return cf;
}

mat4 matTrans(mat4 m){
    mat4 t;

    // Diagonal Elements stays the same
    t.x.x = m.x.x;
    t.y.y = m.y.y;
    t.z.z = m.z.z;
    t.w.w = m.w.w;

    // Now flip the columns and rows
    // First row put into first column
    t.x.y = m.y.x;
    t.x.z = m.z.x;
    t.x.w = m.w.x;
    // First column put into first row
    t.y.x = m.x.y;
    t.z.x = m.x.z;
    t.w.x = m.x.w;

    // Flip the two remaining elements each in the 2nd column and row
    t.y.z = m.z.y;
    t.y.w = m.w.y;
    t.z.y = m.y.z;
    t.w.y = m.y.w;

    // Flip the very last two elements whcih are in the 3rd row and 3rd column
    t.z.w = m.w.z;
    t.w.z = m.z.w;

    return t;
}

// Returns the determinant of m
float matDeter(mat4 m, mat4 minor){
    return (float)(m.x.x * minor.x.x) - (float)(m.x.y * minor.x.y) + (float)(m.x.z * minor.x.z) - (float)(m.x.w * minor.x.w);
}

// Scalar Matrix Multiplication
mat4 smMult(float oD, mat4 tran){
    mat4 a;

    // Goes through each vector and multiplies them by the determinant and then saves them in the 'A'
    a.x = svMultiplication(oD, tran.x);
    a.y = svMultiplication(oD, tran.y);
    a.z = svMultiplication(oD, tran.z);
    a.w = svMultiplication(oD, tran.w);

    return a;
}

mat4 matInv(mat4 m){

    // Might need to recheck this order and modify the methods
    mat4 minor = matMinor(m); 
    mat4 cofactor = matCofactor(minor); 
    mat4 transpose = matTrans(cofactor); 
    float determinant = matDeter(m, minor);
    mat4 inv = smMult((float)(1.00/determinant), transpose);

    return inv;
}

vec4 mvMult(mat4 m, vec4 v){

    vec4 mv;

    // Multiplies each row value by the vector values and adds them all up
    mv.x = (m.x.x * v.x) + (m.y.x * v.y) + (m.z.x * v.z) + (m.w.x * v.w);
    mv.y = (m.x.y * v.x) + (m.y.y * v.y) + (m.z.y * v.z) + (m.w.y * v.w);
    mv.z = (m.x.z * v.x) + (m.y.z * v.y) + (m.z.z * v.z) + (m.w.z * v.w);
    mv.w = (m.x.w * v.x) + (m.y.w * v.y) + (m.z.w * v.z) + (m.w.w * v.w);

    return mv;
}

// Methods added during lab04
mat4 scale(float Bx, float By, float Bz){
    mat4 m;

    m.x = (vec4) {Bx, 0.0, 0.0, 0.0};
    m.y = (vec4) {0.0, By, 0.0, 0.0};
    m.z = (vec4) {0.0, 0.0, Bz, 0.0};
    m.w = (vec4) {0.0, 0.0, 0.0, 1.0};

    return m;
}

mat4 translate(float Ax, float Ay, float Az){
    mat4 m;

    m.x = (vec4) {1.0, 0.0, 0.0, 0.0};
    m.y = (vec4) {0.0, 1.0, 0.0, 0.0};
    m.z = (vec4) {0.0, 0.0, 1.0, 0.0};
    m.w = (vec4) {Ax, Ay, Az, 1.0};

    return m;
}

mat4 rotateZ(float theta){
    float rad = (theta * M_PI)/180.0;
    mat4 m;

    m.x = (vec4) {cos(rad), sin(rad), 0.0, 0.0};
    m.y = (vec4) {-sin(rad), cos(rad), 0.0, 0.0};
    m.z = (vec4) {0.0, 0.0, 1.0, 0.0};
    m.w = (vec4) {0.0, 0.0, 0.0, 1.0};

    
    // m.x = (vec4) {cos(theta), sin(theta), 0.0, 0.0};
    // m.y = (vec4) {-sin(theta), cos(theta), 0.0, 0.0};
    // m.z = (vec4) {0.0, 0.0, 1.0, 0.0};
    // m.w = (vec4) {0.0, 0.0, 0.0, 1.0};

    return m;
}

mat4 rotateX(float theta){
    float rad = (theta * M_PI)/180.0;
    mat4 m;

    m.x = (vec4) {1.0, 0.0, 0.0, 0.0};
    m.y = (vec4) {0.0, cos(rad), sin(rad), 0.0};
    m.z = (vec4) {0.0, -sin(rad), cos(rad), 0.0};
    m.w = (vec4) {0.0, 0.0, 0.0, 1};

    
    // m.x = (vec4) {1.0, 0.0, 0.0, 0.0};
    // m.y = (vec4) {0.0, cos(theta), sin(theta), 0.0};
    // m.z = (vec4) {0.0, -sin(theta), cos(theta), 0.0};
    // m.w = (vec4) {0.0, 0.0, 0.0, 1};


    return m;

}

mat4 rotateY(float theta){
    float rad = (theta * M_PI)/180.0;
    mat4 m;

    m.x = (vec4) {cos(rad), 0.0, -sin(rad), 0.0};
    m.y = (vec4) {0.0, 1.0, 0.0, 0.0};
    m.z = (vec4) {sin(rad), 0.0, cos(rad), 0.0};
    m.w = (vec4) {0.0, 0.0, 0.0, 1.0};

    
    // m.x = (vec4) {cos(theta), 0.0, -sin(theta), 0.0};
    // m.y = (vec4) {0.0, 1.0, 0.0, 0.0};
    // m.z = (vec4) {sin(theta), 0.0, cos(theta), 0.0};
    // m.w = (vec4) {0.0, 0.0, 0.0, 1.0};

    return m;
}
