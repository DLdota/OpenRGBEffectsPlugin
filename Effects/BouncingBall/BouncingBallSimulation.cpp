#include "BouncingBallSimulation.h"

BouncingBallSimulation::BouncingBallSimulation(
        ControllerZone* controllerZone,
        unsigned int fps,
        unsigned int radius,
        unsigned int gravity,
        unsigned int horizontalVelocity,
        unsigned int spectrumVelocity,
        unsigned int dropHeightPercent,
        unsigned int Brightness,
        int Temperature,
        int Tint
        ):
    controllerZone(controllerZone),
    fps(fps),
    Brightness(Brightness),
    Temperature(Temperature),
    Tint(Tint),
    radius(radius),
    dropHeightPercent(dropHeightPercent),
    spectrumVelocity(spectrumVelocity)
{
    if (controllerZone->type() == ZONE_TYPE_MATRIX)
    {
        width = controllerZone->matrix_map_width();
        height = controllerZone->matrix_map_height();
    }
    else
    {
        height = controllerZone->leds_count();
        width = 1;
    }

    numLeds = width * height;

    dt = getTimeDelta(fps);

    calculatePointsInCircle(radius);

    dx = getHorizontalVelocity(horizontalVelocity);
    ddx = 0;
    ddy = GetGravity(gravity);

    rng = std::default_random_engine(std::random_device{}());
    rndX = std::uniform_int_distribution<>(0, width-1); // [min, max] inclusive
    rndBool = std::uniform_int_distribution<>(0,1);

    baseColor.hue = 0;
    baseColor.saturation = 255;
    baseColor.value = 255;

    initSimulation();
}

void BouncingBallSimulation::DetectSizesChanges()
{
    if (controllerZone->type() == ZONE_TYPE_MATRIX) {
        // Manually check for on-the-fly changes in matrix width or height.
        // Width and height are not updated via slots through the UI.
        int newWidth = controllerZone->matrix_map_width();

        if (width != newWidth) {
            SetWidth(newWidth);
        }

        int newHeight = controllerZone->matrix_map_height();

        if (height != newHeight) {
            SetHeight(newHeight);
        }
    }
}

void BouncingBallSimulation::StepEffect()
{
    // Prevent UI components from updating simulation parameters during step effect calculations
    lockObj.lock();

    // Draw ball
    baseColor.hue = hueDegrees;
    newLedIds.clear();
    for (unsigned int i = 0; i < pointsInBall.size(); i++)
    {
        QPoint point = pointsInBall[i];
        int rx = point.x();
        int ry = point.y();
        int ledIndex = int(y + ry) * width + int(x + rx);

        // Skip points outside boundaries of LED matrix
        if (x + rx < 0 || x + rx >= width ||
                y + ry < 0 || y + ry >= height ||
                ledIndex < 0 || ledIndex >= numLeds)
        {
            continue;
        }

        // Apply brightness to pixel in proportion to distance of pixel from center of ball
        hsv_t pixelColor = baseColor;
        pixelColor.value = pointBrightness[i];

        // Set new LED color
        unsigned int ledId = controllerZone->type() == ZONE_TYPE_MATRIX
                ? controllerZone->map()[ledIndex]
                  : ledIndex;

        controllerZone->SetLED(ledId, RGBColor(hsv2rgb(&pixelColor)), Brightness, Temperature, Tint);

        // Update new and old LED indices
        newLedIds.insert(ledId);
        oldLedIds.erase(ledId);
    }

    // Turn off LEDs no longer within radius of ball
    for (const unsigned int& oldLedId : oldLedIds)
    {
        controllerZone->SetLED(oldLedId, 0, Brightness, Temperature, Tint);
    }

    // New LEDs changed this frame will be old LEDs next frame
    oldLedIds.swap(newLedIds);

    if (controllerZone->type() == ZONE_TYPE_MATRIX)
    {
        // Update horizontal velocity and position of ball
        float dxp = dx + ddx * dt;
        float xp = x + dxp * dt;

        if (xp < 0)
        {
            float percentOvershoot = xp / (xp - x);
            dx = -dxp;
            x = dx * dt * percentOvershoot;
        }
        else if (xp >= width)
        {
            float overshoot = xp - width - 1;
            float percentOvershoot = overshoot / (xp - x);
            dx = -dxp;
            x = width - 1 + dx * dt * percentOvershoot;
        }
        else
        {
            x += dx * dt;
        }
    }

    // Update vertical velocity and position of ball
    float dyp = dy + ddy * dt;
    float yp = y + dyp * dt;

    if (yp >= height)
    {
        float overshoot = yp - height - 1;
        float percentOvershoot = overshoot / (yp - y);
        dy = -impactVelocity + ddy * dt * percentOvershoot;
        y = height - 1 + dy * dt * percentOvershoot;
    }
    else
    {
        dy += ddy * dt;
        y += dy * dt;
    }

    // Update color
    hueDegrees += spectrumVelocity * dt;

    if (hueDegrees >= 360)
    {
        hueDegrees = 0;
    }

    lockObj.unlock();
}

