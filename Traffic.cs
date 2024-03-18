#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;
using namespace std::chrono;

struct TrafficData 
{
    time_point<system_clock> timestamp;
    int lightId;
    int carsPassed;
};

queue<TrafficData> trafficQueue;
mutex queueMutex;
condition_variable cv;

void produceTrafficData(int numSignals) 
{
    while (true) 
    {
        for (int i = 1; i <= numSignals; i++) 
        {
            TrafficData data;
            data.timestamp = system_clock::now();
            data.lightId = i;
            data.carsPassed = rand() % 500;

            unique_lock<mutex> lock(queueMutex);
            trafficQueue.push(data);

            cv.notify_one();
        }

        this_thread::sleep_for(chrono::minutes(5));
    }
}

void consumeTrafficData(int numSignals) 
{
    while (true) 
    {
        unique_lock<mutex> lock(queueMutex);
        cv.wait(lock, [] { return !trafficQueue.empty(); });

        vector<TrafficData> trafficData;

        while (!trafficQueue.empty()) 
        {
            trafficData.push_back(trafficQueue.front());
            trafficQueue.pop();
        }

        lock.unlock();

        cout << "Data of the Traffic Lights" << endl;

        for (const auto& data : trafficData) 
        {
            time_t time = system_clock::to_time_t(data.timestamp);
            struct tm *tm = localtime(&time);
            char buffer[80];
            strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", tm);
            cout << "Timestamp: " << buffer << " | Light ID: " << data.lightId << " | Cars Passed: " << data.carsPassed << endl;
        }
        cout << endl;

        sort(trafficData.begin(), trafficData.end(), [](const TrafficData& first, const TrafficData& second) 
        {
            return first.carsPassed > second.carsPassed;
        });

        cout << "Top 3 traffic lights that are congested are as follows =>" << endl;

        for (int i = 0; i < min(3, (int)trafficData.size()); i++) 
        {
            time_t time = system_clock::to_time_t(trafficData[i].timestamp);
            struct tm *tm = localtime(&time);
            char buffer[80];
            strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", tm);
            cout << "Timestamp: " << buffer << " | Light ID: " << trafficData[i].lightId << " | Cars Passed: " << trafficData[i].carsPassed << endl;
        }

        this_thread::sleep_for(chrono::hours(1));
    }
}

int main() 
{
    int numSignals = 20;

    thread producer(produceTrafficData, numSignals);
    thread consumer(consumeTrafficData, numSignals);

    producer.join();
    consumer.join();

    return 0;
}
