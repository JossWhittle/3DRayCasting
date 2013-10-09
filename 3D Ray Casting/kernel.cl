// #pragma OPENCL EXTENSION cl_amd_printf : enable

__constant float PI = 3.14159f, TOLERANCE = 0.01f;

typedef float4 Colour;

typedef struct Camera_s {
	float3 pos;
	float3 norm;
	float3 left;
	float3 up;
} Camera;

typedef struct Ray_s {
	float3 o;
	float3 d; 
} Ray;

int wt(float v, float t) {
	return (v < t + TOLERANCE && v > t - TOLERANCE) ? 1 : 0;
}

int intersectAABB(
	Ray *ray, 
	float3 centre, 
	float *dist
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

	float3 intersect = (*ray).o + ((*ray).d * tmin);
	int axis = 1;

	float delta  = fabs(intersect.x - v0.x);
	float deltab = fabs(intersect.x - v1.x) ;
	if (deltab < delta) {
		axis = 1;
		delta = deltab;
	}

	deltab = fabs(intersect.y - v0.y) ;
	if (deltab < delta) {
		axis = 2;
		delta = deltab;
	}

	deltab = fabs(intersect.y - v1.y) ;
	if (deltab < delta) {
		axis = 2;
		delta = deltab;
	}

	deltab = fabs(intersect.z - v0.z) ;
	if (deltab < delta) {
		axis = 3;
		delta = deltab;
	}

	deltab = fabs(intersect.z - v1.z) ;
	if (deltab < delta) {
		axis = 3;
		delta = deltab;
	}

	*dist = tmin;
	return axis;
}

__kernel void runK(
	write_only image2d_t outImage,
	global read_only Camera *camera
) {
	// Get Node Index, Image Size, and Image Pixel
	int index = get_global_id(0);
	int2 outPixel, outSize = get_image_dim(outImage);
	outPixel.x = index % outSize.x;
	outPixel.y = (index - outPixel.x) / outSize.x;

	
	float2 p = (float2)(
		(0.5f * ((2.0f * (float)outPixel.x) + 1.0f - (float)outSize.x)),
		(0.5f * ((2.0f * (float)outPixel.y) + 1.0f - (float)outSize.y))
	);

	Ray ray;
	ray.d = fast_normalize((*camera).norm + ((*camera).left * p.x) + ((*camera).up * p.y));
	ray.o = (*camera).pos;

	Colour col = (Colour)(0.0f,0.0f,1.0f,1.0f);

	float d0 = 9999999, d1;
	int a0 = 0, a1;

	a0 = intersectAABB(&ray, (float3)(0.0f,0.0f,0.0f), &d0);
	
	a1 = intersectAABB(&ray, (float3)(0.0f,1.0f,0.0f), &d1);
	if (d1 < d0) {
		d0 = d1;
		a0 = a1;
	}

	a1 = intersectAABB(&ray, (float3)(0.0f,-1.0f,0.0f), &d1);
	if (d1 < d0) {
		d0 = d1;
		a0 = a1;
	}

	a1 = intersectAABB(&ray, (float3)(1.0f,0.0f,0.0f), &d1);
	if (d1 < d0) {
		d0 = d1;
		a0 = a1;
	}

	a1 = intersectAABB(&ray, (float3)(-1.0f,0.0f,0.0f), &d1);
	if (d1 < d0) {
		d0 = d1;
		a0 = a1;
	}

	a1 = intersectAABB(&ray, (float3)(0.0f,0.0f,1.0f), &d1);
	if (d1 < d0) {
		d0 = d1;
		a0 = a1;
	}

	a1 = intersectAABB(&ray, (float3)(0.0f,0.0f,-1.0f), &d1);
	if (d1 < d0) {
		d0 = d1;
		a0 = a1;
	}

	switch (a0) {
	case 1:
		col = (Colour)(1.0f,0.0f,0.0f,1.0f);
		break;
	case 2:
		col = (Colour)(0.0f,1.0f,0.0f,1.0f);
		break;
	case 3:
		col = (Colour)(0.5f,0.0f,1.0f,1.0f);
		break;
	}

	// Write to image
	outPixel.y = outSize.y - 1 - outPixel.y;
	write_imagef(outImage, outPixel, col);
}