float BouncingBallSimulation::GetGravity(int value)
{
    return value <= 10 ? value : (10 + pow(1.07, value));
}

int BouncingBallSimulation::GetWidth()
{
    return width;
}

int BouncingBallSimulation::GetHeight()
{
    return height;
}

// Used to make the physics framerate independent
float BouncingBallSimulation::getTimeDelta(unsigned int fps)
{
    return 1.0f/fps;
}

float BouncingBallSimulation::getDropHeight(unsigned int dropHeightPercent, unsigned int height)
{
    return dropHeightPercent / 100.0 * height;
}

// Solve projectile motion eqn: 0.5*m*v^2 = m*g*h
// to calculate impact velocity, used to reset projectile velocity every 'bounce'.
// This prevents floating point precision error accumulation over time.
float BouncingBallSimulation::getImpactVelocity(float gravity, float dropHeight)
{
    return sqrt(2 * gravity * dropHeight);
}

float BouncingBallSimulation::getHorizontalVelocity(int value)
{
    return value * (dx < 0 ? -1 : 1);
}

void BouncingBallSimulation::calculatePointsInCircle(int radius)
{
    pointsInBall.clear();
    pointBrightness.clear();

    for (int rx = -radius; rx <= radius; rx++)
    {
        for (int ry = -radius; ry <= radius; ry++)
        {
            if (rx * rx + ry * ry <= radius * radius)
            {
                double distance = sqrt(pow(rx, 2) + pow(ry, 2));
                unsigned char brightness = (1-distance/radius)*255;

                pointsInBall.push_back(QPoint(rx, ry));
                pointBrightness.push_back(brightness);
            }
        }
    }
}

void BouncingBallSimulation::initSimulation()
{
    float dropHeight = getDropHeight(dropHeightPercent, height-1);
    impactVelocity = getImpactVelocity(ddy, dropHeight);

    y = height - dropHeight;
    dy = 0;

    x = rndX(rng);
    dx = dx * (rndBool(rng) ? -1 : 1);

    oldLedIds.clear();
    newLedIds.clear();
    controllerZone->SetAllZoneLEDs(0, Brightness, Temperature, Tint);
}

void BouncingBallSimulation::SetBrightness(unsigned int value)
{
    lockObj.lock();
    Brightness = value;
    lockObj.unlock();
}

void BouncingBallSimulation::SetTemperature(int value)
{
    lockObj.lock();
    Temperature = value;
    lockObj.unlock();
}

void BouncingBallSimulation::SetTint(int value)
{
    lockObj.lock();
    Tint = value;
    lockObj.unlock();
}

void BouncingBallSimulation::SetFps(unsigned int value)
{
    lockObj.lock();
    fps = value;
    dt = getTimeDelta(fps);
    lockObj.unlock();
}

void BouncingBallSimulation::SetWidth(unsigned int value)
{
    lockObj.lock();
    width = value;
    numLeds = width * height;
    calculatePointsInCircle(radius);
    rndX = std::uniform_int_distribution<>(0, width-1);
    initSimulation();
    lockObj.unlock();
}

void BouncingBallSimulation::SetHeight(unsigned int value)
{
    lockObj.lock();
    height = value;
    numLeds = width * height;
    calculatePointsInCircle(radius);
    initSimulation();
    lockObj.unlock();
}

void BouncingBallSimulation::SetRadius(unsigned int value)
{
    lockObj.lock();
    radius = value;
    calculatePointsInCircle(radius);
    lockObj.unlock();
}

void BouncingBallSimulation::SetGravity(unsigned int value)
{
    lockObj.lock();
    ddy = GetGravity(value);
    initSimulation();
    lockObj.unlock();
}

void BouncingBallSimulation::SetDropHeightPercent(unsigned int value)
{
    lockObj.lock();
    dropHeightPercent = value;
    initSimulation();
    lockObj.unlock();
}

void BouncingBallSimulation::SetHorizontalVelocity(unsigned int value)
{
    lockObj.lock();
    dx = getHorizontalVelocity(value);
    lockObj.unlock();
}

void BouncingBallSimulation::SetSpectrumVelocity(unsigned int value)
{
    lockObj.lock();
    spectrumVelocity = value;
    lockObj.unlock();
}
