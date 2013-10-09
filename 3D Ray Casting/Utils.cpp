#include "Utils.h"

void glPrintString(void *font, const char *str) {
	int len, i;

	len = (int)strlen(str);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(font, str[i]);
	}
}

void glPrintString(int x, int y, std::string str) {
	glRasterPos2i(x,y);
	glPrintString(GLUT_BITMAP_HELVETICA_12, str.c_str());
}

void setFloat3(cl_float3 &v, float x, float y, float z) {
	v.s[0] = x;
	v.s[1] = y;
	v.s[2] = z;
}

float dotFloat3(cl_float3 &a, cl_float3 &b) {
	return (a.s[X] * b.s[X]) + (a.s[Y] * b.s[Y]) + (a.s[Z] * b.s[Z]);
}

cl_float3 crossFloat3(cl_float3 &a, cl_float3 &b) {
	cl_float3 c;
	c.s[X] = (a.s[Y] * b.s[Z]) - (a.s[Z] * b.s[Y]);
	c.s[Y] = (a.s[Z] * b.s[X]) - (a.s[X] * b.s[Z]);
	c.s[Z] = (a.s[X] * b.s[Y]) - (a.s[Y] * b.s[X]);
	return c;
}

cl_float3 addFloat3(cl_float3 &a, cl_float3 &b) {
	cl_float3 c;
	c.s[0] = a.s[0] + b.s[0];
	c.s[1] = a.s[1] + b.s[1];
	c.s[2] = a.s[2] + b.s[2];
	return c;
}

cl_float3 subFloat3(cl_float3 &a, cl_float3 &b) {
	cl_float3 c;
	c.s[0] = a.s[0] - b.s[0];
	c.s[1] = a.s[1] - b.s[1];
	c.s[2] = a.s[2] - b.s[2];
	return c;
}

cl_float3 multFloat3(cl_float3 &a, cl_float3 &b) {
	cl_float3 c;
	c.s[0] = a.s[0] * b.s[0];
	c.s[1] = a.s[1] * b.s[1];
	c.s[2] = a.s[2] * b.s[2];
	return c;
}

cl_float3 divFloat3(cl_float3 &a, cl_float3 &b) {
	cl_float3 c;
	c.s[0] = a.s[0] / b.s[0];
	c.s[1] = a.s[1] / b.s[1];
	c.s[2] = a.s[2] / b.s[2];
	return c;
}

cl_float3 scaleFloat3(cl_float3 &a, float b) {
	cl_float3 c;
	c.s[0] = a.s[0] * b;
	c.s[1] = a.s[1] * b;
	c.s[2] = a.s[2] * b;
	return c;
}

cl_float3 normFloat3(cl_float3 &a) {
	return scaleFloat3(a, FastInvSqrt(len2Float3(a)));
}

float len2Float3(cl_float3 &a) {
	return (a.s[X] * a.s[X]) + (a.s[Y] * a.s[Y]) + (a.s[Z] * a.s[Z]);
}

float lenFloat3(cl_float3 &a) {
	return sqrtf(len2Float3(a));
}

float FastInvSqrt(float x) {
  float xhalf = 0.5f * x;
  int i = *(int*)&x;          // evil floating point bit level hacking
  i = 0x5f3759df - (i >> 1);  // what the fuck?
  x = *(float*)&i;
  x = x*(1.5f-(xhalf*x*x));
  return x;
}

void setTranCam(Camera &cam, float x, float y, float z) {
	cam.pos.s[X] = x;
	cam.pos.s[Y] = y;
	cam.pos.s[Z] = z;
}

void tranCam(Camera &cam, float x, float y, float z) {
	cam.pos.s[X] += x;
	cam.pos.s[Y] += y;
	cam.pos.s[Z] += z;
}

void setRotCam(Camera &cam, float x, float y, float z) {

	cam.norm = Z_VEC;
	cam.up   = Y_VEC;
	rotCam(cam, x,y,z);
}

void rotCam(Camera &cam, float x, float y, float z) {

	if (x != 0.0f) {
		rotXCam(cam, x);
	}

	if (y != 0.0f) {
		rotYCam(cam, y);
	}

	if (z != 0.0f) {
		rotZCam(cam, z);
	}
}

void rotXCam(Camera &cam, float x) {

	x = (x * 3.141592653589793f) / 180.0f;

	float m[3][3] = {
		{1, 0      , 0       },
		{0, cosf(x), -sinf(x)},
		{0, sinf(x), cosf(x) }
	};

	cam.norm = normFloat3(transform(cam.norm, m));
	cam.up   = normFloat3(transform(cam.up  , m));

	cam.pos  = transform(cam.pos , m);
}

void rotYCam(Camera &cam, float y) {
	
	y = (y * 3.141592653589793f) / 180.0f;

	float m[3][3] = {
		{cosf(y) , 0, sinf(y)},
		{0       , 1, 0      }, 
		{-sinf(y), 0, cosf(y)}
	};

	cam.norm = normFloat3(transform(cam.norm, m));
	cam.up   = normFloat3(transform(cam.up  , m));

	cam.pos  = transform(cam.pos , m);
}

void rotZCam(Camera &cam, float z) {
	
	z = (z * 3.141592653589793f) / 180.0f;

	float m[3][3] = {
		{cosf(z), -sinf(z), 0},
		{sinf(z), cosf(z) , 0}, 
		{0      , 0       , 1}
	};

	cam.norm = normFloat3(transform(cam.norm, m));
	cam.up   = normFloat3(transform(cam.up  , m));

	cam.pos  = transform(cam.pos , m);
}

cl_float3 transform(cl_float3 a, float m[3][3]) {

	cl_float3 c;
	c.s[X] = (a.s[X] * m[0][0]) + (a.s[Y] * m[0][1]) + ((a.s[Z] * m[0][2]));
	c.s[Y] = (a.s[X] * m[1][0]) + (a.s[Y] * m[1][1]) + ((a.s[Z] * m[1][2]));
	c.s[Z] = (a.s[X] * m[2][0]) + (a.s[Y] * m[2][1]) + ((a.s[Z] * m[2][2]));
	return c;
}

void makeLUCam(Camera &cam) {

	cam.left = normFloat3(crossFloat3(cam.norm, cam.up));
	cam.up = normFloat3(crossFloat3(cam.left, cam.norm));

	cam.left = scaleFloat3(cam.left, FOV / 1280);
	cam.up = scaleFloat3(cam.up, FOV / 1280);
}