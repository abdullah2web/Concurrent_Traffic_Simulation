#include <iostream>
#include <random>
#include <mutex>
#include <condition_variable>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] { return !_messages.empty(); });
    T msg = std::move(_messages.back());
    _messages.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> uLock(_mutex);
    _messages.push_back(std::move(msg));
    _cond.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    
    while (true)
    {
        auto curr_phase = msg_queue_->receive();
        if (curr_phase == green) return;
    }
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. 
    // To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases,this));

}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.

    std::random_device rand;
    std::mt19937 mt(rand());
    std::uniform_real_distripution<float> cycleDuration(4000, 6000);
    auto t1 = std::chrono::high_resolution_clock::now();
    float time_cycle = cycleDuration(mt);
    
    while (true) {
        auto t2 = std::chrono::high_resolution_clock::now();
        def_t = t2 - t1;
        if (def_t.count() > cycleDuration) {
            t1 = std::chrono::high_resolution_clock::now();
            time_cycle = cycleDuration(mt);
            _currentPhase = (_currentPhase == TrafficLightPhase::green) ? red : green;
            
            TrafficLightPhase phase = _currentPhase;
            msg_queue_.send(std::move(phase));
        }
        
    }
}
