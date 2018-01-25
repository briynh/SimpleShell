#include <pthread.h>
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <assert.h>    
#include <signal.h>
#include <alloca.h>
#include <time.h>
#include <termios.h>
#include <string.h>

#define BUFFER_SIZE 10
#define MAX_TRIES 10



//Static Data Variables
static int ibuffer[BUFFER_SIZE];	//The Material Buffer
static int In = 0;					//Next available empty slot in buffer
static int Out = 0;					//Next available data slot
static int Counter = 0;

static int M0Counter = 0;
static int M1Counter = 0;
static int M2Counter = 0;

static int ACounter = 0;
static int BCounter = 0;
static int CCounter = 0;

static int pauser = 0;


static int OperatorCount = 3;


pthread_mutex_t imutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Xmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Ymutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Zmutex = PTHREAD_MUTEX_INITIALIZER;

void *operator(void* raw_operatorNo)
{
	int operatorNo = (intptr_t)raw_operatorNo;

	//printf("Operator %d: Starting\n", operatorNo);

	int materials[2] = { -1,-1 };
	int hasMaterials = 0;
	int tester = 0;
	int outputProduct = 0;
	int toolCount = 0;

	while (1) {
		while (pauser == 1)
		{
			//Do nothing
		}

		while (Counter <= 0+(OperatorCount-1))
		{
			//Do nothing
		}
		//Start Critical Section

		//Checks if operator still needs materials
		if (hasMaterials == 0)
		{
			pthread_mutex_lock(&imutex);

			//Checks if the material counter is empty
			if (materials[0] == -1)
			{
				//printf("Mutex Locked\n");
				printf("Operator %d received material %d\n", operatorNo, ibuffer[Out]);
				materials[0] = ibuffer[Out];
				if (Out == 9)
				{
					Out = 0;
				}
				else
				{
					Out++;
				}
				Counter--;
				pthread_mutex_unlock(&imutex);
				//printf("Mutex Unlocked\n");
				continue;
			}
			else if (materials[0] == ibuffer[Out])	//Checks if already holding same item
			{
				ibuffer[In] = materials[0];
				printf("Operator %d returned material %d\n", operatorNo, materials[0]);
				materials[0] = -1;
				if (Out == 9)
				{
					Out = 0;
				}
				else
				{
					Out++;
				}
				if (In == 9)
				{
					In = 0;
				}
				else
				{
					In++;
				}
				
				pthread_mutex_unlock(&imutex);
				continue;
			}
			else
			{
				if (((ACounter - BCounter) > 9) || ((ACounter - CCounter) > 9))
				{
					if ((materials[0] + ibuffer[Out]) == 1)
					{
						ibuffer[In] = materials[0];

						materials[0] = -1;
						
						if (Out == 9)
						{
							Out = 0;
						}
						else
						{
							Out++;
						}

						

						if (In == 9)
						{
							In = 0;
						}
						else
						{
							In++;
						}
						pthread_mutex_unlock(&imutex);
							continue;
					}
				}

				if (((BCounter - ACounter) > 9) || ((BCounter - CCounter) > 9))
				{
					if ((materials[0] + ibuffer[Out]) == 2)
					{
						ibuffer[In] = materials[0];
						materials[0] = -1;
						
						if (Out == 9)
						{
							Out = 0;
						}
						else
						{
							Out++;
						}
						if (In == 9)
						{
							In = 0;
						}
						else
						{
							In++;
						}
						pthread_mutex_unlock(&imutex);
						continue;
					}

				}

				if (((CCounter - BCounter) > 9) || ((CCounter - ACounter) > 9))
				{
					if ((materials[0] + ibuffer[Out]) == 3)
					{
						ibuffer[In] = materials[0];
						materials[0] = -1;
						
						if (Out == 9)
						{
							Out = 0;
						}
						else
						{
							Out++;
						}
						if (In == 9)
						{
							In = 0;
						}
						else
						{
							In++;
						}
						pthread_mutex_unlock(&imutex);
						continue;
					}
				}

				printf("Operator %d received material %d\n", operatorNo, ibuffer[Out]);
				materials[1] = ibuffer[Out];
				if (Out == 9)
				{
					Out = 0;
				}
				else
				{
					Out++;
				}
				Counter--;
				hasMaterials = 1;
				pthread_mutex_unlock(&imutex);
				//printf("Mutex Unlocked\n");
				continue;
			}
		}
		else {	//Now needs to get the tools

			outputProduct = materials[0] + materials[1];

			if (outputProduct == 1) //Product A: Made by materials 1 and 2, and tools X and y
			{
				for (int i = 0; i < MAX_TRIES; i++)
				{
					if (pthread_mutex_trylock(&Xmutex) == 0)
					{
						//printf("Successfully obtained ONE tool\n");
						toolCount = 1;
						break;
					}
				}
				for (int i = 0; i < MAX_TRIES; i++)
				{
					if (pthread_mutex_trylock(&Ymutex) == 0)
					{
						//printf("Successfully obtained TWO tools\n");
						toolCount = 2;
						break;
					}
				}
				if (toolCount < 2)
				{
					pthread_mutex_unlock(&Xmutex);
					continue;
				}
				else if (toolCount == 2)
				{
					printf("Product A was successfully created by Operator %d and added to output queue\n",operatorNo);
					ACounter++;
					pthread_mutex_unlock(&Xmutex);
					pthread_mutex_unlock(&Ymutex);
				}
			}

			if (outputProduct == 2) //Product B: Made by materials 2 and 3, and tools Y and Z
			{
				for (int i = 0; i < MAX_TRIES; i++)
				{
					if (pthread_mutex_trylock(&Ymutex) == 0)
					{
						//printf("Successfully obtained ONE tool\n");
						toolCount = 1;
						break;
					}
				}
				for (int i = 0; i < MAX_TRIES; i++)
				{
					if (pthread_mutex_trylock(&Zmutex) == 0)
					{
						//printf("Successfully obtained TWO tools\n");
						toolCount = 2;
						break;
					}
				}
				if (toolCount < 2)
				{
					pthread_mutex_unlock(&Ymutex);
					continue;
				}
				else if (toolCount == 2)
				{
					printf("Product B was successfully created by Operator %d and added to output queue\n",operatorNo);
					BCounter++;
					pthread_mutex_unlock(&Ymutex);
					pthread_mutex_unlock(&Zmutex);
				}
			}

			if (outputProduct == 3) //Product C: Made by materials 3 and 1, and tools Z and X
			{


				for (int i = 0; i < MAX_TRIES; i++)
				{
					if (pthread_mutex_trylock(&Zmutex) == 0)
					{
						//printf("Successfully obtained ONE tool\n");
						toolCount = 1;
						break;
					}
				}
				for (int i = 0; i < MAX_TRIES; i++)
				{
					if (pthread_mutex_trylock(&Xmutex) == 0)
					{
						//printf("Successfully obtained TWO tools\n");
						toolCount = 2;
						break;
					}
				}
				if (toolCount < 2)
				{
					pthread_mutex_unlock(&Zmutex);
					continue;
				}
				else if (toolCount == 2)
				{
					printf("Product C was successfully created by Operator %d and added to output queue\n",operatorNo);
					CCounter++;
					pthread_mutex_unlock(&Zmutex);
					pthread_mutex_unlock(&Xmutex);
				}
			}

			//Use materials to create output
			hasMaterials = 0;
			materials[0] = -1;
			materials[1] = -1;
			toolCount = 0;
			//printf("Current Totals are A:%d, B:%d, C:%d\n", ACounter, BCounter, CCounter);
			continue;
		}
	}


}

