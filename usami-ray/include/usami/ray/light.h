#pragma once
#include "usami/memory/arena.h"
#include "usami/color.h"
#include "usami/ray/ray.h"

namespace usami::ray
{
    class Scene;
    class Primitive;
    class LightSample;

    enum class LightType
    {
        DeltaPoint,
        DeltaDirection,
        Infinite,
        Area,
    };

    class Light : public UsamiObject
    {
    protected:
        LightType type_;

    public:
        Light(LightType type) : type_(type)
        {
        }

        // TODO: adjust this interface
        virtual SpectrumRGB Eval(const Ray& ray) const = 0;

        virtual LightSample Sample(const IntersectionInfo& isect, const Point2f& u) const = 0;

        /**
         * Estimate total radient flux generated by the light source
         */
        virtual SpectrumRGB Power() const = 0;
    };

    class AreaLight : public Light
    {
    private:
        const Primitive* object_;

    public:
        AreaLight(const Primitive* object) : Light(LightType::Area), object_(object)
        {
        }

        const Primitive* GetPrimitive() const noexcept
        {
            return object_;
        }
    };

    class LightSample
    {
    private:
        // sampled incident direction
        Vec3f wi_;

        // sampled light source position
        Vec3f point_;

        // sampled incident radiance
        SpectrumRGB radiance_;

        // probability distribution of the point being sampled
        float pdf_;

        // type of light that is sampled
        LightType type_;

    public:
        LightSample(Vec3f wi, Vec3f point, Vec3f radiance, float pdf, LightType type)
            : wi_(wi), point_(point), radiance_(radiance), pdf_(pdf), type_(type)
        {
        }

        bool TestIllumination() const
        {
            return pdf_ != 0 && radiance_ != Vec3f(0.f);
        }

        bool TestVisibility(const Scene& scene, const IntersectionInfo& isect_obj,
                            Workspace& workspace) const;

        Ray GenerateTestRay(const Vec3f& p) const noexcept
        {
            return RayFromTo(point_, p);
        }

        Ray GenerateShadowRay(const Vec3f& p) const noexcept
        {
            return RayFromTo(p, point_);
        }

        Vec3f IncidentDirection() const noexcept
        {
            return wi_;
        }

        Vec3f Point() const noexcept
        {
            return point_;
        }

        float Pdf() const noexcept
        {
            return pdf_;
        }

        SpectrumRGB Radiance() const noexcept
        {
            return radiance_;
        }

        LightType Type() const noexcept
        {
            return type_;
        }
    };
} // namespace usami::ray
