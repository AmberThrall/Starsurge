#pragma once
#include <chrono>
#include <map>
#include <functional>
#include <thread>

// Time is measured in seconds.
namespace Starsurge {
    class Game;

    class Timer {
    public:
        static Timer& Inst();

        // Call 'callback' after 'delay' seconds. If 'repeat' is set, the timer resets and 'callback' is called again after 'delay' seconds.
        template <typename... Ts>
        unsigned int Callback(double delay, bool repeat, std::function<void(Ts...)> callback, Ts... ts) {
            // Find the first available id.
            int id = 0;
            while (this->callbacks.find(id) != this->callbacks.end()) {
                id += 1;
            }
            this->callbacks[id] = delay;

            // Start a thread up.
            std::thread t([=]() {
                while (true) {
                    if (this->callbacks.find(id) == this->callbacks.end()) return;
                    std::this_thread::sleep_for(std::chrono::milliseconds((int)std::round(this->callbacks[id] * 1000.0)));
                    if (this->callbacks.find(id) == this->callbacks.end()) return;
                    callback(ts...);
                    if (!repeat) return;
                }
            });
            t.detach();
            return id;
        }
        unsigned int Callback(double delay, bool repeat, std::function<void()> function) {
            return Callback<>(delay, repeat, function);
        }

        void SetCallbackDelay(unsigned int id, double delay);
        void StopCallback(unsigned int id);

        double Now();
        unsigned int StartTimer();
        double StopTimer(unsigned int id);

        double DeltaTime();
    private:
        void Start();
        void UpdateDeltaTime();

        double deltaTime;
        std::chrono::steady_clock::time_point lastFrame;
        std::chrono::steady_clock::time_point gameStart;
        std::map<unsigned int, std::chrono::steady_clock::time_point> timers;
        std::map<unsigned int, double> callbacks;

        friend class Game;
    };
}
