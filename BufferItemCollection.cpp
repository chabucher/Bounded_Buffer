#include "BufferItemCollection.hpp"

BufferItemCollection::BufferItemCollection()
{
	pthread_mutex_init(&mutex, NULL); // Initialize mutex
}

BufferItemCollection::~BufferItemCollection()
{
	pthread_mutex_destroy(&mutex); // Terminate mutex
}

// Inserts item to list within a pthread_mutex monitor
void BufferItemCollection::InsertItem (BufferItem item)
{
	pthread_mutex_lock(&mutex);
        items.push_back(item);
    pthread_mutex_unlock(&mutex);
}

//Retrieves the size of list within a pthread_mutex monitor
int BufferItemCollection::NumberOfItems ()
{
	int count = 0;
	
	pthread_mutex_lock(&mutex);
        count = (int)items.size();
	pthread_mutex_unlock(&mutex);
	
	return count;
}

void BufferItemCollection::PrintThreadStats()
{
	// Use hash maps to tally up the collection's items by threads. The ulong is
    // a thread id that serves as a key to the int value, which is a tally.
	unordered_map<ulong, int> consumer_freq_map;
	unordered_map<ulong, int> producer_freq_map;
 
    for (auto item: items) 
	{
        consumer_freq_map[item.receiver_thread]++;
    }
	
	for (auto item: items)
	{
		producer_freq_map[item.sender_thread]++;
	}
 
	//Outputs the statistics to the terminal
	cout << "Producer Thread Stats" << endl;
	for (auto it: producer_freq_map)
		std::cout << it.first << " - " 
                  << it.second <<  '\n';
 
	cout << "Consumer Thread Stats" << endl;
    for (auto it: consumer_freq_map) {
        std::cout << it.first << " - " 
                  << it.second <<  '\n';
	}
}

// Prints the producer_id, consumer_id, and item_id of every item in the list
// within a pthread_mutex monitor
void BufferItemCollection::PrintItems ()
{
    pthread_mutex_lock(&mutex);
    
        cout << "Producer Thread, Consumer Thread, Item" << endl;
        
        for (BufferItem i : items)
        {
            cout << i.sender_thread << "," << i.receiver_thread
                 << "," << i._id << endl;
        }

        PrintThreadStats();
        cout << "Total responses: " << items.size() << endl;
    
	pthread_mutex_unlock(&mutex);
}
