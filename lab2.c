//Author Peter Adamson
//compile with gcc -pthread prog.c -o prog

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//define the node structure for the queue
typedef struct qNode Node;
struct qNode
{
	int value;
	Node *next;	
};

//global variable declarations
int avg;
int max;
int min;
int fd[2];
int fd2[2];
int fd3[2];
Node *head;
Node *tail;

//function declarations
void *average();
void *maximum();
void *minimum();
void enqueue(Node *newNum);
Node *readNum();
int length();

int main()
{
	//populate the queue
	Node *newNum;
	newNum = readNum();
	while(newNum != NULL)
	{
		enqueue(newNum);
		newNum = readNum();
	}

	//determine how many elements are in the queue
	int size = length();
	int arr[size];
	int i;
	int call;
	int call2;
	int call3;
	int wri;
	int num;

	Node *curr = head;
	for(i = 0; i < size; i++)
	{
		arr[i] = curr->value;
		curr = curr->next;
	}

	//thread declarations
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;

	//pipe syscall
	call = pipe(fd);
	call2 = pipe(fd2);
	call3 = pipe(fd3);

	//write data into the pipes
	for(i = 0; i < size; i++)
	{
		num = arr[i];
		wri = write(fd[1],&num,sizeof(int));
		wri = write(fd2[1],&num,sizeof(int));
		wri = write(fd3[1],&num,sizeof(int));
	}

	//close the pipe inputs
	close(fd[1]);
	close(fd2[1]);
	close(fd3[1]);

	//create the threads
	pthread_create(&thread1, NULL, &average, NULL);
	pthread_create(&thread2, NULL, &maximum, NULL);
	pthread_create(&thread3, NULL, &minimum, NULL);

	//wait for all threads to finish
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);

	//print the results
	printf("The average value is %d\nThe minimum value is %d\nThe maximum value is %d\n",avg,min,max);
}

//will find the average out of an array of numbers
void *average()
{
	int average = 0;
	int count = 0;
	int num;
	int call;
	
	//loop as long as there are still numbers to read in the pipe
	while(1)
	{
		call = read(fd[0],&num, sizeof(int));
		if(call != 4)	//there are no more numbers to read in the pipe
		{
			break;
		}
		count = count + 1;
		average = average + num;
	}
	avg = average / count;
}

//will find the maximum out of an array of numbers
void *maximum()
{
	int tempMax;
	int call;
	call = read(fd2[0],&max,sizeof(int));
	if(call == 4)	//there is at least one number in the pipe
	{
		while(1)	//loop as long as there are still numbers to read in the pipe
		{
			call = read(fd2[0],&tempMax,sizeof(int));
			if(call != 4)	//there are no more numbers in the pipe
			{
				break;
			}
			if(tempMax > max)	//the number we have read is our new max
			{
				max = tempMax;
			}
		}
	}
}

//will find the minimum out of an array of numbers
void *minimum()
{
	int tempMin;
	int call;
	call = read(fd3[0],&min,sizeof(int));
	if(call == 4)	//there is at least one number in the pipe
	{
		while(1)	//loop as long as there are still numbers to read in the pipe
		{
			call = read(fd3[0],&tempMin,sizeof(int));
			if(call != 4)	//there are no more numbers in the pipe
			{
				break;
			}
			if(tempMin < min)	//the number we have read is our new min
			{
				min = tempMin;
			}
		}
	}
}

//reads in a number from standard input
Node *readNum()
{
	//set up the job to be added
	int value;
	Node *newNum = NULL;

	if(!feof(stdin) && (1 == scanf("%d", &value))) //we have a number
	{
		if(value != 0) //we have not reached the end of our numbers
		{
			newNum = (Node*)malloc(sizeof(Node));
			newNum->value = value;
			newNum->next = NULL;
		}
	}
	return newNum;
}

//loads a job into the end of the queue
void enqueue(Node *newNum)
{
	//set up the job to be added
	int value;
	Node *temp = NULL;
	temp = (Node*)malloc(sizeof(Node));
	temp->value = newNum->value;
	temp->next = NULL;

	if(tail == NULL)	//the queue must be empty, so set both head and tail to temp
	{
		head = temp;
		tail = temp;
	}
	else			//the queue is not empty, so add the job to the end of the queue
	{
		tail->next = temp;
		tail = temp;	
	}
}

//find the lenght of the queue
int length()
{
	int length = 0;
	Node *temp = head;

	if(tail == NULL)	//the queue must be empty
	{
		return length;
	}
	else			//the queue is not empty, so increment length and continue until the end is reached
	{
		//continue incrementing length as long as there is data in the queue
		while(temp)
		{
			length++;
			temp = temp->next;
		}
	}
	return length;
}
