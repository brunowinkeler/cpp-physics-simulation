#include "TrajectoryRecorder.h"

#include <algorithm>
#include <cmath>

namespace physim
{
    void TrajectoryRecorder::record(float x, float y, float time, float speed)
    {
        points.push_back({x, y, time, speed});
    }

    void TrajectoryRecorder::clear()
    {
        points.clear();
    }

    const std::vector<TrajectoryPoint> &TrajectoryRecorder::getPoints() const
    {
        return points;
    }

    std::optional<TrajectoryPoint> TrajectoryRecorder::getApexPoint() const
    {
        if (points.size() < 2)
        {
            return std::nullopt;
        }

        const auto apexIterator = std::max_element(
            points.begin(), points.end(),
            [](const TrajectoryPoint &left, const TrajectoryPoint &right)
            {
                return left.y < right.y;
            });

        const std::size_t apexIndex = static_cast<std::size_t>(apexIterator - points.begin());

        if (apexIndex == (points.size() - 1))
        {
            return std::nullopt;
        }

        if (apexIndex == 0 || points.size() < 3)
        {
            return points.front();
        }

        const TrajectoryPoint &previousPoint = points[apexIndex - 1];
        const TrajectoryPoint &apexSample = points[apexIndex];
        const TrajectoryPoint &nextPoint = points[apexIndex + 1];

        const double derivativeAtPrevious = evaluateQuadraticDerivative(previousPoint.time, previousPoint, apexSample, nextPoint);
        const double derivativeAtNext = evaluateQuadraticDerivative(nextPoint.time, previousPoint, apexSample, nextPoint);

        double apexTime = static_cast<double>(apexSample.time);
        if (std::abs(derivativeAtNext - derivativeAtPrevious) > 1.0e-9)
        {
            apexTime = static_cast<double>(previousPoint.time) -
                       (derivativeAtPrevious * static_cast<double>(nextPoint.time - previousPoint.time)) /
                           (derivativeAtNext - derivativeAtPrevious);
        }

        apexTime = std::clamp(
            apexTime,
            static_cast<double>(previousPoint.time),
            static_cast<double>(nextPoint.time));

        return TrajectoryPoint{
            static_cast<float>(evaluateQuadratic(apexTime, previousPoint.time, previousPoint.x, apexSample.time, apexSample.x, nextPoint.time, nextPoint.x)),
            static_cast<float>(evaluateQuadratic(apexTime, previousPoint.time, previousPoint.y, apexSample.time, apexSample.y, nextPoint.time, nextPoint.y)),
            static_cast<float>(apexTime),
            static_cast<float>(evaluateQuadratic(apexTime, previousPoint.time, previousPoint.speed, apexSample.time, apexSample.speed, nextPoint.time, nextPoint.speed))};
    }

    double TrajectoryRecorder::evaluateQuadratic(double time,
                                                 float time0, float value0,
                                                 float time1, float value1,
                                                 float time2, float value2)
    {
        const double basis0 = ((time - time1) * (time - time2)) /
                              ((static_cast<double>(time0) - time1) * (static_cast<double>(time0) - time2));
        const double basis1 = ((time - time0) * (time - time2)) /
                              ((static_cast<double>(time1) - time0) * (static_cast<double>(time1) - time2));
        const double basis2 = ((time - time0) * (time - time1)) /
                              ((static_cast<double>(time2) - time0) * (static_cast<double>(time2) - time1));

        return (basis0 * value0) + (basis1 * value1) + (basis2 * value2);
    }

    double TrajectoryRecorder::evaluateQuadraticDerivative(double time,
                                                           const TrajectoryPoint &point0,
                                                           const TrajectoryPoint &point1,
                                                           const TrajectoryPoint &point2)
    {
        const double derivative0 = ((2.0 * time) - point1.time - point2.time) /
                                   ((static_cast<double>(point0.time) - point1.time) * (static_cast<double>(point0.time) - point2.time));
        const double derivative1 = ((2.0 * time) - point0.time - point2.time) /
                                   ((static_cast<double>(point1.time) - point0.time) * (static_cast<double>(point1.time) - point2.time));
        const double derivative2 = ((2.0 * time) - point0.time - point1.time) /
                                   ((static_cast<double>(point2.time) - point0.time) * (static_cast<double>(point2.time) - point1.time));

        return (derivative0 * point0.y) + (derivative1 * point1.y) + (derivative2 * point2.y);
    }
} // namespace physim
