#include "BoundedBuffer.hpp"


BoundedBuffer::BoundedBuffer(): in(0), out(0), counter(0)
{
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&mutex, 0, 1);
}

void BoundedBuffer::Produce (BufferItem& item)
{
    // Surround critical section with semaphores
	sem_wait(&empty);
	sem_wait(&mutex);
		
        // Assign current thread (a producer) to this item
		item.sender_thread = pthread_self();
		buffer[in] = item; //Add item to buffer
		counter++;
		in = (in + 1) % BUFFER_SIZE; //Calculate next "in" index
	
    // Signal sempahores!
	sem_post(&mutex);
	sem_post(&full);
}

BufferItem BoundedBuffer::Consume ()
{
	
	BufferItem result; // Item to return
	
    // Surround critical section with semaphores
	sem_wait(&full);
	sem_wait(&mutex);
		
		result = buffer[out]; //Consume item
		counter--;
		out = (out + 1) % BUFFER_SIZE;
		
		// Assign current thread (a consumer) to this item
		result.receiver_thread = pthread_self();
	
    // Signal semaphores!
	sem_post(&mutex);
	sem_post(&empty);
	
	return result;
}

// Properly destory the sempahores
BoundedBuffer::~BoundedBuffer()
{
	sem_destroy(&full);
	sem_destroy(&empty);
	sem_destroy(&mutex);
}
