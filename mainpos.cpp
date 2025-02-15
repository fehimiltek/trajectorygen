
#include "motion.h"

float newtonCbrt(float x, int iterations = 10)
{
    float guess = x; // Initial guess
    for (int i = 0; i < iterations; i++)
    {
        guess = (2.0 * guess + x / (guess * guess)) / 3.0;
    }
    return guess;
}

unsigned int counter = 0;
void *MC_MoveAbsolute::mc_move_absolute_exec()
{    
    
    accelerationTime = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - begin).count()) * 0.000001f;
    deltaTime = accelerationTime - previousTime;


    if (stage == 10 && (currentAcceleration >= MaxAcceleration * 1.003))
    {
        stage = 20;
        currentAcceleration = MaxAcceleration;
    }
    else if (stage == 20 && ((DesiredVelocity - currentVelocity) <= ((currentAcceleration * t3) - (0.5 * JerkLimit * std::pow(t3, 2)) * 1.003)))
    {
        stage = 30;
    }
    else if (stage == 30 && (currentAcceleration <= 0.003))
    {
        stage = 40;
        currentAcceleration = 0;
        currentVelocity = DesiredVelocity;

    }
    else if (stage == 50 && (abs(currentAcceleration) * 1.003 > MaxDeceleration))
    {
        stage = 60;
    }
    else if (stage == 60 && ((currentVelocity * 1.003) < (-(currentAcceleration * t3) - (0.5 * JerkLimit * std::pow(t3, 2)))
    || (remainedDistance < (std::pow(currentAcceleration, 3) / (6 * std::pow(JerkLimit, 2))))))
    {
        stage = 70;
    }
    else if (stage == 70 && (currentVelocity <= 0))
    {
        std::cout << "velocity under zero" << std::endl;
        stage = 80;
    }


    switch (stage)
    {
    case 10:
        {
            deltaAcceleration = JerkLimit * deltaTime;
            currentAcceleration += deltaAcceleration;
            deltaVelocity =  currentAcceleration * deltaTime;
            currentVelocity += deltaVelocity;
            currentPosition += currentVelocity * deltaTime;
            bufferDistance = currentPosition ;
            counter++;
            std::cout << "t1 : " << "currentPosition " << currentPosition << " currentVelocity " << currentVelocity << " currentAcceleration " << currentAcceleration << " counter: " << counter << std::endl;
        }
        break;
    
    case 20:
        {
            deltaVelocity = currentAcceleration * deltaTime;
            currentVelocity += deltaVelocity;
            currentPosition += currentVelocity * deltaTime;
            bufferDistance = currentPosition;
            counter++;
            std::cout << "t2 : " << "currentPosition " << currentPosition << " currentVelocity " << currentVelocity << " currentAcceleration " << currentAcceleration << " counter: " << counter << std::endl;
        }
        break;
    
    case 30:
        {
            deltaAcceleration = JerkLimit * deltaTime;
            currentAcceleration -= deltaAcceleration;
            deltaVelocity = currentAcceleration * deltaTime;
            currentVelocity += deltaVelocity;
            currentPosition += currentVelocity * deltaTime;
            bufferDistance = currentPosition;
            counter++;
            std::cout << "t3 : " << "currentPosition " << currentPosition << " currentVelocity " << currentVelocity << " currentAcceleration " << currentAcceleration << " counter: " << counter << std::endl;
        }
        break;
    
    case 40:
        {
            currentPosition += currentVelocity * deltaTime;
            counter++;
            // std::cout << "t4 : " << "currentPosition " << currentPosition << " currentVelocity " << currentVelocity << " currentAcceleration " << currentAcceleration << " counter: " << counter << std::endl;
        }
        break;

    case 49:
        {
            currentPosition += currentVelocity * deltaTime;
            counter++;
            stage = 50;
        }
        break;
    case 50:
        {
            deltaDeceleration = JerkLimit * deltaTime;
            currentAcceleration -= deltaDeceleration;
            deltaVelocity = currentAcceleration * deltaTime;
            currentVelocity += deltaVelocity;
            currentPosition += currentVelocity * deltaTime;
            counter++;
            std::cout << "t5 : " << "currentPosition " << currentPosition << " currentVelocity " << currentVelocity << " currentAcceleration " << currentAcceleration << " counter: " << counter << std::endl;
        }
        break;

    case 60:
        {
            deltaVelocity = currentAcceleration * deltaTime;
            currentVelocity += deltaVelocity;
            currentPosition += currentVelocity * deltaTime;
            counter++;
            std::cout << "t6 : " << "currentPosition " << currentPosition << " currentVelocity " << currentVelocity << " currentAcceleration " << currentAcceleration << " counter: " << counter << std::endl;
        }
        break;

    case 70:
        {
            deltaDeceleration = JerkLimit * deltaTime;
            // (remainedDistance) > (-std::pow(currentAcceleration, 3) / (6 * std::pow(JerkLimit, 2)))
            // ? currentAcceleration = currentAcceleration : currentAcceleration += deltaDeceleration; 
            currentAcceleration += deltaDeceleration;
            std::cout << "remainedDistance " << remainedDistance << std::endl;
            std::cout << "compared " << -std::pow(currentAcceleration, 3) / (6 * std::pow(JerkLimit, 2)) << std::endl;
            deltaVelocity = currentAcceleration * deltaTime;
            currentVelocity += deltaVelocity;
            currentPosition += currentVelocity * deltaTime;
            counter++;
            std::cout << "t7 : " << "currentPosition " << currentPosition << " currentVelocity " << currentVelocity << " currentAcceleration " << currentAcceleration << " counter: " << counter << std::endl;
        }
        break;

    case 80:
        {
            Done = true;
            Busy = false;
        }
        break;

    }

    remainedDistance = desiredPosition - currentPosition;
    previousTime = accelerationTime;
    if (remainedDistance <= 0.003)
    {
        stage = 80;
        currentPosition = desiredPosition;
    }

    if (remainedDistance <= bufferDistance)
    {
        if (stage == 40)
        {
            stage = 50;
            counter = 0;
            previousTime = 0.0;
            begin = std::chrono::system_clock::now();
        }
        else if (stage == 10)
        {
            stage = 70;
            counter = 0;
            previousTime = 0.0;
            begin = std::chrono::system_clock::now();
        }
    }


    // if (stage == 1 && (currentAcceleration >= MaxAcceleration))
    // {
    //     stage = 2;
    // }
    // if (stage == 2 && (currentVelocity >= DesiredVelocity))
    // {
    //     stage = 3;
    // }
    // if (stage == 3 && (currentAcceleration <= 0))
    // {
    //     stage = 4;
    // }
    // if (stage == 5 && (abs(currentAcceleration) >= MaxDeceleration))
    // {
    //     stage = 6;
    // }
    // if (stage == 6 && (desiredPosition - currentPosition) < (bufferT3distance))
    // {
    //     stage = 7;
    // }
    // if (stage == 7 && (desiredPosition - currentPosition) < 0)
    // {
    //     stage = 8;
    // }
 

    targetPosition = tempPosition + (currentPosition * direction);

    positionData.push_back(targetPosition);
    // std::cout << "targetPosition " << targetPosition << " counter: " << counter << std::endl;

    return NULL;
}

