#pragma once

namespace Starsurge {
    template <typename... Ts>
    class TypeList {};

    template <typename... Ts>
    class Callback;

    template <typename... Ts, typename... Us>
    class Callback<TypeList<Ts...>, TypeList<Us...>> {
    private:
        std::function<void(Us..., void*)> function;
        void * data;
    public:
        std::tuple<Ts...> storage;

        void Register(std::function<void(Us..., void*)> t_function, void * t_data) {
            function = t_function;
            data = t_data;
        }

        void Call(Us... us) {
            function(us..., data);
        }
    };
}
