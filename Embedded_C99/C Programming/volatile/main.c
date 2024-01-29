
#define RAM_ADDRESS 0x20000000

static int u32CheckingVolatile = 0U;
static int count = 0U;
int main(void)
{
   	
	while(1)
	{
		u32CheckingVolatile = *(int*) RAM_ADDRESS ;
		count++;
		if(u32CheckingVolatile != 0U) 
		{
			break;
		}
	}
	count = 100;
	return 0;
}
