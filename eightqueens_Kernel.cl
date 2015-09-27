__kernel void helloworld(__global int* in, __global int* out)
{
	int i = get_global_id(0);
	int j,c=0,n,m;
	int a,b;
	int input[8]={0,0,0,0,0,0,0,0};
	int max=8*8*8*8*8*8*8*8/in[0];
	
	for(m=i*max;m<(i+1)*max;m++){

		n=m;
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

	out[i] = c;
}