void MC_MoveAbsolute::mc_move_absolute(short unsigned int axis, bool Execute, bool ContinuousUpdate,
                                       float Position, float Velocity, float Acceleration,
                                       float Deceleration, float Jerk, short unsigned int Direction,
                                       short unsigned int BufferMode, bool EN)
{

    if (EN)
    {
        Axis = axis;
        if (Done)
        {
            Done = false;
        }
        if (Execute && !Execute_previous)
        {

            if (first_scan)
            {
                m_Task = Task::Periodic();
                m_Task.SetPeriod(350);
                m_Task.Bind(std::bind(&MC_MoveAbsolute::mc_move_absolute_exec, this));
                first_scan = false;
            }
            Busy = true;
            Done = false;
            tempPosition = 0;
            (Position - tempPosition) > 0 ? direction = 1.0 : direction = -1.0;
            desiredPosition = std::abs(Position - tempPosition);
            currentPosition = 0.0;
            currentVelocity = 0.0;
            currentAcceleration = 0.0;
            buffertime = 0.0;
            bufferDistance = 0.0;
            accelerationTime = 0.0;
            deltaTime = 0.0;
            deltaAcceleration = 0.0;
            deltaVelocity = 0.0;
            deltaDeceleration = 0.0;
            JerkLimit = Jerk;
            MaxAcceleration = Acceleration;
            MaxDeceleration = Deceleration;
            DesiredVelocity = Velocity;
            stage = 10;
            t1 = (MaxAcceleration / Jerk);
            t2 = (DesiredVelocity / MaxAcceleration) - t1;
            t3 = (MaxAcceleration / Jerk);
            // std::cout << "t1 : " << t1 << " t2 : " << t2 << " t3 : " << t3 << " counter: " << counter << std::endl;
            previousTime = 0.0;
            positionData.clear(); // ✅ Clear previous data
            begin = std::chrono::system_clock::now();
            m_Task.Start();
        }


        if (ContinuousUpdate && (!Execute && Execute_previous))
        {
            stage = 49;
            previousTime = 0.0;
            begin = std::chrono::system_clock::now();
        }

        if (!Busy)
        {
            m_Task.Stop();
        }
    }
    Execute_previous = Execute;
    EN = ENO;
}

// int main()
// {
//     MC_MoveAbsolute mc_move_absolute;
//     truncatepos();
//     truncatevel();
//     truncateacc();
//     while (!mc_move_absolute.Done)
//     {
// //         std::cout << "mc_move_absolute done " <<  mc_move_absolute.Done << " counter: " << counter << std::endl;
//         mc_move_absolute.mc_move_absolute(0, true, false, 10, 100.0, 500.0, 500.0, 10000.0, 0, 0, true);
//         std::this_thread::sleep_for(std::chrono::milliseconds(1));
//     }
//     return 0;
// }
