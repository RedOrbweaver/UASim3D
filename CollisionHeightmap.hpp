#ifndef COLLISION_HEIGHTMAP_HPP
#define COLLISION_HEIGHTMAP_HPP

#include "CollisionObject.hpp" 
#include <vector>
#include <glm/glm.hpp>

class CollisionHeightmap : public CollisionObject {
public:
    int numX, numZ;
    float cellSize;
    float offsetX, offsetZ;
    
    std::vector<float> heights;
    std::vector<glm::vec3> normals;

    CollisionHeightmap(int nx, int nz, float step, float width, float depth);

    virtual void RayCollisionInternal(float delta, float time, ray& r, ray_collision& rc) override;

    void render();

private:
    float getInterpolatedHeight(float x, float z) const;
    glm::vec3 getInterpolatedNormal(float x, float z) const;
};

#endif