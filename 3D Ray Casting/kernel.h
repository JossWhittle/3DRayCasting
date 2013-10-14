
__constant float PI = 3.14159f, TOLERANCE = 0.01f;

__constant int X_AXIS = 1, Y_AXIS = 2, Z_AXIS = 3;

__constant float4 INVERT = (float4)(-1.0f,-1.0f,-1.0f,1.0f);

typedef float4 Colour;

typedef struct Camera_s {
	float3 pos;
	float3 norm;
	float3 left;
	float3 up;
	int2 size;
} Camera;

typedef struct Ray_s {
	float3 o;
	float3 d; 
} Ray;

typedef struct AABB_s {
	float3 pos;
	float3 half;
	int8 child;

	int value;
} AABB;

int wt(float v, float t) {
	return (v < t + TOLERANCE && v > t - TOLERANCE) ? 1 : 0;
}

int intersectAABB(
	Ray *ray, 
	float3 centre, 
	float *dist,
	int *axis, 
	float3 *intersect,
	float2 *texCoord
);