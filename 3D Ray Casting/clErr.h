#include <stdio.h>
#include <CL/opencl.h>

/**
* Print OpenCL Error Messages
*/
void clPrintErr(cl_int status, char * prefix, FILE *fp);