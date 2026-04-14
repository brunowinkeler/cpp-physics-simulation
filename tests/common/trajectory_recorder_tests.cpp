#include "core/simulation/TrajectoryRecorder.h"

#include "gtest/gtest.h"

#include <cmath>
#include <cstddef>

TEST(TrajectoryRecorderTest, RetentionPolicyCapsGrowth)
{
    physim::TrajectoryRecorder trajectoryRecorder;
    const physim::TrajectoryRetentionPolicy retentionPolicy = trajectoryRecorder.getRetentionPolicy();
    const std::size_t totalSamples = retentionPolicy.maxRecordedPoints * 8;
    constexpr float sampleTimeStep = 0.001f;

    for (std::size_t index = 0; index < totalSamples; ++index)
    {
        const float time = static_cast<float>(index) * sampleTimeStep;
        const float x = static_cast<float>(index) * 0.25f;
        const float y = std::sin(time) * 20.0f;
        const bool forceSample = index == (totalSamples - 1);
        trajectoryRecorder.record(x, y, time, 10.0f + y, forceSample);
    }

    const auto &points = trajectoryRecorder.getPoints();
    bool strictlyIncreasingTime = true;
    for (std::size_t index = 1; index < points.size(); ++index)
    {
        if (points[index].time <= points[index - 1].time)
        {
            strictlyIncreasingTime = false;
            break;
        }
    }

    const double expectedFinalTime = static_cast<double>(totalSamples - 1) * sampleTimeStep;

    EXPECT_LE(points.size(), retentionPolicy.maxRecordedPoints)
        << "Trajectory recorder should stay within its configured point budget";
    EXPECT_GT(points.size(), 2u)
        << "Trajectory recorder should keep enough samples to draw a curve";
    EXPECT_NEAR(points.front().time, 0.0, 1.0e-6)
        << "Trajectory recorder should preserve the first sample";
    EXPECT_NEAR(points.back().time, expectedFinalTime, 1.0e-6)
        << "Trajectory recorder should preserve the latest sample";
    EXPECT_TRUE(strictlyIncreasingTime)
        << "Trajectory recorder should keep samples ordered after compaction";
}

TEST(TrajectoryRecorderTest, SamplingAccumulatesAcrossSkippedSteps)
{
    physim::TrajectoryRecorder trajectoryRecorder;
    const float minRecordedTimeStep = trajectoryRecorder.getRetentionPolicy().minRecordedTimeStep;

    trajectoryRecorder.record(0.0f, 0.0f, 0.0f, 0.0f, true);
    trajectoryRecorder.record(1.0f, 1.0f, minRecordedTimeStep * 0.45f, 1.0f);
    EXPECT_EQ(trajectoryRecorder.getPoints().size(), 1u)
        << "Trajectory recorder should skip samples below the minimum interval";

    trajectoryRecorder.record(2.0f, 2.0f, minRecordedTimeStep * 1.10f, 2.0f);
    EXPECT_EQ(trajectoryRecorder.getPoints().size(), 2u)
        << "Trajectory recorder should append once the minimum interval is reached";

    trajectoryRecorder.record(3.0f, 3.0f, minRecordedTimeStep * 1.55f, 3.0f);
    EXPECT_EQ(trajectoryRecorder.getPoints().size(), 2u)
        << "Trajectory recorder should keep the last committed point when skipping";

    trajectoryRecorder.record(4.0f, 4.0f, minRecordedTimeStep * 2.20f, 4.0f);
    const auto &points = trajectoryRecorder.getPoints();

    ASSERT_EQ(points.size(), 3u)
        << "Trajectory recorder should accumulate elapsed time across skipped samples";
    EXPECT_NEAR(points.back().time, static_cast<double>(minRecordedTimeStep * 2.20f), 1.0e-6)
        << "Trajectory recorder should append the next eligible sample";
}

TEST(TrajectoryRecorderTest, RollingWindowKeepsRecentSamplesDense)
{
    constexpr std::size_t maxRecordedPoints = 8;
    constexpr float sampleTimeStep = 0.05f;

    physim::TrajectoryRecorder trajectoryRecorder{{
        maxRecordedPoints,
        0.0f,
        physim::TrajectoryRetentionMode::RollingWindow,
    }};

    for (std::size_t index = 0; index < 20; ++index)
    {
        const float time = static_cast<float>(index) * sampleTimeStep;
        trajectoryRecorder.record(static_cast<float>(index),
                                  static_cast<float>(index) * 2.0f,
                                  time,
                                  1.0f,
                                  true);
    }

    const auto &points = trajectoryRecorder.getPoints();

    ASSERT_EQ(points.size(), maxRecordedPoints)
        << "Rolling window retention should keep only the configured number of recent samples";
    EXPECT_NEAR(points.front().time, 12.0 * sampleTimeStep, 1.0e-6)
        << "Rolling window retention should discard the oldest samples first";
    EXPECT_NEAR(points.back().time, 19.0 * sampleTimeStep, 1.0e-6)
        << "Rolling window retention should preserve the latest sample";

    for (std::size_t index = 1; index < points.size(); ++index)
    {
        EXPECT_NEAR(points[index].time - points[index - 1].time, sampleTimeStep, 1.0e-6)
            << "Rolling window retention should keep the original sampling density inside the retained window";
    }
}
