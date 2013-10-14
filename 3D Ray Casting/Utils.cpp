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

cl_float3 multFloat3(cl_float3 &a, float x, float y, float z) {
	cl_float3 c;
	c.s[0] = a.s[0] * x;
	c.s[1] = a.s[1] * y;
	c.s[2] = a.s[2] * z;
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

///
//  Load an image using the FreeImage library and create an OpenCL
//  image out of it
//
Image2D LoadCLImage(Context context, char *fileName)
{
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(fileName, 0);
    FIBITMAP* image = FreeImage_Load(format, fileName);

    // Convert to 32-bit image
    FIBITMAP* temp = image;
    image = FreeImage_ConvertTo32Bits(image);
    FreeImage_Unload(temp);

    int width = FreeImage_GetWidth(image);
    int height = FreeImage_GetHeight(image);

    char *buffer = new char[width * height * 4];
    memcpy(buffer, FreeImage_GetBits(image), width * height * 4);

    FreeImage_Unload(image);

    Image2D clImage;
	
	try {
		clImage = Image2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ImageFormat(CL_RGBA, CL_UNORM_INT8), width, height, 0, buffer);
	} catch (cl::Error e) {
		
		return 0;
	}
    
    return clImage;
}

void loadVolume(char *fileName, int width, int height, int depth, short ***data, int &min, int &max) {
	
	std::ifstream inFile;
	std::size_t size = 0; // here

	inFile.open(fileName, std::ios::in|std::ios::binary|std::ios::ate );
	char* oData = 0;
	
	inFile.seekg(0, std::ios::end); // set the pointer to the end
	size = inFile.tellg() ; // get the length of the file
	std::cout << "Size of file: " << size;
	inFile.seekg(0, std::ios::beg); // set the pointer to the beginning

	oData = new char[ size+1 ]; //  for the '\0'
	inFile.read( oData, size );
	oData[size] = '\0' ; // set '\0' 
	std::cout << " oData size: " << strlen(oData) << "\n";

	short read; 
	int b1, b2, c = 0;
	
	data = new short**[depth];

	for (int s = 0; s < depth; s++) {

		data[s] = new short*[height];
		
		for (int v = 0; v < height; v++) {

			data[s][v] = new short[width];

			for (int u = 0; u < width; u++) {
				
				b1 = ((int)oData[c]) & 0xff; 
				b2 = ((int)oData[c+1]) & 0xff; 
				c += 2;

				read = (short)((b2<<8) | b1);

				data[s][v][u] = read;
						
				if (read < min) {
					min = read;
				} else if (read > max) {
					max = read;
				}
			}
		}
	}

}

AABB makeAABB(float x, float y, float z, float half, int value) {
	AABB box;
	setFloat3(box.pos, x,y,z);
	setFloat3(box.half, half,half,half);
	box.value = value;

	for (int i = 0; i < 8; i++) {
		box.child.s[i] = -1;
	}

	return box;
}
