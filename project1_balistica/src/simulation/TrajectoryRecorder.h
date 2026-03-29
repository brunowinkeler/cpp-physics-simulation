#ifndef TRAJECTORY_RECORDER_H
#define TRAJECTORY_RECORDER_H

#include <optional>
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
        void record(float x, float y, float time, float speed);

        void clear();

        const std::vector<TrajectoryPoint> &getPoints() const;

        std::optional<TrajectoryPoint> getApexPoint() const;

    private:
        static double evaluateQuadratic(double time,
                                        float time0, float value0,
                                        float time1, float value1,
                                        float time2, float value2);

        static double evaluateQuadraticDerivative(double time,
                                                  const TrajectoryPoint &point0,
                                                  const TrajectoryPoint &point1,
                                                  const TrajectoryPoint &point2);

        std::vector<TrajectoryPoint> points;
    };
} // namespace physim

#endif // TRAJECTORY_RECORDER_H
