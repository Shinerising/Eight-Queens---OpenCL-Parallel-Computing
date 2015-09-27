
#include <CL/cl.h>
#include <CL/opencl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
using namespace std;

cl_uint2 box[8];

int convertToString(const char *filename, std::string& s)
{
	size_t size;
	char*  str;
	std::fstream f(filename, (std::fstream::in | std::fstream::binary));

	if(f.is_open())
	{
		size_t fileSize;
		f.seekg(0, std::fstream::end);
		size = fileSize = (size_t)f.tellg();
		f.seekg(0, std::fstream::beg);
		str = new char[size+1];
		if(!str)
		{
			f.close();
			return 0;
		}

		f.read(str, fileSize);
		f.close();
		str[size] = '\0';
		s = str;
		delete[] str;
		return 0;
	}
	cout<<"Error: failed to open file\n:"<<filename<<endl;
	return -1;
}

int main(int argc, char* argv[])
{

	/*Step1: Getting platforms and choose an available one.*/
	cl_uint numPlatforms;//the NO. of platforms
	cl_platform_id platform = NULL;//the chosen platform
	cl_int	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (status != CL_SUCCESS)
	{
		cout<<"Error: Getting platforms!"<<endl;
		return 1;
	}

	std::cout<<numPlatforms <<std::endl;

	/*For clarity, choose the first available platform. */
	if(numPlatforms > 0)
	{
		cl_platform_id* platforms = (cl_platform_id* )malloc(numPlatforms* sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);
		platform = platforms[0];
		free(platforms);
	}

	/*Step 2:Query the platform and choose the first GPU device if has one.Otherwise use the CPU as device.*/
	cl_uint				numDevices = 0;
	cl_device_id        *devices;
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);	
	if (numDevices == 0) //no GPU available.
	{
		cout << "No GPU device available."<<endl;
		cout << "Choose CPU as default device."<<endl;
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);	
		devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));

		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices, devices, NULL);
	}
	else
	{
		devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));

		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
	}
	

	/*Step 3: Create context.*/
	cl_context context = clCreateContext(NULL,1, devices,NULL,NULL,NULL);
	
	/*Step 4: Creating command queue associate with the context.*/
	cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);

	/*Step 5: Create program object */
	const char *filename = "eightqueens_Kernel.cl";
	string sourceStr;
	status = convertToString(filename, sourceStr);
	const char *source = sourceStr.c_str();
	size_t sourceSize[] = {strlen(source)};
	cl_program program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);
	
	/*Step 6: Build program. */
	status=clBuildProgram(program, 1,devices,NULL,NULL,NULL);

	/*Step 7: Initial input,output for the host and create memory objects for the kernel*/
	size_t strlength = 8*8*8;
	cout<<"start"<<endl;
	cout<<"Choose the device(1,2)"<<endl;
	int k;
	cin>>k;

	int i,c=0;
	int j;
	int a,b;
	int p=0;
	int ci=0;
	int input[8]={0,0,0,0,0,0,0,0};
	input[0]=strlength;
	int *output = (int*) malloc(strlength);
	clock_t one, two;
	
	
	cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 8 * sizeof(int),(void *) input, NULL);
	cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY , (strlength) * sizeof(int), NULL, NULL);

	/*Step 8: Create kernel object */
	cl_kernel kernel = clCreateKernel(program,"eightqueens", NULL);

	/*Step 9: Sets Kernel arguments.*/
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBuffer);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&outputBuffer);
	
	/*Step 10: Running the kernel.*/
	size_t global_work_size[1] = {strlength};
	for(ci=0;ci<10;ci++){
	c=0;
	p=0;
	if(k==1){
		one = clock();
	for(i=0;i<8*8*8*8*8*8*8*8;i++){
		int n=i;
		input[7]=n%8;
		n=(n-n%8)/8;
		input[6]=n%8;
		n=(n-n%8)/8;
		input[5]=n%8;
		n=(n-n%8)/8;
		input[4]=n%8;
		n=(n-n%8)/8;
		input[3]=n%8;
		n=(n-n%8)/8;
		input[2]=n%8;
		n=(n-n%8)/8;
		input[1]=n%8;
		n=(n-n%8)/8;
		input[0]=n%8;

	j=1;
	for(a=0;a<8;a++){
		for(b=a;b<8;b++){
			if(a!=b){
				if(input[a]==input[b]){j=0;}
				else if(abs(a-b)==abs(input[a]-input[b])){j=0;}
			}
		}
	}
	c += j;
	}
		two = clock();
	}
	else{
		one = clock();
	status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
	status = clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0, (strlength) *sizeof(int), output, 0, NULL, NULL);
	for(i=0;i<strlength;i++){
		c+=output[i];
	}
		two = clock();
	}

	cout<<"output string:"<<endl;
	cout<<c<<endl;
	cout << "Runtime: " << (double)(two-one) * 1000.0 / CLOCKS_PER_SEC << " ms!" << endl;
	}
	/*Step 12: Clean the resources.*/
	status = clReleaseKernel(kernel);//*Release kernel.
	status = clReleaseProgram(program);	//Release the program object.
	status = clReleaseMemObject(inputBuffer);//Release mem object.
	status = clReleaseMemObject(outputBuffer);
	status = clReleaseCommandQueue(commandQueue);//Release  Command queue.
	status = clReleaseContext(context);//Release context.

	if (devices != NULL)
	{
		free(devices);
		devices = NULL;
	}

	return 0;
}