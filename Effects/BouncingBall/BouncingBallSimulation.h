#ifndef BOUNCINGBALLSIMULATION_H
#define BOUNCINGBALLSIMULATION_H

#include <unordered_set>
#include <random>
#include <QPoint>

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
        unsigned int dropHeightPercent,
        unsigned int Brightness,
        int Temperature,
        int Tint
        );

    static float GetGravity(int value);

    void StepEffect();
    void DetectSizesChanges();

    int GetWidth();
    int GetHeight();

    void SetFps(unsigned int);
    void SetBrightness(unsigned int);
    void SetTemperature(int);
    void SetTint(int);
    void SetWidth(unsigned int);
    void SetHeight(unsigned int);
    void SetRadius(unsigned int);
    void SetGravity(unsigned int);
    void SetDropHeightPercent(unsigned int);
    void SetHorizontalVelocity(unsigned int);
    void SetSpectrumVelocity(unsigned int);

private:
    float getTimeDelta(unsigned int fps);
    float getDropHeight(unsigned int percentDropHeight, unsigned int height);
    float getImpactVelocity(float acceleration, float dropHeight);
    float getHorizontalVelocity(int value);
    void  calculatePointsInCircle(int radius);
    void  initSimulation();

    std::mutex lockObj;

    ControllerZone* controllerZone;

    std::default_random_engine rng;
    std::uniform_int_distribution<> rndX;
    std::uniform_int_distribution<> rndBool;

    hsv_t baseColor;
    float hueDegrees = 0;

    float x, y,     // position
          dx, dy,   // velocity
          ddx, ddy, // acceleration
          dt;       // time step    
    float impactVelocity;

    unsigned int fps;
    unsigned int Brightness;
    int Temperature=0;
    int Tint=0;
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
