#include <iostream>
#include <fstream> // For file handling
#include <thread>  // For multithreading
#include <chrono>
#include <cmath>
#include <fcntl.h>   // For non-blocking input handling
#include <termios.h> // For terminal settings
#include <unistd.h>
#include <Task/Periodic.h>

#include "main.h"

void configureTerminalForNonBlockingInput()
{
    termios terminalSettings;
    tcgetattr(STDIN_FILENO, &terminalSettings);          // Get current terminal settings
    terminalSettings.c_lflag &= ~(ICANON | ECHO);        // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &terminalSettings); // Apply new settings
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);            // Make standard input non-blocking
}

// Function to check if the 'g' key is pressed
bool isGKeyPressed()
{
    char input;
    ssize_t bytesRead = read(STDIN_FILENO, &input, 1); // Non-blocking read
    return (bytesRead > 0 && input == 'g');
}

std::ofstream positionFile("positions.txt"); // Global file stream for saving positions

void *MC_MoveVelocity::mc_move_velocity_exec()
{
    // Print and save target position
    // std::cout << "targetPosition: " << targetPosition << std::endl;
    if (positionFile.is_open())
    {
        positionFile << targetPosition << std::endl; // Save target position to file
    }
    Busy = true;
    return NULL;
}

void MC_MoveVelocity::mc_move_velocity(short unsigned int axis, bool Execute, bool ContinuousUpdate,
                                       float Velocity, float Acceleration, float Deceleration,
                                       float Jerk, short unsigned int Direction, short unsigned int BufferMode, bool EN)
{
    if (EN)
    {
        Axis = axis;

        if (first_scan)
        {
            m_Task = Task::Periodic();
            m_Task.SetPeriod(1000); // Set to run periodically
            m_Task.Bind(std::bind(&MC_MoveVelocity::mc_move_velocity_exec, this));
            first_scan = false;
        }
        if (Execute > Execute_previous)
        {
            direction = (Direction == 0) ? -1.0 : 1.0;
            Busy = true;
            targetPosition = tempPosition;
            Deceleration_start = true;

            // Calculate the time intervals based on jerk, acceleration, and velocity
            t1 = (Acceleration / Jerk);     // Time to reach max acceleration (jerk limited)
            t2 = (Velocity / Acceleration) - t1; // Time to reach max velocity
            t3 = (Acceleration / Jerk);     // Time for jerk-controlled deceleration
            std::cout << "t1: " << t1 << std::endl;
            std::cout << "t2: " << t2 << std::endl;
            std::cout << "t3: " << t3 << std::endl;
            begin = std::chrono::system_clock::now();
            previousTime = begin;
            m_Task.Start();
        }

        if (Busy)
        {
            if (Execute)
            {
                currentTime = std::chrono::system_clock::now();
                accelerationTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - begin).count() * 0.001; // Convert to seconds
                deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime).count() * 0.001;

                if (accelerationTime <= t1)
                {
                    // Phase 1: Jerk-controlled acceleration
                    currentAcceleration += Jerk * deltaTime;
                    currentVelocity += currentAcceleration * deltaTime;
                    targetPosition += currentVelocity * deltaTime;
                    // std::cout << "t1 ----------" << std::endl;
                    // std::cout << "current acceleration " << currentAcceleration << std::endl;
                    // std::cout << "current velocity " << currentVelocity << std::endl;
                    // std::cout << "target pos " << targetPosition << std::endl;

                }
                else if (accelerationTime <= (t1 + t2))
                {
                    // Phase 2: Constant acceleration
                    currentAcceleration = Acceleration;
                    currentVelocity += currentAcceleration * deltaTime;
                    targetPosition += currentVelocity * deltaTime;
                    // maxAcceleration = currentAcceleration;
                    // std::cout << "t2 ----------" << std::endl;
                    // std::cout << "current acceleration " << currentAcceleration << std::endl;
                    // std::cout << "current velocity " << currentVelocity << std::endl;
                    // std::cout << "target pos " << targetPosition << std::endl;
                }
                else if (accelerationTime <= (t1 + t2 + t3))
                {
                    currentAcceleration -= Jerk * deltaTime;
                    currentVelocity += currentAcceleration * deltaTime;
                    targetPosition += currentVelocity * deltaTime;
                    // maxVelocity = currentVelocity;
                    // std::cout << "t3 ----------" << std::endl;
                    // std::cout << "current acceleration " << currentAcceleration << std::endl;
                    // std::cout << "current velocity " << currentVelocity << std::endl;
                    // std::cout << "target pos " << targetPosition << std::endl;
                }
                else
                {
                    // Phase 3: Constant velocity
                    // currentVelocity = Velocity * direction;
                    targetPosition += currentVelocity * deltaTime * direction;
                }

            }
            else
            {
                // Deceleration code (if needed)
                if (Deceleration_start)
                {
                    Deceleration_start = false;
                    begin = std::chrono::system_clock::now();
                    t1 = (Deceleration / Jerk);
                    t2 = (currentVelocity / Deceleration) - t1;
                    t3 = (Deceleration / Jerk);         // Time for jerk-controlled deceleration
                    // std::cout << "t1: " << t1 << std::endl;
                    // std::cout << "t2: " << t2 << std::endl;
                    // std::cout << "t3: " << t3 << std::endl;
                }
                currentTime = std::chrono::system_clock::now();
                decelerationTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - begin).count() * 0.001; // Convert to seconds
                deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime).count() * 0.001;

                if (decelerationTime <= t1 && decelerationTime <= accelerationTime)
                {
                    currentDeceleration -= Jerk * deltaTime;
                    currentVelocity += currentDeceleration * deltaTime;
                    targetPosition += currentVelocity * deltaTime;
                    // std::cout << "t1 ----------" << std::endl;
                    // std::cout << "current deceleration " << currentDeceleration << std::endl;
                    // std::cout << "current velocity " << currentVelocity << std::endl;
                    // std::cout << "target pos " << targetPosition << std::endl;
                }
                else if (decelerationTime <= (t1 + t2) && decelerationTime <= accelerationTime)
                {
                    currentDeceleration = -1 * Deceleration;
                    currentVelocity += currentDeceleration * deltaTime;
                    targetPosition += currentVelocity * deltaTime;
                    // std::cout << "t2 ----------" << std::endl;
                    // std::cout << "current velocity " << currentVelocity << std::endl;
                    // std::cout << "target pos " << targetPosition << std::endl;
                }
                else if (decelerationTime <= (t1 + t2 + t3) && decelerationTime <= accelerationTime)
                {
                    currentDeceleration += Jerk * deltaTime;
                    currentVelocity += currentDeceleration * deltaTime;
                    targetPosition += currentVelocity * deltaTime;
                    // std::cout << "t3 ----------" << std::endl;
                    // std::cout << "current deceleration " << currentDeceleration << std::endl;
                    // std::cout << "current velocity " << currentVelocity << std::endl;
                    // std::cout << "target pos " << targetPosition << std::endl;
                }
                else
                {
                    // std::cout << "motion ended" << std::endl;
                    Busy = false;
                    m_Task.Stop();
                }              

            }

            if (Execute_previous && !Execute)
            {
                begin = std::chrono::system_clock::now();
            }
        }

        Execute_previous = Execute;
        previousTime = currentTime;
        EN = ENO;
    }
}

int main()
{
    MC_MoveVelocity mc_move_velocity;

    configureTerminalForNonBlockingInput(); // Set up non-blocking input

    bool execute = false;

    while (true)
    {
        // Check if 'g' key is pressed and hold the execute flag true while it's pressed
        if (isGKeyPressed())
        {
            if (execute)
            {
                execute = false;
            }
            else
            {
                execute = true;
            }
        }

        mc_move_velocity.mc_move_velocity(0, execute, true, 1000.0, 10000.0, 10000.0, 100000.0, 1, 0, true);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}
