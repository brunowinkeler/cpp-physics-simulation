#ifndef COLLISION_GRID_H
#define COLLISION_GRID_H

#include "GasMath.h"

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

namespace physim
{
    class CollisionGrid
    {
    public:
        void configure(float boxWidth, float boxHeight, float desiredCellSize)
        {
            const float clampedCellSize = std::max(desiredCellSize, 1.0e-3f);
            const int newColumnCount = std::max(1, static_cast<int>(std::ceil(boxWidth / clampedCellSize)));
            const int newRowCount = std::max(1, static_cast<int>(std::ceil(boxHeight / clampedCellSize)));

            const bool layoutChanged = std::abs(cellSize - clampedCellSize) > 1.0e-6f ||
                                       newColumnCount != columnCount ||
                                       newRowCount != rowCount;

            cellSize = clampedCellSize;
            columnCount = newColumnCount;
            rowCount = newRowCount;

            if (layoutChanged)
            {
                cells.assign(static_cast<std::size_t>(columnCount * rowCount), {});
                return;
            }

            clear();
        }

        void clear()
        {
            for (std::vector<int> &cell : cells)
            {
                cell.clear();
            }
        }

        void insert(const GasVector2 &position, int particleIndex)
        {
            const auto [column, row] = getCellCoordinates(position);
            cells[static_cast<std::size_t>(row * columnCount + column)].push_back(particleIndex);
        }

        std::pair<int, int> getCellCoordinates(const GasVector2 &position) const
        {
            const int column = std::clamp(static_cast<int>(position.x / cellSize), 0, columnCount - 1);
            const int row = std::clamp(static_cast<int>(position.y / cellSize), 0, rowCount - 1);
            return {column, row};
        }

        const std::vector<int> &getCell(int column, int row) const
        {
            static const std::vector<int> EMPTY_CELL;
            if (column < 0 || column >= columnCount || row < 0 || row >= rowCount)
            {
                return EMPTY_CELL;
            }

            return cells[static_cast<std::size_t>(row * columnCount + column)];
        }

    private:
        float cellSize{1.0f};
        int columnCount{1};
        int rowCount{1};
        std::vector<std::vector<int>> cells;
    };
} // namespace physim

#endif // COLLISION_GRID_H
