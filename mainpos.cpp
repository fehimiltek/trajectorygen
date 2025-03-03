#include "motion.h"
bool solveQuadratic(float a, float b, float c, float &x1, float &x2)
{
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
    {
        return false;
    }

    float sqrt_discriminant = std::sqrt(discriminant);
    float denom = 2 * a;

    x1 = (-b + sqrt_discriminant) / denom;
    x2 = (-b - sqrt_discriminant) / denom;

    return true;
}

unsigned int counter = 0;

void *MC_MoveAbsolute::mc_move_absolute_exec()
{

    accelerationTime = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - begin).count()) * 0.000001f;
    deltaTime = accelerationTime - previousTime;

    if (stage == 10 && (accelerationTime > t1))
    {
        stage = 20;
    }
    else if (stage == 20 && (accelerationTime > (t1 + t2)))
    {
        stage = 30;
    }
    else if (stage == 30 && (accelerationTime > (t1 + t2 + t3)))
    {
        stage = 40;
        currentAcceleration = 0;
    }
    else if (stage == 50 && (accelerationTime > t1))
    {
        stage = 60;
    }

    else if (stage == 60 && (accelerationTime > (t1 + t2)))
    {
        stage = 70;
    }
    else if (stage == 70 && ((accelerationTime > (t1 + t2 + t3)) || (currentVelocity <= 0)))
    {
        stage = 80;
    }

    switch (stage)
    {
    case 10:
    {
        deltaAcceleration = JerkLimit * deltaTime;
        currentAcceleration += deltaAcceleration;
        deltaVelocity = currentAcceleration * deltaTime;
        currentVelocity += deltaVelocity;
        currentPosition += currentVelocity * deltaTime;
        bufferDistance = currentPosition;
        counter++;
        // std::cout << "t1 : " << "currentPosition " << currentPosition << " currentVelocity " << currentVelocity << " currentAcceleration " << currentAcceleration << " counter: " << counter << std::endl;
    }
    break;

    case 20:
    {
        deltaVelocity = currentAcceleration * deltaTime;
        currentVelocity += deltaVelocity;
        currentPosition += currentVelocity * deltaTime;
        bufferDistance = currentPosition;
        counter++;
        // std::cout << "t2 : " << "currentPosition " << currentPosition << " currentVelocity " << currentVelocity << " currentAcceleration " << currentAcceleration << " counter: " << counter << std::endl;
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
        // std::cout << "t3 : " << "currentPosition " << currentPosition << " currentVelocity " << currentVelocity << " currentAcceleration " << currentAcceleration << " counter: " << counter << std::endl;
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
        // std::cout << "t5 : " << "currentPosition " << currentPosition << " currentVelocity " << currentVelocity << " currentAcceleration " << currentAcceleration << " counter: " << counter << std::endl;
    }
    break;

    case 60:
    {
        deltaVelocity = currentAcceleration * deltaTime;
        currentVelocity += deltaVelocity;
        currentPosition += currentVelocity * deltaTime;
        counter++;
        // std::cout << "t6 : " << "currentPosition " << currentPosition << " currentVelocity " << currentVelocity << " currentAcceleration " << currentAcceleration << " counter: " << counter << std::endl;
    }
    break;

    case 70:
    {
        deltaDeceleration = JerkLimit * deltaTime;
        currentAcceleration += deltaDeceleration;
        deltaVelocity = currentAcceleration * deltaTime;
        currentVelocity += deltaVelocity;
        currentPosition += currentVelocity * deltaTime;
        counter++;
        // std::cout << "t7 : " << "currentPosition " << currentPosition << " currentVelocity " << currentVelocity << " currentAcceleration " << currentAcceleration << " counter: " << counter << std::endl;
    }
    break;

    case 80:
    {
        Done = true;
        Busy = false;
    }
    break;

    case 90:
    {
        Error = true;
        ErrorID = 5; // ErrorID 5: Negative Velocity target not reached
        Busy = false;
    }
    break;
    }

    if (desiredPosition - currentPosition <= 0.001)
    {
        stage = 80;
    }
    if (currentVelocity < 0.0)
    {
        stage = 90;
    }

    else if ((stage == 40) && (desiredPosition - currentPosition <= bufferDistance))
    {
        stage = 49;
        counter = 0;
        accelerationTime = 0.0;
        begin = std::chrono::system_clock::now();
    }
    // std::cout << "acceleration time: " << accelerationTime << " stage: " << stage << std::endl;
    targetPosition = tempPosition + (currentPosition * direction);

    positionData.push_back(currentPosition);
    velocityData.push_back(currentVelocity);
    accelerationData.push_back(currentAcceleration);
    // std::cout << "targetPosition " << targetPosition << " counter: " << counter << std::endl;

    previousTime = accelerationTime;
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
            remainedDistance = 0.0;
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
            t2 = (DesiredVelocity - (MaxAcceleration * t1)) / MaxAcceleration;
            if (t2 < 0)
            {
                t2 = 0;
            }
            t3 = t1;

            float t1_distance = (0.1667 * JerkLimit * t1 * t1 * t1);
            float t2_distance = (0.5 * JerkLimit * t1 * t1 * t2) + (0.5 * MaxAcceleration * t2 * t2);
            float t3_distance = (0.5 * JerkLimit * t1 * t1 * t3) + (MaxAcceleration * t2 * t3) + (0.5 * MaxAcceleration * t3 * t3) - (0.1667 * JerkLimit * t3 * t3 * t3);

            if (desiredPosition < (2 * (t1_distance + t3_distance)))
            {
                t1 = std::cbrt(desiredPosition / (2 * JerkLimit));
                t2 = 0;
                t3 = t1;
            }

            else if (desiredPosition < (2 * (t1_distance + t2_distance + t3_distance)))
            {
                float x2 = (desiredPosition - (2 * (t1_distance + t3_distance))) * 0.5;
                float t2_1;
                float t2_2;
                bool solved = solveQuadratic((0.5 * MaxAcceleration), (0.5 * JerkLimit * t1 * t1), -x2, t2_1, t2_2);
                if (solved)
                {
                    t1 = t1;
                    t2 = std::max(t2_1, t2_2);
                    t3 = t1;
                }
                else
                {
                    std::cout << "No solution" << std::endl;
                }
            }

            positionData.clear();     // ✅ Clear previous data
            velocityData.clear();     // ✅ Clear previous data
            accelerationData.clear(); // ✅ Clear previous data
            accelerationTime = 0.0;
            previousTime = 0.0;
            begin = std::chrono::system_clock::now();
            periodicRunner.start(std::bind(&MC_MoveAbsolute::mc_move_absolute_exec, this), std::chrono::microseconds(350));
        }

        if (ContinuousUpdate && (!Execute && Execute_previous))
        {
            stage = 40;
            previousTime = 0.0;
            begin = std::chrono::system_clock::now();
        }

        if (!Busy)

        {
            std::cout << "Error: Axis is not busy" << std::endl;
        }
    }
    Execute_previous = Execute;
    EN = ENO;
}

