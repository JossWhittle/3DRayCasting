// #pragma OPENCL EXTENSION cl_amd_printf : enable

#include "kernel.h"

const sampler_t SAMPLER = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

__kernel void runK(
	global write_only float  *depthBuf,
	global write_only float  *colourBuf,
	global read_only Camera *camera,
	read_only image2d_t spriteMap
) {
	// Get Node Index, Image Size, and Image Pixel
	int index = get_global_id(0);
	int indexDepth  = (index * 3) + 2;
	int indexColour = index * 4;

	int2 outPixel, outSize = (int2)(1280,720);
	outPixel.x = index % outSize.x;
	outPixel.y = (index - outPixel.x) / outSize.x;

	
	float2 p = (float2)(
		(0.5f * ((2.0f * (float)outPixel.x) + 1.0f - (float)outSize.x)),
		(0.5f * ((2.0f * (float)(outSize.y - 1 - outPixel.y)) + 1.0f - (float)outSize.y))
	);

	Ray ray;
	ray.d = fast_normalize((*camera).norm + ((*camera).left * p.x) + ((*camera).up * p.y));
	ray.o = (*camera).pos;

	Colour col = (Colour)(0.0f,0.0f,1.0f,1.0f);

	float3 intersect = (float3)(0.0f,0.0f,0.0f);
	float2 texCoord  = (float2)(0.0f,0.0f);
	float dist = 9999;
	int axis = 0;

	intersectAABB(&ray, (float3)(0.0f,0.0f,0.0f), &dist, &axis, &intersect, &texCoord);
	intersectAABB(&ray, (float3)(0.0f,1.0f,0.0f), &dist, &axis, &intersect, &texCoord);
	intersectAABB(&ray, (float3)(0.0f,-1.0f,0.0f), &dist, &axis, &intersect, &texCoord);
	intersectAABB(&ray, (float3)(1.0f,0.0f,0.0f), &dist, &axis, &intersect, &texCoord);
	intersectAABB(&ray, (float3)(-1.0f,0.0f,0.0f), &dist, &axis, &intersect, &texCoord);
	intersectAABB(&ray, (float3)(0.0f,0.0f,1.0f), &dist, &axis, &intersect, &texCoord);
	intersectAABB(&ray, (float3)(0.0f,0.0f,-1.0f), &dist, &axis, &intersect, &texCoord);
	

	if (axis != 0) {
		col = read_imagef( spriteMap, SAMPLER, (texCoord * 64) + ((float2)((abs(axis) - 1) * 64 , 0.0f)) );
	}

	/*if (axis != 0 && (((int)(texCoord.x * 100) % 10 == 0) || ((int)(texCoord.y * 100) % 10 == 0))) {
		col *= INVERT;
		col += 1.0f;
	}*/

	// Write to image
	colourBuf[indexColour] = col.x;
	colourBuf[indexColour+1] = col.y;
	colourBuf[indexColour+2] = col.z;
	colourBuf[indexColour+3] = col.w;
	depthBuf[indexDepth] = dist;
}

int intersectAABB(
	Ray *ray, 
	float3 centre, 
	float *dist,
	int *axis, 
	float3 *intersect,
	float2 *texCoord
) {
	float3 dirFrac, t0,t1, v0,v1;
	float tmin,tmax;

	dirFrac = 1.0f / (*ray).d; 

	v0 = centre - (float3)(0.5f,0.5f,0.5f);
	v1 = centre + (float3)(0.5f,0.5f,0.5f);

	t0 = (v0 - (*ray).o) * dirFrac; 
	t1 = (v1 - (*ray).o) * dirFrac; 

	tmin = fmax(fmax(fmin(t0.x, t1.x), fmin(t0.y, t1.y)), fmin(t0.z, t1.z));
	tmax = fmin(fmin(fmax(t0.x, t1.x), fmax(t0.y, t1.y)), fmax(t0.z, t1.z));

	if (tmax < 0.0f) {
		return -1;
	} else if (tmin > tmax) {
		return -2; // <- Causes weird clipping error. Might be fixed by tweaking KD generation
	}
		
	if (tmin <= TOLERANCE && tmax <= TOLERANCE) {
		return -3;
	}

	if (tmin > *dist) {
		return -4;
	}
	*dist = tmin;

	*intersect = (*ray).o + ((*ray).d * tmin);
	float3 p = (*intersect) - v0;

	*axis = X_AXIS;
	*texCoord = (float2)(p.z, p.y);

	float delta  = fabs((*intersect).x - v0.x);
	float deltab = fabs((*intersect).x - v1.x) ;
	if (deltab < delta) {
		*axis = -X_AXIS;
		*texCoord = (float2)(p.z, p.y);
		delta = deltab;
	}

	deltab = fabs((*intersect).y - v0.y) ;
	if (deltab < delta) {
		*axis = Y_AXIS;
		*texCoord = (float2)(p.x, p.z);
		delta = deltab;
	}

	deltab = fabs((*intersect).y - v1.y) ;
	if (deltab < delta) {
		*axis = -Y_AXIS;
		*texCoord = (float2)(p.x, p.z);
		delta = deltab;
	}

	deltab = fabs((*intersect).z - v0.z) ;
	if (deltab < delta) {
		*axis = Z_AXIS;
		*texCoord = (float2)(p.x, p.y);
		delta = deltab;
	}

	deltab = fabs((*intersect).z - v1.z) ;
	if (deltab < delta) {
		*axis = -Z_AXIS;
		*texCoord = (float2)(p.x, p.y);
	}

	return 1;
}