#include <chrono>
#include <iostream>
#include <random>
#include "TrafficLight.h"

/******************************************************************************
 *  Message Queue Class */

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _condition.wait(lock, [this] {return !_queue.empty();});
    T message = std::move(_queue.front());
    _queue.pop_front()

    return message;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lock (_mutex);
    _queue.emplace_back(msg);
    _condition.notify_one();
}


/******************************************************************************
 *  Traffic Light Class */

TrafficLight::TrafficLight() :
_currentPhase { TrafficLightPhase::red }
{}

void TrafficLight::waitForGreen()
{
    while (true)
    {
        TrafficLightPhase lightPhase = _messageQueue.receive();
        if (lightPhase == TrafficLightPhase::green)
        {
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate() override
{
    threads.emplace_back(std::thread(cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    auto previousUpdate = std::chrono::system_clock::now();
    int cycleTime = GetRandomNumberInRange(4000, 6000);

    while (true)
    {
        size_t timeSinceUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - previousUpdate).count();
        
        if (timeSinceUpdate > cycleTime)
        {
            switch(_currentPhase)
            {
                case TrafficLightPhase::red : _currentPhase = TrafficLightPhase::green;
                break;
                case TrafficLightPhase::green : _currentPhase = TrafficLightPhase::red;
                break;
            }
            _messageQueue.send(std::move(_currentPhase));
            cycleTime = 4000 + std::rand() % 6000;
            previousUpdate = std::chrono::system_clock::now();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int TrafficLight::GetRandomNumberInRange(int lowerBound, int upperBound)
{
    std::mt19937 randomize{ std::random_device{}() };
    std::uniform_int_distribution number{ lowerBound, upperBound};
    return number(randomize);
}