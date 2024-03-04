using System;
using System.Collections.Generic;
using System.Threading;

// Class to represent the traffic signal data
class TrafficSignalData
{
    public DateTime Timestamp { get; set; } // Timestamp of the data
    public int TrafficLightId { get; set; } // ID of the traffic light
    public int CarsPassed { get; set; } // Number of cars that passed the traffic light
}

class TrafficControl
{
    private const int BufferSize = 1; // Size of the buffer
    private const int NumberOfProducers = 7; // Number of producer threads
    private const int NumberOfConsumers = 4; // Number of consumer threads

    private static Queue<TrafficSignalData> buffer = new Queue<TrafficSignalData>(BufferSize); // Buffer to store the data
    private static SortedList<int, int> congestionData = new SortedList<int, int>(); // Data to store congestion information

    private static object bufferLock = new object(); // Lock for the buffer
    private static object congestionDataLock = new object(); // Lock for the congestion data

    private static ManualResetEvent producerEvent = new ManualResetEvent(false); // Event to signal when data is available for consumers
    private static ManualResetEvent consumerEvent = new ManualResetEvent(false); // Event to signal when there is space available in the buffer for producers

    static void Main()
    {
        // Create producer threads
        Thread[] producerThreads = new Thread[NumberOfProducers];
        for (int i = 0; i < NumberOfProducers; i++)
        {
            producerThreads[i] = new Thread(ProducerThread);
            producerThreads[i].Start();
        }

        // Create consumer threads
        Thread[] consumerThreads = new Thread[NumberOfConsumers];
        for (int i = 0; i < NumberOfConsumers; i++)
        {
            consumerThreads[i] = new Thread(ConsumerThread);
            consumerThreads[i].Start();
        }

        // Wait for all producer threads to finish
        for (int i = 0; i < NumberOfProducers; i++)
        {
            producerThreads[i].Join();
        }

        // Notify consumers that no more data will be produced
        producerEvent.Set();

        // Wait for all consumer threads to finish
        for (int i = 0; i < NumberOfConsumers; i++)
        {
            consumerThreads[i].Join();
        }
    }

    static void ProducerThread()
    {
        Random rand = new Random();
        for (int i = 0; i < 12; i++) // Produce 12 data items
        {
            TrafficSignalData newData = new TrafficSignalData
            {
                Timestamp = DateTime.Now.AddMinutes(5 * i), // Generate a timestamp for the data
                TrafficLightId = rand.Next(1, 11), // Generate a random traffic light ID between 1 and 10
                CarsPassed = rand.Next(1, 101) // Generate a random number of cars passed between 1 and 100
            };

            lock (bufferLock) // Lock the buffer to prevent race conditions
            {
                while (buffer.Count == BufferSize) // Wait if the buffer is full
                {
                    Monitor.Wait(bufferLock);
                }

                buffer.Enqueue(newData); // Add the data to the buffer
                producerEvent.Set(); // Signal that data is available for consumers
                Monitor.Pulse(bufferLock); // Notify a waiting thread that the buffer has been modified
            }

            Thread.Sleep(rand.Next(500, 1000)); // Simulate some delay before producing the next data
        }
    }

    static void ConsumerThread()
    {
        while (true) // Run indefinitely
        {
            producerEvent.WaitOne(); // Wait for data to be available from producers

            lock (bufferLock) // Lock the buffer to prevent race conditions
            {
                while (buffer.Count == 0) // Wait if the buffer is empty
                {
                    Monitor.Wait(bufferLock);
                }

                TrafficSignalData data = buffer.Dequeue(); // Get the data from the buffer
                consumerEvent.Set(); // Signal that there is space available in the buffer for producers
                Monitor.Pulse(bufferLock); // Notify a waiting thread that the buffer has been modified

                lock (congestionDataLock) // Lock the congestion data to prevent race conditions
                {
                    if (congestionData.ContainsKey(data.TrafficLightId)) // If the traffic light ID already exists in the congestion data
                    {
                        congestionData[data.TrafficLightId] += data.CarsPassed; // Update the number of cars passed for that traffic light ID
                    }
                    else
                    {
                        congestionData.Add(data.TrafficLightId, data.CarsPassed); // Add a new entry for the traffic light ID
                    }
                }

                Console.WriteLine($"Data - Time: {data.Timestamp}, LightId: {data.TrafficLightId}, CarsPassed: {data.CarsPassed}"); // Print the data

                Thread.Sleep(100); // Simulate some processing time
            }
        }
    }
    
}
