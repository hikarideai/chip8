#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include "GLFW/glfw3.h"

struct Timer {
    double last, delay, initial;
    Timer() : last(glfwGetTime()), initial(glfwGetTime()) {}
    Timer(double delay) : last(glfwGetTime()), delay(delay), initial(glfwGetTime()) {}

    bool tick() {
        if (glfwGetTime() - last >= delay) {
            last = glfwGetTime();
            return true;
        }
        return false;
    }

    double elapsed() {
        return glfwGetTime() - last;
    }
    
    double existedFor() {
        return glfwGetTime() - initial;
    }

    double left() {
        return delay - (glfwGetTime() - last);
    }

    double flip() {
        auto temp = elapsed();
        refresh();
        return temp;
    }

    void refresh() {
        last = glfwGetTime();
    }

    void setDelay(double delay) {
        this->delay = delay;
        refresh();
    }
};

#endif // TIMER_H_INCLUDED
