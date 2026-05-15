#include "CollisionHeightmap.hpp"
#include <GL/gl.h>
#include <cmath>

CollisionHeightmap::CollisionHeightmap(int nx, int nz, float step, float width, float depth) 
    : numX(nx), numZ(nz), cellSize(step) 
{
    offsetX = width / 2.0f;
    offsetZ = depth / 2.0f;
    
    heights.resize(numX * numZ, -2.5f); 
    normals.resize(numX * numZ, glm::vec3(0, 1, 0));

    for (int z = 0; z < numZ; ++z) {
        for (int x = 0; x < numX; ++x) {
            float worldX = x * cellSize - offsetX;
            float worldZ = z * cellSize - offsetZ;
            heights[z * numX + x] = -2.5f + 0.3f * std::sin(worldX) * std::cos(worldZ); 
        }
    }

    for (int z = 0; z < numZ; ++z) {
        for (int x = 0; x < numX; ++x) {
            float lewy  = (x > 0) ? heights[z * numX + (x - 1)] : heights[z * numX + x];
            float prawy = (x < numX - 1) ? heights[z * numX + (x + 1)] : heights[z * numX + x];
            float gora  = (z > 0) ? heights[(z - 1) * numX + x] : heights[z * numX + x];
            float dol   = (z < numZ - 1) ? heights[(z + 1) * numX + x] : heights[z * numX + x];

            float df_dx = (prawy - lewy) / (2.0f * cellSize);
            float df_dz = (dol - gora) / (2.0f * cellSize);

            glm::vec3 n(-df_dx, 1.0f, -df_dz);
            normals[z * numX + x] = glm::normalize(n);
        }
    }
}

void CollisionHeightmap::render() {
    glColor3f(0.6f, 0.5f, 0.4f); 

    glBegin(GL_TRIANGLES);
    for (int z = 0; z < numZ - 1; ++z) {
        for (int x = 0; x < numX - 1; ++x) {
            int i00 = z * numX + x;
            int i10 = z * numX + (x + 1);
            int i01 = (z + 1) * numX + x;
            int i11 = (z + 1) * numX + (x + 1);

            glm::vec3 p00(x * cellSize - offsetX, heights[i00], z * cellSize - offsetZ);
            glm::vec3 p10((x + 1) * cellSize - offsetX, heights[i10], z * cellSize - offsetZ);
            glm::vec3 p01(x * cellSize - offsetX, heights[i01], (z + 1) * cellSize - offsetZ);
            glm::vec3 p11((x + 1) * cellSize - offsetX, heights[i11], (z + 1) * cellSize - offsetZ);

            glNormal3fv(&normals[i00][0]); glVertex3fv(&p00[0]);
            glNormal3fv(&normals[i10][0]); glVertex3fv(&p10[0]);
            glNormal3fv(&normals[i01][0]); glVertex3fv(&p01[0]);

            glNormal3fv(&normals[i10][0]); glVertex3fv(&p10[0]);
            glNormal3fv(&normals[i11][0]); glVertex3fv(&p11[0]);
            glNormal3fv(&normals[i01][0]); glVertex3fv(&p01[0]);
        }
    }
    glEnd();
}

float CollisionHeightmap::getInterpolatedHeight(float x, float z) const {
    float gridX = (x + offsetX) / cellSize;
    float gridZ = (z + offsetZ) / cellSize;

    int i = (int)gridX;
    int j = (int)gridZ;

    if (i < 0) i = 0; if (i >= numX - 1) i = numX - 2;
    if (j < 0) j = 0; if (j >= numZ - 1) j = numZ - 2;

    float u = gridX - i;
    float w = gridZ - j;

    float h00 = heights[j * numX + i];
    float h10 = heights[j * numX + (i + 1)];
    float h01 = heights[(j + 1) * numX + i];
    float h11 = heights[(j + 1) * numX + (i + 1)];

    return (1.0f - u) * (1.0f - w) * h00 + u * (1.0f - w) * h10 + (1.0f - u) * w * h01 + u * w * h11;
}

glm::vec3 CollisionHeightmap::getInterpolatedNormal(float x, float z) const {
    float gridX = (x + offsetX) / cellSize;
    float gridZ = (z + offsetZ) / cellSize;

    int i = (int)gridX;
    int j = (int)gridZ;

    if (i < 0) i = 0; if (i >= numX - 1) i = numX - 2;
    if (j < 0) j = 0; if (j >= numZ - 1) j = numZ - 2;

    float u = gridX - i;
    float w = gridZ - j;

    glm::vec3 n00 = normals[j * numX + i];
    glm::vec3 n10 = normals[j * numX + (i + 1)];
    glm::vec3 n01 = normals[(j + 1) * numX + i];
    glm::vec3 n11 = normals[(j + 1) * numX + (i + 1)];

    glm::vec3 interp = (1.0f - u) * (1.0f - w) * n00 + u * (1.0f - w) * n10 + (1.0f - u) * w * n01 + u * w * n11;
    return glm::normalize(interp); 
}

void CollisionHeightmap::RayCollisionInternal(float delta, float time, ray& r, ray_collision& rc) {
    glm::vec3 p0 = r.position;
    glm::vec3 p1 = p0 + r.velocity * delta;

    float h0 = getInterpolatedHeight(p0.x, p0.z);
    float h1 = getInterpolatedHeight(p1.x, p1.z);

    if (p0.y >= h0 && p1.y < h1) {
        float gap0 = p0.y - h0;
        float gap1 = p1.y - h1;
        float fraction = gap0 / (gap0 - gap1); 

        glm::vec3 hit_point = p0 + r.velocity * (delta * fraction);
        
        glm::vec3 n = getInterpolatedNormal(hit_point.x, hit_point.z);
        float dot_product = glm::dot(r.velocity, n);
        r.velocity = r.velocity - n * (2.0f * dot_product);

        rc.has_collided = true;
        rc.distance = glm::length(r.velocity * (delta * fraction));
    }
}