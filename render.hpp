#pragma once
#include "hmain.hpp"

inline int printOversizedTriangles(Wave& wave, const Cuboid_dimensions& Cube, float maxArea)
{
    // te same sta�e co w refine
    const float cuboidHalfWidth = Cube.width * 0.5f;
    const float cuboidHalfHeight = Cube.height * 0.5f;
    const float cuboidHalfDepth = Cube.depth * 0.5f;
    const float safetyMargin = 0.2f;

    // por�wnujemy |cross|^2 > (2*maxArea)^2
    const float thr2 = 4.0f * maxArea * maxArea;

    int count = 0;
    for (const auto &t : wave.triangles)
    {
        int a = t.indices[0], b = t.indices[1], c = t.indices[2];
        const glm::vec3 &pa = wave.nodes[a].position;
        const glm::vec3 &pb = wave.nodes[b].position;
        const glm::vec3 &pc = wave.nodes[c].position;

        // pomijamy tr�jk�ty blisko �ciany (jak w refine)
        const bool nearWall =
            (std::fabs(pa.x) > cuboidHalfWidth - safetyMargin) ||
            (std::fabs(pa.y) > cuboidHalfHeight - safetyMargin) ||
            (std::fabs(pa.z) > cuboidHalfDepth - safetyMargin) ||
            (std::fabs(pb.x) > cuboidHalfWidth - safetyMargin) ||
            (std::fabs(pb.y) > cuboidHalfHeight - safetyMargin) ||
            (std::fabs(pb.z) > cuboidHalfDepth - safetyMargin) ||
            (std::fabs(pc.x) > cuboidHalfWidth - safetyMargin) ||
            (std::fabs(pc.y) > cuboidHalfHeight - safetyMargin) ||
            (std::fabs(pc.z) > cuboidHalfDepth - safetyMargin);

        if (nearWall)
            continue;

        // pole bez sqrt: |(pb-pa) x (pc-pa)|^2
        const glm::vec3 cr = glm::cross(pb - pa, pc - pa);
        const float cr2 = glm::dot(cr, cr);

        if (cr2 > thr2)
            ++count;
    }
    float procent = (float)count / (float)wave.triangles.size() * 100.0f;
    std::cout << "Oversized (splittable) triangles: "
              << wave.triangles.size() << "\n";
    return count;
}

inline void buildBuffersFor(const std::vector<node> &verts,
                     const std::vector<Triangle> &tris,
                     MeshGL &m,
                     bool dynamic)
{
    // if (!GLAD_GL_VERSION_1_5) { std::cerr << "VBO niedost�pne (GL < 1.5)\n"; return; }
    m.dynamic = dynamic;

    if (!m.vbo)
        glGenBuffers(1, &m.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);

    const GLsizeiptr vbSize = (GLsizeiptr)(verts.size() * sizeof(node));
    const GLenum usage = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
    glBufferData(GL_ARRAY_BUFFER, vbSize, verts.empty() ? nullptr : (const void *)verts.data(), usage);

    if (!m.ibo)
        glGenBuffers(1, &m.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ibo);

    const GLsizeiptr ibSize = (GLsizeiptr)(tris.size() * 3 * sizeof(unsigned int));
    const void *idxSrc = tris.empty() ? nullptr : (const void *)&tris[0].indices[0];
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibSize, idxSrc, GL_STATIC_DRAW);

    m.indexCount = (GLsizei)(tris.size() * 3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
} // bufory

inline void updatePositionsFor(const std::vector<node> &verts, MeshGL &m)
{
    if (!m.vbo)
        return;
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);

    const GLsizeiptr vbSize = (GLsizeiptr)(verts.size() * sizeof(node));
    glBufferData(GL_ARRAY_BUFFER, vbSize, nullptr, GL_DYNAMIC_DRAW);
    if (!verts.empty())
        glBufferSubData(GL_ARRAY_BUFFER, 0, vbSize, (const void *)verts.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

inline void drawCuboidTransparentSorted(glm::vec3 cameraPos, Cuboid_dimensions& temp_Cube)
{
    float halfWidth = temp_Cube.width / 2.0f;
    float halfHeight = temp_Cube.height / 2.0f;
    float halfDepth = temp_Cube.depth / 2.0f;

    glm::vec3 vertices[] = {
        {-halfWidth + temp_Cube.x_offset, -halfHeight + temp_Cube.y_offset, halfDepth + temp_Cube.z_offset},
        {halfWidth + temp_Cube.x_offset, -halfHeight + temp_Cube.y_offset, halfDepth + temp_Cube.z_offset},
        {halfWidth + temp_Cube.x_offset, halfHeight + temp_Cube.y_offset, halfDepth + temp_Cube.z_offset},
        {-halfWidth + temp_Cube.x_offset, halfHeight + temp_Cube.y_offset, halfDepth + temp_Cube.z_offset},
        {-halfWidth + temp_Cube.x_offset, -halfHeight + temp_Cube.y_offset, -halfDepth + temp_Cube.z_offset},
        {halfWidth + temp_Cube.x_offset, -halfHeight + temp_Cube.y_offset, -halfDepth + temp_Cube.z_offset},
        {halfWidth + temp_Cube.x_offset, halfHeight + temp_Cube.y_offset, -halfDepth + temp_Cube.z_offset},
        {-halfWidth + temp_Cube.x_offset, halfHeight + temp_Cube.y_offset, -halfDepth + temp_Cube.z_offset}};

    struct Face
    {
        int indices[4];
        float distance;
    };

    Face faces[6] = {
        {{0, 1, 2, 3}, 0.0f}, // prz�d
        {{4, 5, 6, 7}, 0.0f}, // ty�
        {{0, 3, 7, 4}, 0.0f}, // lewo
        {{1, 2, 6, 5}, 0.0f}, // prawo
        {{0, 1, 5, 4}, 0.0f}, // d�
        {{2, 3, 7, 6}, 0.0f}  // g�ra
    };

    // Oblicz odleg�o�ci �cian od kamery
    for (int i = 0; i < 6; ++i)
    {
        glm::vec3 center(0.0f);
        for (int j = 0; j < 4; ++j)
        {
            center += vertices[faces[i].indices[j]];
        }
        center /= 4.0f;
        faces[i].distance = glm::length(center - cameraPos);
    }

    // Sortuj �ciany od najdalszej do najbli�szej
    for (int i = 0; i < 5; ++i)
    {
        for (int j = i + 1; j < 6; ++j)
        {
            if (faces[i].distance < faces[j].distance)
            {
                std::swap(faces[i], faces[j]);
            }
        }
    }

    // transparentne sciany
    glBegin(GL_QUADS);
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            const glm::vec3 &v = vertices[faces[i].indices[j]];
            glVertex3f(v.x, v.y, v.z);
        }
    }
    glEnd();

    // KRAW�DZIE (drut)
    //  glColor4f(0.0f, 0.0f, 0.0f, 1.0f); // czarne, nieprzezroczyste
    glDisable(GL_DEPTH_TEST);
    glLineWidth(2.0f);

    static const int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // prz�d
        {4, 5},
        {5, 6},
        {6, 7},
        {7, 4}, // ty�
        {0, 4},
        {1, 5},
        {2, 6},
        {3, 7} // ��cz�ce
    };
    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    glBegin(GL_LINES);
    for (int e = 0; e < 12; ++e)
    {
        const glm::vec3 &v0 = vertices[edges[e][0]];
        const glm::vec3 &v1 = vertices[edges[e][1]];
        glVertex3f(v0.x, v0.y, v0.z);
        glVertex3f(v1.x, v1.y, v1.z);
    }
    glEnd();
}