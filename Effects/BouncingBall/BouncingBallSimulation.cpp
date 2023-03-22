#include "BouncingBallSimulation.h"

BouncingBallSimulation::BouncingBallSimulation(
    ControllerZone* controllerZone,
    unsigned int fps,
    unsigned int radius,
    unsigned int gravity,
    unsigned int horizontalVelocity,
    unsigned int spectrumVelocity,
    unsigned int dropHeightPercent):
    controllerZone(controllerZone)
{
    this->zoneIndex = controllerZone->zone_idx;
    this->zoneType = controllerZone->type();

    if (this->zoneType == ZONE_TYPE_MATRIX) {
        this->width = controllerZone->matrix_map_width();
        this->height = controllerZone->matrix_map_height();
    } else {
        this->height = controllerZone->leds_count();
        this->width = 1;
    }

    this->numLeds = width * height;
    this->fps = fps;
    this->dt = getTimeDelta(fps);
    this->radius = radius;
    calculatePointsInCircle(radius);
    this->dropHeightPercent = dropHeightPercent;
    this->dx = getHorizontalVelocity(horizontalVelocity);
    this->spectrumVelocity = spectrumVelocity;
    this->ddx = 0;
    this->ddy = GetGravity(gravity);

    rng = std::default_random_engine(std::random_device{}());
    rndX = std::uniform_int_distribution<>(0, width-1); // [min, max] inclusive
    rndBool = std::uniform_int_distribution<>(0,1);

    baseColor.hue = 0;
    baseColor.saturation = 255;
    baseColor.value = 255;

    initSimulation();
}

void BouncingBallSimulation::StepEffect()
{
    // Prevent UI components from updating simulation parameters during step effect calculations
    lockObj.lock();

    // Draw ball
    baseColor.hue = hueDegrees;
    newLedIds.clear();
    for (unsigned int i = 0; i < pointsInBall.size(); i++) {
        QPoint point = pointsInBall[i];
        int rx = point.x();
        int ry = point.y();
        int ledIndex = int(y + ry) * width + int(x + rx);

        // Skip points outside boundaries of LED matrix
        if (x + rx < 0 || x + rx >= width ||
            y + ry < 0 || y + ry >= height ||
            ledIndex < 0 || ledIndex >= numLeds) {
            continue;
        }

        // Apply brightness to pixel in proportion to distance of pixel from center of ball
        hsv_t pixelColor = baseColor;
        pixelColor.value = pointBrightness[i];

        // Set new LED color
        unsigned int ledId = zoneType == ZONE_TYPE_MATRIX
                ? controllerZone->controller->zones[zoneIndex].matrix_map->map[ledIndex]
                : ledIndex;
        controllerZone->SetLED(startIndex + ledId, RGBColor(hsv2rgb(&pixelColor)), Brightness);

        // Update new and old LED indices
        newLedIds.insert(ledId);
        oldLedIds.erase(ledId);
    }

    // Turn off LEDs no longer within radius of ball
    for (const unsigned int& oldLedId : oldLedIds) {
        controllerZone->SetLED(startIndex + oldLedId, off, Brightness);
    }

    // New LEDs changed this frame will be old LEDs next frame
    oldLedIds.swap(newLedIds);

    if (zoneType == ZONE_TYPE_MATRIX) {
        // Update horizontal velocity and position of ball
        float dxp = dx + ddx * dt;
        float xp = x + dxp * dt;

        if (xp < 0) {
            float percentOvershoot = xp / (xp - x);
            dx = -dxp;
            x = dx * dt * percentOvershoot;
        } else if (xp >= width) {
            float overshoot = xp - width - 1;
            float percentOvershoot = overshoot / (xp - x);
            dx = -dxp;
            x = width - 1 + dx * dt * percentOvershoot;
        } else {
            x += dx * dt;
        }
    }

    // Update vertical velocity and position of ball
    float dyp = dy + ddy * dt;
    float yp = y + dyp * dt;

    if (yp >= height) {
        float overshoot = yp - height - 1;
        float percentOvershoot = overshoot / (yp - y);
        dy = -impactVelocity + ddy * dt * percentOvershoot;
        y = height - 1 + dy * dt * percentOvershoot;
    } else {
        dy += ddy * dt;
        y += dy * dt;
    }

    // Update color
    hueDegrees += spectrumVelocity * dt;
    if (hueDegrees >= 360) {
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
    return this->width;
}

int BouncingBallSimulation::GetHeight()
{
    return this->height;
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
    for (int rx = -radius; rx <= radius; rx++) {
        for (int ry = -radius; ry <= radius; ry++) {
            if (rx * rx + ry * ry <= radius * radius) {
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
    controllerZone->SetAllZoneLEDs(off, Brightness);
}

void BouncingBallSimulation::SetBrightness(unsigned int Brightness)
{
    this->lockObj.lock();
    this->Brightness = Brightness;
    this->lockObj.unlock();
}

void BouncingBallSimulation::SetFps(unsigned int fps)
{
    this->lockObj.lock();
    this->fps = fps;
    this->dt = getTimeDelta(fps);
    this->lockObj.unlock();
}

void BouncingBallSimulation::SetWidth(unsigned int width)
{
    this->lockObj.lock();
    this->width = width;
    this->numLeds = width * height;
    calculatePointsInCircle(radius);
    rndX = std::uniform_int_distribution<>(0, width-1);
    initSimulation();
    this->lockObj.unlock();
}

void BouncingBallSimulation::SetHeight(unsigned int height)
{
    this->lockObj.lock();
    this->height = height;
    this->numLeds = width * height;
    calculatePointsInCircle(radius);
    initSimulation();
    this->lockObj.unlock();
}

void BouncingBallSimulation::SetRadius(unsigned int radius)
{
    this->lockObj.lock();
    this->radius = radius;
    calculatePointsInCircle(radius);
    this->lockObj.unlock();
}

void BouncingBallSimulation::SetGravity(unsigned int gravity)
{
    this->lockObj.lock();
    this->ddy = GetGravity(gravity);
    initSimulation();
    this->lockObj.unlock();
}

void BouncingBallSimulation::SetDropHeightPercent(unsigned int dropHeightPercent)
{
    this->lockObj.lock();
    this->dropHeightPercent = dropHeightPercent;
    initSimulation();
    this->lockObj.unlock();
}

void BouncingBallSimulation::SetHorizontalVelocity(unsigned int horizontalVelocity)
{
    this->lockObj.lock();
    this->dx = getHorizontalVelocity(horizontalVelocity);
    this->lockObj.unlock();
}

void BouncingBallSimulation::SetSpectrumVelocity(unsigned int spectrumVelocity)
{
    this->lockObj.lock();
    this->spectrumVelocity = spectrumVelocity;
    this->lockObj.unlock();
}
