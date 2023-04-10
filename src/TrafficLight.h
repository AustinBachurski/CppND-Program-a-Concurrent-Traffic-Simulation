#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include <random>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

enum class TrafficLightPhase
{
    red,
    green,
};

/******************************************************************************
 *  Message Queue Class */

template <class T>
class MessageQueue
{
public:
    T receive();
    void send(T&& phase);

private:
    std::condition_variable _condition;
    std::deque<T> _queue;
    std::mutex _queueMutex;
};


/******************************************************************************
 *  Traffic Light Class */


class TrafficLight : public TrafficObject
{
public:
    TrafficLight();
    ~TrafficLight();
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();


private:
    std::condition_variable _condition;
    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _messageQueue;
    std::mutex _mutex;
    void cycleThroughPhases();
    int GetRandomNumberInRange(int lowerBound, int upperBound);
};

#endif