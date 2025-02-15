#ifndef MOTION_H
#define MOTION_H

#include <cmath>
#include <atomic>
#include <cmath>
#include <chrono>
#include <iostream>
#include <thread>
#include <memory>
#include <unistd.h>
#include <Task/Periodic.h>
#include <QApplication>
#include <fstream> // For file handling

    class MC_MoveVelocity
{
public:
    bool ENO{};
    short unsigned int Axis{};
    bool InVelocity{};
    bool Busy{};
    bool Active{};
    bool CommandAborted{};
    bool Error{};
    uint16_t ErrorID{};

    void mc_move_velocity(short unsigned int Axis, bool Execute = false, bool ContinuousUpdate = false,
                          float Velocity = 0.0, float Acceleration = 0.0, float Deceleration = 0.0, float Jerk = 0.0,
                          short unsigned int Direction = 0, short unsigned int BufferMode = 0, bool EN = true);

    void *mc_move_velocity_exec();

private:
    Task::Periodic m_Task;
    bool Execute_previous{};
    float direction{1.0};
    bool first_scan{1};
    bool motionExecuted{};
    float tempPosition{};
    float targetPosition{};
    float t1{};
    float t2{};
    float t3{};
    float t4{};
    float t5{};
    float deltaTime{};
    float currentVelocity{};
    float currentAcceleration{};
    float currentDeceleration{};
    float accelerationTime{};
    float decelerationTime{};
    std::chrono::system_clock::time_point begin;
    std::chrono::system_clock::time_point currentTime;
    std::chrono::system_clock::time_point previousTime;
    // std::chrono::system_clock::time_point deltaTime;
    bool Deceleration_start{};
};

class MC_MoveAbsolute
{
public:
    short unsigned int Axis;
    bool ENO;
    bool Done;
    bool Busy;
    bool Active;
    bool CommandAborted;
    bool Error;
    uint16_t ErrorID;
    void mc_move_absolute(short unsigned int axis, bool Execute = false, bool ContinuousUpdate = false, float Position = 0.0, float Velocity = 0.0, float Acceleration = 0.0, float Deceleration = 0.0, float Jerk = 0.0, short unsigned int Direction = 0, short unsigned int BufferMode = 0, bool EN = true);
    void *mc_move_absolute_exec();
    std::vector<double> positionData;

private:
    Task::Periodic m_Task;
    bool Execute_previous{};
    float direction{1.0};
    bool first_scan{1};
    bool motionExecuted{};
    unsigned short int stage{};
    float tempPosition{};
    float targetPosition{};
    float desiredPosition{};
    float bufferDistance{};
    float remainedDistance{};
    float buffertime{};
    float t1{};
    float t2{};
    float t3{};
    float t4{};
    float t5{};
    float t6{};
    float deltaTime{};
    float deltaVelocity{};
    float deltaAcceleration{};
    float deltaDeceleration{};
    float currentPosition{};
    float currentVelocity{};
    float currentAcceleration{};
    float JerkLimit;
    float MaxAcceleration;
    float MaxDeceleration;
    float DesiredVelocity;
    float accelerationTime{};
    float decelerationTime{};
    float previousTime{};
    float ratio{};
    float oneoverRatio{};
    std::chrono::system_clock::time_point begin;
    bool Acceleration_start{};
    bool Deceleration_start{};
    bool AccelerationCompleted{};
};


#endif // MOTION_H