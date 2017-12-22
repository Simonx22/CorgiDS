/*
    CorgiDS Copyright PSISP 2017
    Licensed under the GPLv3
    See LICENSE.txt for details
*/

#include <chrono>
#include "config.hpp"
#include "emuthread.hpp"

using namespace std;

EmuThread::EmuThread(QObject* parent) : QThread(parent)
{
    pause_status = 0x1;
}

int EmuThread::init()
{
    return e.init();
}

int EmuThread::load_firmware()
{
    mutex.lock();
    int error = e.load_firmware();
    mutex.unlock();
    return error;
}

int EmuThread::load_game(QString ROM_name)
{
    mutex.lock();
    int error = e.load_ROM(ROM_name.toStdString());
    mutex.unlock();
    return error;
}

void EmuThread::run()
{
    abort = false;
    QMutex emu_mutex;
    long long max_us_count = 1000000 / 60;
    long long second_count = 1000000;
    int frames = 0;
    auto FPS_update = chrono::system_clock::now();
    forever
    {
        if (abort)
            return;
        if (pause_status)
            usleep(1000); //TODO: better way to handle pause than sleeping?
        else
        {
            emu_mutex.lock();
            auto last_update = chrono::system_clock::now();
            e.run();
            frames++;
            e.get_upper_frame(upper_buffer);
            e.get_lower_frame(lower_buffer);
            emit finished_frame(upper_buffer, lower_buffer);
            auto now = chrono::system_clock::now();
            std::chrono::duration<float> diff = now - last_update;
            auto us = chrono::duration_cast<chrono::microseconds>(diff).count();
            if (Config::enable_framelimiter && us < max_us_count)
            {
                this->usleep(max_us_count - us);
            }
            emu_mutex.unlock();

            diff = now - FPS_update;
            us = chrono::duration_cast<chrono::microseconds>(diff).count();
            if (us >= second_count)
            {
                emit update_FPS(frames);
                FPS_update = chrono::system_clock::now();
                frames = 0;
            }
        }
    }
}

void EmuThread::shutdown()
{
    mutex.lock();
    abort = true;
    mutex.unlock();
}

void EmuThread::pause(PAUSE_EVENT event)
{
    mutex.lock();
    pause_status |= 1 << event;
    mutex.unlock();
}

void EmuThread::unpause(PAUSE_EVENT event)
{
    mutex.lock();
    pause_status &= ~(1 << event);
    mutex.unlock();
}

//Unsure if mutexes here are necessary
void EmuThread::press_key(DS_KEYS key)
{
    mutex.lock();
    switch (key)
    {
        case BUTTON_LEFT:
            e.button_left_pressed();
            break;
        case BUTTON_RIGHT:
            e.button_right_pressed();
            break;
        case BUTTON_UP:
            e.button_up_pressed();
            break;
        case BUTTON_DOWN:
            e.button_down_pressed();
            break;
        case BUTTON_A:
            e.button_a_pressed();
            break;
        case BUTTON_B:
            e.button_b_pressed();
            break;
        case BUTTON_X:
            e.button_x_pressed();
            break;
        case BUTTON_Y:
            e.button_y_pressed();
            break;
        case BUTTON_L:
            e.button_l_pressed();
            break;
        case BUTTON_R:
            e.button_r_pressed();
            break;
        case BUTTON_START:
            e.button_start_pressed();
            break;
        case BUTTON_SELECT:
            e.button_select_pressed();
            break;
        case DEBUGGING:
            e.debug();
            break;
    }
    mutex.unlock();
}

void EmuThread::release_key(DS_KEYS key)
{
    mutex.lock();
    switch (key)
    {
        case BUTTON_LEFT:
            e.button_left_released();
            break;
        case BUTTON_RIGHT:
            e.button_right_released();
            break;
        case BUTTON_UP:
            e.button_up_released();
            break;
        case BUTTON_DOWN:
            e.button_down_released();
            break;
        case BUTTON_A:
            e.button_a_released();
            break;
        case BUTTON_B:
            e.button_b_released();
            break;
        case BUTTON_X:
            e.button_x_released();
            break;
        case BUTTON_Y:
            e.button_y_released();
            break;
        case BUTTON_L:
            e.button_l_released();
            break;
        case BUTTON_R:
            e.button_r_released();
            break;
        case BUTTON_START:
            e.button_start_released();
            break;
        case BUTTON_SELECT:
            e.button_select_released();
            break;
    }
    mutex.unlock();
}

void EmuThread::touchscreen_event(int x, int y)
{
    mutex.lock();
    e.touchscreen_press(x, y);
    mutex.unlock();
}
