#include <chrono>
#include "../include/Timer.h"
#include "../include/Logging.h"

Starsurge::Timer& Starsurge::Timer::Inst() {
    static Timer instance;
    return instance;
}

void Starsurge::Timer::SetCallbackDelay(unsigned int id, double delay) {
    if (this->callbacks.find(id) == this->callbacks.end()) {
        Error("Tried to alter callback.");
        return;
    }
    this->callbacks[id] = delay;
}

void Starsurge::Timer::StopCallback(unsigned int id) {
    if (this->callbacks.find(id) == this->callbacks.end()) {
        Error("Tried to stop unknown callback.");
        return;
    }
    this->callbacks.erase(id);
}

double Starsurge::Timer::Now() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now - this->gameStart).count() / 1000000.0;
}

unsigned int Starsurge::Timer::StartTimer() {
    // Find the first available id.
    int id = 0;
    while (this->timers.find(id) != this->timers.end()) {
        id += 1;
    }

    this->timers[id] = std::chrono::steady_clock::now();
    return id;
}

double Starsurge::Timer::StopTimer(unsigned int id) {
    if (this->timers.find(id) == this->timers.end()) {
        Error("Tried to stop unknown timer.");
        return 0;
    }
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    double ret = std::chrono::duration_cast<std::chrono::microseconds>(now - this->timers[id]).count() / 1000000.0;
    this->timers.erase(id);
    return ret;
}

double Starsurge::Timer::DeltaTime() {
    return deltaTime;
}

void Starsurge::Timer::Start() {
    this->gameStart = std::chrono::steady_clock::now();
    this->lastFrame = this->gameStart;
}

void Starsurge::Timer::UpdateDeltaTime() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> dur = now - lastFrame;
    this->deltaTime = dur.count()  / 1000.0;
    lastFrame = now;
}
