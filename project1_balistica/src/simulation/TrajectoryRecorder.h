#ifndef TRAJECTORY_RECORDER_H
#define TRAJECTORY_RECORDER_H

#include <vector>

namespace physim
{
    struct TrajectoryPoint
    {
        float x;
        float y;
        float time;
        float speed;
    };

    class TrajectoryRecorder
    {
    public:
        void record(float x, float y, float time, float speed)
        {
            points.push_back({x, y, time, speed});
        }

        void clear()
        {
            points.clear();
        }

        const std::vector<TrajectoryPoint> &getPoints() const
        {
            return points;
        }

    private:
        std::vector<TrajectoryPoint> points;
    };
} // namespace physim

#endif // TRAJECTORY_RECORDER_H
