#pragma once
#include "Components.hpp"

class collision_manifold
{
public:
    collision_manifold(const glm::vec3 normal, const float penetration_depth, c_transform& transform1,
                       c_transform& transform2, const object_collision_type type1, const object_collision_type type2)
        : penetration_depth_(penetration_depth), normal(normal), type1(type1), type2(type2), transform1_(transform1),
          transform2_(transform2)
    {
        //printf("Collision started between: %u %u\n", id1, id2);
        printf("Depth: %f\n", penetration_depth_);
        printf("Normal%f\n %f\n %f\n", normal.x, normal.y, normal.z);
        //send collision begin event

        //temp testing thing:::
    }
    ~collision_manifold()
    {
        //printf("Collision stopped between: %u %u\n", id1, id2);
        //send some type of collision end event
    }

    void resolve_collision() const
    {
        if (type1 == object_collision_type::DYNAMIC && type2 == object_collision_type::DYNAMIC)
        {
            resolve_dynamic_vs_dynamic();
        }
        else if (type1 == object_collision_type::DYNAMIC)
        {
            resolve_dynamic_vs_not_dynamic(transform1_);
        }
        else if (type2 == object_collision_type::DYNAMIC)
        {
            resolve_dynamic_vs_not_dynamic(transform2_);
        }
        // No action needed for STATIC vs STATIC
    }

    float penetration_depth_ = 0;
    glm::vec3 normal;

private:
    void resolve_dynamic_vs_dynamic() const
    {
        transform1_.position += normal * penetration_depth_ * 0.5f;
        transform2_.position -= normal * penetration_depth_ * 0.5f;
    }

    void resolve_dynamic_vs_not_dynamic(c_transform& dynamic_transform) const
    {
        dynamic_transform.position -= normal * penetration_depth_;
    }

    object_collision_type type1;
    object_collision_type type2;
    c_transform& transform1_;
    c_transform& transform2_;
};