void plotPositionData()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return;
    }

    GLFWwindow *window = glfwCreateWindow(800, 600, "Position Data Plot", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    glViewport(0, 0, 800, 600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, positionData.size(), -1.2, 1.2, -1.0, 1.0); // 2D projection

    // Normalize position data
    float minVal = *std::min_element(positionData.begin(), positionData.end());
    float maxVal = *std::max_element(positionData.begin(), positionData.end());

    std::vector<float> normalizedData;
    for (float pos : positionData)
    {
        float normalized = 2 * ((pos - minVal) / (maxVal - minVal)) - 1; // Normalize to [-1,1]
        normalizedData.push_back(normalized);
    }

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(1.0f, 1.0f, 1.0f); // White color for graph
        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < normalizedData.size(); ++i)
        {
            glVertex2f(i, normalizedData[i]); // Plot points
        }
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

int main()
{
    MC_MoveAbsolute motionController;

    motionController.Busy = true;
    std::cout << "Enter Target Position (or -1 to exit): ";
    float targetPosition;
    std::cin >> targetPosition;
    while (motionController.Busy)
    {
        if (targetPosition == -1)
            break;
        motionController.mc_move_absolute(0, true, false, targetPosition, 100.0, 1000.0, 1000.0, 20000.0, 0, 0, true);
    }

    plotPositionData();

    return 0;
}