// Charles Bucher
// 5/9/17
// Consumer/Producer Threading Program

// Expected platform: Ubunto Mate i386, 32-bit.

// Significant portion of code modified/used from Professor Justin Okun of
// California State University Fullerton, Spring 2017, CPSC 351.

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <list>
#include <semaphore.h>

#include "BufferItem.hpp"
#include "BoundedBuffer.hpp"
#include "BufferItemCollection.hpp"

using namespace std;
using namespace std::chrono;

//Thread-safe bounded buffer to facilitate the item transfers 
BoundedBuffer boundedBuffer;

//Thread-safe collection that will contain all of the produced/consumed items
BufferItemCollection itemCollection;

//The number of items to be produced and consumed 
const int ITEM_COUNT = 100000;

bool go = true;

list<pthread_t> consumer_threads;
list<pthread_t> producer_threads;
		
//Creates the items the producer will pass to the consumer
// via the bounded buffer.
list<BufferItem>* CreateItems (int numOfItems)
{
	list<BufferItem>* items = new list<BufferItem>; 
	BufferItem item;
	
	//Convenient variable to help generate unique ids.
	static int count = 0;
	
	for (int i = 0; i < numOfItems; i++)
	{
		item._id = count++;
		items->push_back (item);
	}
		
	return items;
}

//The code executed by each producer thread
void* ProducerThread(void* ptr)
{
	cout << "Started ProducerThread: " << pthread_self() << endl;
	
	list<BufferItem> items = *((list<BufferItem>*)ptr);

	for (BufferItem item: items)
	{
		boundedBuffer.Produce(item);
	}

	delete (list<BufferItem>*)ptr;

	return NULL;
}

//The code executed by each consumer thread.
void* ConsumerThread(void* ptr)
{
	cout << "Started ConsumerThread: " << pthread_self() << endl;
	
    //Make this thread cancellable at any time
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	while (go)
	{
		itemCollection.InsertItem(boundedBuffer.Consume());
	}

	return NULL;
}


//Creates the quanity of producer threads specified by the parameter
void StartProducerThreads (int numThreads)
{
	pthread_t newThread;
	
	for(int i = 0; i < numThreads; i++)
	{
        /* Each producer will create a fraction of the total items (ITEM_COUNT),
           hence, ITEM_COUNT/numThreads. End statistics should report each 
           producer thread produced the same number of items. */
		pthread_create(&newThread, NULL, ProducerThread,
                       CreateItems(ITEM_COUNT/numThreads));
        
		producer_threads.push_back(newThread); // Add to producer_threads
	}
}

//Creates the quanity of consumer threads specified by the parameter
void StartConsumerThreads (int numThreads)
{
	
	for(int i = 0; i < numThreads; i++)
	{
		pthread_t newThread;
		pthread_create(&newThread, NULL, ConsumerThread, NULL);
		consumer_threads.push_back(newThread); // Add to consumer_threads
	}
}

//Joins all threads in the passed in list
void JoinThreads (list<pthread_t> threads)
{
	for(pthread_t thread : threads)
	{
		pthread_join(thread, NULL);
	}
}

//Cancels all threads
void CancelThreads (list<pthread_t> threads)
{
	for (pthread_t thread : threads)
	{
		pthread_cancel(thread);
	}
}
	

int main(int argc, char **argv)
{
	int num_producers = 4;
	int num_consumers = 5;
	
	
	StartProducerThreads (num_producers); //Creates the producer threads
	StartConsumerThreads (num_consumers); //Creates the consumer threads
	
	//Blocks thread until all producer threads have produced all their items
	JoinThreads(producer_threads);

	// Do not proceed until all items have been consumed
	while (go)
	{
		if (itemCollection.NumberOfItems() ==
            (ITEM_COUNT/num_producers)*num_producers)
		{
			go = false;
			
			//Forcibly cancels the consumer threads
			CancelThreads(consumer_threads);
		}
	}
	
	//Print the item statistics to the terminal
	itemCollection.PrintItems();
	
	//Prevent the terminal from closing. 
	cout << "Press enter to exit." << endl;
	cin.get();

	return 0;
}
