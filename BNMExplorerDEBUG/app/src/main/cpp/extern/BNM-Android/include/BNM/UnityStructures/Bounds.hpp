#pragma once

#include <string>
#include <cmath>
#include <algorithm>
#include "../UserSettings/GlobalSettings.hpp"
#include "Vector3.hpp"

namespace BNM::Structures::Unity {

    /**
        @brief Represents an axis aligned bounding box in 3D space.
    */
    struct Bounds {
        Vector3 m_Center{};
        Vector3 m_Extents{};

        inline constexpr Bounds() = default;
        inline constexpr Bounds(const Vector3 &center, const Vector3 &size)
            : m_Center(center), m_Extents(size * 0.5f) {}

        /**
            @brief The center of the bounding box.
            @return Center Vector3.
        */
        inline Vector3 GetCenter() const { return m_Center; }
        inline void SetCenter(const Vector3 &center) { m_Center = center; }

        /**
            @brief The total size of the box (extents * 2).
            @return Size Vector3.
        */
        inline Vector3 GetSize() const { return m_Extents * 2.0f; }
        inline void SetSize(const Vector3 &size) { m_Extents = size * 0.5f; }

        /**
            @brief The extents of the Bounding Box (half the size).
            @return Extents Vector3.
        */
        inline Vector3 GetExtents() const { return m_Extents; }
        inline void SetExtents(const Vector3 &extents) { m_Extents = extents; }

        /**
            @brief The minimal point of the box (center - extents).
            @return Min Vector3.
        */
        inline Vector3 GetMin() const { return m_Center - m_Extents; }
        inline void SetMin(const Vector3 &min) { SetMinMax(min, GetMax()); }

        /**
            @brief The maximal point of the box (center + extents).
            @return Max Vector3.
        */
        inline Vector3 GetMax() const { return m_Center + m_Extents; }
        inline void SetMax(const Vector3 &max) { SetMinMax(GetMin(), max); }

        /**
            @brief Sets the bounds to the min and max value of the box.
            @param min Minimum corner.
            @param max Maximum corner.
        */
        inline void SetMinMax(const Vector3 &min, const Vector3 &max) {
            m_Extents = (max - min) * 0.5f;
            m_Center = min + m_Extents;
        }

        /**
            @brief Grows the Bounds to include the point.
            @param point Vector3 point to encapsulate.
        */
        inline void Encapsulate(const Vector3 &point) {
            Vector3 curMin = GetMin();
            Vector3 curMax = GetMax();
            SetMinMax(
                Vector3(std::min(curMin.x, point.x), std::min(curMin.y, point.y), std::min(curMin.z, point.z)),
                Vector3(std::max(curMax.x, point.x), std::max(curMax.y, point.y), std::max(curMax.z, point.z))
            );
        }

        /**
            @brief Grows the Bounds to include another Bounds.
            @param bounds Bounds to encapsulate.
        */
        inline void Encapsulate(const Bounds &bounds) {
            Encapsulate(bounds.m_Center - bounds.m_Extents);
            Encapsulate(bounds.m_Center + bounds.m_Extents);
        }

        /**
            @brief Expand the bounds by increasing its size by amount along each side.
            @param amount Expansion amount.
        */
        inline void Expand(float amount) {
            amount *= 0.5f;
            m_Extents += Vector3(amount, amount, amount);
        }

        /**
            @brief Expand the bounds by increasing its size by amount along each side.
            @param amount Vector3 expansion amount.
        */
        inline void Expand(const Vector3 &amount) {
            m_Extents += amount * 0.5f;
        }

        /**
            @brief Is point contained in the bounding box?
            @param point Test point.
            @return True if inside bounds.
        */
        inline bool Contains(const Vector3 &point) const {
            Vector3 min = GetMin();
            Vector3 max = GetMax();
            return point.x >= min.x && point.x <= max.x &&
                   point.y >= min.y && point.y <= max.y &&
                   point.z >= min.z && point.z <= max.z;
        }

        /**
            @brief The closest point on the bounding box to the specified point.
            @param point Target point.
            @return Closest point on the bounding box.
        */
        inline Vector3 ClosestPoint(const Vector3 &point) const {
            Vector3 minP = GetMin();
            Vector3 maxP = GetMax();
            return {
                std::clamp(point.x, minP.x, maxP.x),
                std::clamp(point.y, minP.y, maxP.y),
                std::clamp(point.z, minP.z, maxP.z)
            };
        }

        /**
            @brief Does another bounding box intersect with this bounding box?
            @param bounds Test bounds.
            @return True if intersects.
        */
        inline bool Intersects(const Bounds &bounds) const {
            Vector3 minA = GetMin();
            Vector3 maxA = GetMax();
            Vector3 minB = bounds.GetMin();
            Vector3 maxB = bounds.GetMax();
            return minA.x <= maxB.x && maxA.x >= minB.x &&
                   minA.y <= maxB.y && maxA.y >= minB.y &&
                   minA.z <= maxB.z && maxA.z >= minB.z;
        }

        inline bool operator==(const Bounds &other) const {
            return m_Center == other.m_Center && m_Extents == other.m_Extents;
        }
        inline bool operator!=(const Bounds &other) const = default;
    };
}
