#ifndef TRAJECTORY_RECORDER_H
#define TRAJECTORY_RECORDER_H

#include <cstddef>
#include <optional>
#include <vector>

namespace physim
{
    enum class TrajectoryRetentionMode
    {
        CompactHistory = 0,
        RollingWindow,
    };

    struct TrajectoryPoint
    {
        float x;
        float y;
        float time;
        float speed;
    };

    struct TrajectoryRetentionPolicy
    {
        std::size_t maxRecordedPoints;
        float minRecordedTimeStep;
        TrajectoryRetentionMode mode{TrajectoryRetentionMode::CompactHistory};
    };

    class TrajectoryRecorder
    {
    public:
        static constexpr std::size_t DEFAULT_MAX_RECORDED_POINTS = 4096;
        static constexpr float DEFAULT_MIN_RECORDED_TIME_STEP = 1.0f / 240.0f;

        TrajectoryRecorder() = default;
        explicit TrajectoryRecorder(const TrajectoryRetentionPolicy &retentionPolicy);

        void record(float x, float y, float time, float speed, bool forceSample = false);
        void setRetentionPolicy(const TrajectoryRetentionPolicy &retentionPolicy);

        void clear();

        const std::vector<TrajectoryPoint> &getPoints() const;
        const TrajectoryRetentionPolicy &getRetentionPolicy() const { return retentionPolicy; }

        std::optional<TrajectoryPoint> getApexPoint() const;

    private:
        void retainWithinBudget();
        void compactRecordedPoints();
        void discardOldestRecordedPoints();
        bool shouldAppendPoint(const TrajectoryPoint &point, bool forceSample) const;
        static TrajectoryRetentionPolicy sanitizeRetentionPolicy(const TrajectoryRetentionPolicy &retentionPolicy);

        static double evaluateQuadratic(double time,
                                        float time0, float value0,
                                        float time1, float value1,
                                        float time2, float value2);

        static double evaluateQuadraticDerivative(double time,
                                                  const TrajectoryPoint &point0,
                                                  const TrajectoryPoint &point1,
                                                  const TrajectoryPoint &point2);

        std::vector<TrajectoryPoint> points;
        TrajectoryRetentionPolicy retentionPolicy{DEFAULT_MAX_RECORDED_POINTS,
                                                  DEFAULT_MIN_RECORDED_TIME_STEP,
                                                  TrajectoryRetentionMode::CompactHistory};
    };
} // namespace physim

#endif // TRAJECTORY_RECORDER_H