void *generator(void* raw_generatorNo)
{
	int generatorNo = (intptr_t)raw_generatorNo;
	//printf("Generator %d: Starting\n", generatorNo);


	while (1) {
		while (pauser == 1)
		{
			//Do nothing
		}
		while (Counter >= BUFFER_SIZE-(OperatorCount -1))
		{
			//Do nothing
		}
		while ((ibuffer[In - 1] == generatorNo)|| (ibuffer[In - 2] == generatorNo))
		{
			//Do nothing
		}
		//Start Critical Section
		pthread_mutex_lock(&imutex);
		//printf("Mutex Locked\n");
		ibuffer[In] = generatorNo;
		printf("Material %d is generated and added to input buffer. %d materials ready\n", generatorNo, Counter+1);
		if (In == 9)
		{
			In = 0;
		}
		else {
			In++;
		}
		Counter++;

		if (generatorNo == 0)
		{
			M0Counter++;
		}
		else if (generatorNo == 1)
		{
			M1Counter++;
		}
		else {
			M2Counter++;
		}
		pthread_mutex_unlock(&imutex);
		//End Critical Section
		//printf("Mutex unlocked, total count: %d\n", Counter);
	}

}

int getch(void)
{
	int c=0;

      struct termios org_opts, new_opts;
      int res=0;

      res=tcgetattr(STDIN_FILENO, &org_opts);

      assert(res==0);

      memcpy(&new_opts, &org_opts, sizeof(new_opts));

      new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
      tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
      c=getchar();
      res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
      assert(res==0);

      return(c);	

}

void* listener(void* raw_generatorNo)
{
	char c;
	int TempOut;
	while(1)
	{
		c = getch();
		if (c == 'p')
		{
			
			pauser = 1;
			sleep(1);
			printf("Execution of program is paused!\n");



			printf("\nTotal for Material 0: %d", M0Counter);
			printf("\nTotal for Material 1: %d", M1Counter);
			printf("\nTotal for Material 2: %d\n", M2Counter);

			printf("Input Buffer:");
			TempOut = Out;
			while(TempOut != In)
			{
				printf(" %d ", ibuffer[TempOut]);
				TempOut = (TempOut+1)%(BUFFER_SIZE);
			}
			printf("\nTotal for Product A: %d", ACounter);
			printf("\nTotal for Product B: %d", BCounter);
			printf("\nTotal for Product C: %d", CCounter);
			printf("\nOutput Queue has %d products",ACounter + BCounter + CCounter);


		}
		if (c == 'r')
		{
			pauser = 0;
		}
	}

}




int main(int argc, char *argv[])
{
	pthread_t *generators;
	pthread_t *operators;
	pthread_t *listeners;
	int nGenerators = 3;
	int nOperators = OperatorCount;

	generators = (pthread_t*)alloca(nGenerators * sizeof(pthread_t));
	operators = (pthread_t*)alloca(nOperators * sizeof(pthread_t));
	listeners = (pthread_t*)alloca(1* sizeof(pthread_t));
	int i = 0;

	pthread_create(&listeners[0], NULL, listener,(void*)(intptr_t)i);
	
	

	for (int i = 0; i < nGenerators; i++)
	{
		pthread_create(&generators[i], NULL, generator,(void*)(intptr_t)i);
	}

	for (int i = 0; i < nOperators; i++)
	{
		pthread_create(&operators[i], NULL, operator, (void*)(intptr_t)i);
	}
	for (int i = 0; i < nGenerators; i++)
	{
		pthread_join(generators[i], NULL);
	}

	for (int i = 0; i < nOperators; i++)
	{
		pthread_join(operators[i], NULL);
	}

	pthread_join(listeners[0], NULL);
	
	return 0;
}


