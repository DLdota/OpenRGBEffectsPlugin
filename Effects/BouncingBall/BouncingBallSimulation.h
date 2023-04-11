#ifndef BOUNCINGBALLSIMULATION_H
#define BOUNCINGBALLSIMULATION_H

#include <unordered_set>
#include <random>
#include <QPoint>

#include "ColorUtils.h"
#include "hsv.h"
#include "ControllerZone.h"

class BouncingBallSimulation
{
public:
    BouncingBallSimulation(
        ControllerZone* controllerZone,
        unsigned int fps,
        unsigned int radius,
        unsigned int gravity,
        unsigned int horizontalVelocity,
        unsigned int spectrumVelocity,
        unsigned int dropHeightPercent);

    static float GetGravity(int value);

    void StepEffect();

    int GetWidth();
    int GetHeight();

    void SetFps(unsigned int fps);
    void SetBrightness(unsigned int Brightness);
    void SetWidth(unsigned int width);
    void SetHeight(unsigned int height);
    void SetRadius(unsigned int radius);
    void SetGravity(unsigned int gravity);
    void SetDropHeightPercent(unsigned int dropHeightPercent);
    void SetHorizontalVelocity(unsigned int horizontalVelocity);
    void SetSpectrumVelocity(unsigned int spectrumVelocity);

private:
    float getTimeDelta(unsigned int fps);
    float getDropHeight(unsigned int percentDropHeight, unsigned int height);
    float getImpactVelocity(float acceleration, float dropHeight);
    float getHorizontalVelocity(int value);
    void  calculatePointsInCircle(int radius);
    void  initSimulation();

    std::mutex lockObj;

    zone_type zoneType;
    ControllerZone* controllerZone;
    unsigned int zoneIndex;
    unsigned int startIndex;

    std::default_random_engine rng;
    std::uniform_int_distribution<> rndX;
    std::uniform_int_distribution<> rndBool;

    const RGBColor off = ColorUtils::OFF();
    hsv_t baseColor;
    float hueDegrees = 0;

    float x, y,     // position
          dx, dy,   // velocity
          ddx, ddy, // acceleration
          dt;       // time step    
    float impactVelocity;

    unsigned int fps;
    float Brightness;
    int width;
    int height;
    int numLeds;
    float radius;
    int dropHeightPercent;
    int spectrumVelocity;

    std::unordered_set<unsigned int> oldLedIds;
    std::unordered_set<unsigned int> newLedIds;
    std::vector<QPoint> pointsInBall;
    std::vector<unsigned char> pointBrightness;
};

#endif // BOUNCINGBALLSIMULATION_H
