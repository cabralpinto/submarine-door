#include "shapes.hpp"

// class Shape

Shape::Shape()
    : color(ColorRGBA{1, 1, 1, 1}), ambient(ColorRGBA{1, 1, 1, 1}), diffuse(ColorRGBA{1, 1, 1, 1}), specular(ColorRGBA{1, 1, 1, 1}), shininess((GLfloat) 0) {}

CompoundShape *Shape::clone(int times, std::function<Shape *(int, Shape *)> transform) {
    std::vector<Shape *> clones(times);
    std::generate(clones.begin() + 1, clones.end(), [this]() { return this->clone(); });
    clones[0] = transform(0, this);
    for (int i = 1; i < times; ++i) clones[i] = transform(i, clones[i]);
    return new CompoundShape(clones);
}

Shape *Shape::setColor(ColorRGBA color) {
    this->color = color;
    return this;
}

Shape *Shape::setMaterial(DynamicValue<ColorRGBA> ambient, DynamicValue<ColorRGBA> diffuse, DynamicValue<ColorRGBA> specular, DynamicValue<GLfloat> shininess) {
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
    this->shininess = shininess;
    return this;
}

Shape *Shape::translate(Coordinates3D parameters) {
    transformations.push_back(std::shared_ptr<Transformation>(new Translation(parameters)));
    return this;
}

Shape *Shape::translate(std::function<void(Coordinates3D &)> getParameters) {
    transformations.push_back(std::shared_ptr<Transformation>(new Translation(getParameters)));
    return this;
}

Shape *Shape::rotate(Coordinates3D parameters) {
    transformations.push_back(std::shared_ptr<Transformation>(new Rotation(parameters)));
    return this;
}

Shape *Shape::rotate(std::function<void(Coordinates3D &)> getParameters) {
    transformations.push_back(std::shared_ptr<Transformation>(new Rotation(getParameters)));
    return this;
}

Shape *Shape::scale(Coordinates3D parameters) {
    transformations.push_back(std::shared_ptr<Transformation>(new Scale(parameters)));
    return this;
}

Shape *Shape::scale(std::function<void(Coordinates3D &)> getParameters) {
    transformations.push_back(std::shared_ptr<Transformation>(new Scale(getParameters)));
    return this;
}

void Shape::render() {
    glColor4f(color().r, color().g, color().b, color().a);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient().array);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse().array);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular().array);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess());
    if (transformations.size() > 0) {
        glPushMatrix();
        for (auto &transformation : transformations) transformation->execute();
        renderRaw();
        glPopMatrix();
    } else {
        renderRaw();
    }
}

// class SimpleShape : public Shape

void SimpleShape::createMesh(int level) {
    std::vector<GLfloat> mesh, textureMesh;
    mesh.resize(meshVertices.size() * 4);
    textureMesh.resize(meshTextureVertices.size() * 4);
    auto average = [](const auto &a, const auto &b) { return (a + b) / 2; };

    // vertices
    for (int i = 0; i < meshVertices.size(); i += 3) {
        std::copy(meshVertices.begin() + i, meshVertices.begin() + i + 3, mesh.begin() + (i - i / 12 * 3) * 5 + i / 12 * 3);
        std::transform(
            meshVertices.begin() + i,
            meshVertices.begin() + i + 3,
            meshVertices.begin() + i / 12 * 12 + (i + 3) % 12,
            mesh.begin() + ((i + 3) - (i + 3) / 12 * 3) * 5 + (i + 3) / 12 * 3 - 12,
            average);
        std::copy(
            mesh.begin() + ((i + 3) - (i + 3) / 12 * 3) * 5 + (i + 3) / 12 * 3 - 12,
            mesh.begin() + ((i + 3) - (i + 3) / 12 * 3) * 5 + (i + 3) / 12 * 3 - 9,
            mesh.begin() + ((i - i / 12 * 3) * 5 + i / 12 * 3) / 48 * 48 + ((i - i / 12 * 3) * 5 + i / 12 * 3 + 12) % 48);
    }
    for (int i = 0; i < meshVertices.size(); i += 3) {
        std::transform(
            mesh.begin() + ((i - i / 12 * 3) * 5 + i / 12 * 3) / 48 * 48 + 3,
            mesh.begin() + ((i - i / 12 * 3) * 5 + i / 12 * 3) / 48 * 48 + 6,
            mesh.begin() + ((i - i / 12 * 3) * 5 + i / 12 * 3) / 48 * 48 + 42,
            mesh.begin() + ((i + 6) - (i + 6) / 12 * 3) * 5 + (i + 6) / 12 * 3 - 24,
            average);
    }

    meshVertices.resize(mesh.size());
    std::copy(mesh.begin(), mesh.end(), meshVertices.begin());

    // normals
    for (int i = 0; i < meshNormals.size(); i += 3) {
        std::copy(meshNormals.begin() + i, meshNormals.begin() + i + 3, mesh.begin() + (i - i / 12 * 3) * 5 + i / 12 * 3);
        std::transform(
            meshNormals.begin() + i,
            meshNormals.begin() + i + 3,
            meshNormals.begin() + i / 12 * 12 + (i + 3) % 12,
            mesh.begin() + ((i + 3) - (i + 3) / 12 * 3) * 5 + (i + 3) / 12 * 3 - 12,
            average);
        std::copy(
            mesh.begin() + ((i + 3) - (i + 3) / 12 * 3) * 5 + (i + 3) / 12 * 3 - 12,
            mesh.begin() + ((i + 3) - (i + 3) / 12 * 3) * 5 + (i + 3) / 12 * 3 - 9,
            mesh.begin() + ((i - i / 12 * 3) * 5 + i / 12 * 3) / 48 * 48 + ((i - i / 12 * 3) * 5 + i / 12 * 3 + 12) % 48);
    }
    for (int i = 0; i < meshNormals.size(); i += 3) {
        std::transform(
            mesh.begin() + ((i - i / 12 * 3) * 5 + i / 12 * 3) / 48 * 48 + 3,
            mesh.begin() + ((i - i / 12 * 3) * 5 + i / 12 * 3) / 48 * 48 + 6,
            mesh.begin() + ((i - i / 12 * 3) * 5 + i / 12 * 3) / 48 * 48 + 42,
            mesh.begin() + ((i + 6) - (i + 6) / 12 * 3) * 5 + (i + 6) / 12 * 3 - 24,
            average);
    }

    meshNormals.resize(mesh.size());
    std::copy(mesh.begin(), mesh.end(), meshNormals.begin());

    // texture vertices
    // std::cout << textureVertices.size() << std::endl;
    // for (int i = 0; i < textureVertices.size(); i += 2) {
    //     std::copy(textureVertices.begin() + i, textureVertices.begin() + i + 2, textureMesh.begin() + (i - i / 12 * 2) * 5 + i / 12 * 2);
    //     std::transform(
    //         textureVertices.begin() + i,
    //         textureVertices.begin() + i + 2,
    //         textureVertices.begin() + i / 8 * 8 + (i + 2) % 8,
    //         textureMesh.begin() + ((i + 2) - (i + 2) / 8 * 2) * 5 + (i + 2) / 8 * 2 - 8,
    //         average);
    //     std::copy(
    //         textureMesh.begin() + ((i + 2) - (i + 2) / 8 * 2) * 5 + (i + 2) / 8 * 2 - 8,
    //         textureMesh.begin() + ((i + 2) - (i + 2) / 8 * 2) * 5 + (i + 2) / 8 * 2 - 6,
    //         textureMesh.begin() + ((i - i / 8 * 2) * 5 + i / 8 * 2) / 32 * 32 + ((i - i / 8 * 2) * 5 + i / 8 * 2 + 8) % 32);
    // }
    // for (int i = 0; i < textureVertices.size(); i += 2) {
    //     std::transform(
    //         textureMesh.begin() + ((i - i / 8 * 2) * 5 + i / 8 * 2) / 32 * 32 + 2,
    //         textureMesh.begin() + ((i - i / 8 * 2) * 5 + i / 8 * 2) / 32 * 32 + 4,
    //         textureMesh.begin() + ((i - i / 8 * 2) * 5 + i / 8 * 2) / 32 * 32 + 28,
    //         textureMesh.begin() + ((i + 4) - (i + 4) / 8 * 2) * 5 + (i + 4) / 8 * 2 - 16,
    //         average);
    // }

    // textureVertices.resize(textureMesh.size());
    // std::copy(textureMesh.begin(), textureMesh.end(), textureVertices.begin());

    if (level > 2) createMesh(level - 1);
}

void SimpleShape::renderRaw() {
    if (vertices.size() == 0) {
        vertices.resize(12 * getQuadCount());
        normals.resize(12 * getQuadCount());
        textureVertices.resize(8 * getQuadCount());
        generate();
        meshVertices = vertices;
        meshNormals = normals;
        meshTextureVertices = textureVertices;
        if (meshLevel > 1) createMesh(meshLevel);
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &(meshEnabled() ? meshVertices : vertices)[0]);
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, &(meshEnabled() ? meshNormals : normals)[0]);
    if (texture > 0) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexCoordPointer(2, GL_FLOAT, 0, &textureVertices[0]);
    }

    glDrawArrays(GL_QUADS, 0, (meshEnabled() ? meshVertices : vertices).size() / 3);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    if (texture > 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
};

SimpleShape::SimpleShape() : texture(0), meshLevel(1), meshEnabled(true) {}

SimpleShape *SimpleShape::setTexture(GLuint texture) {
    this->texture = texture;
    return this;
}

SimpleShape *SimpleShape::setMeshLevel(int meshLevel) {
    this->meshLevel = meshLevel;
    return this;
}

SimpleShape *SimpleShape::setMeshEnabled(DynamicValue<bool> meshEnabled) {
    this->meshEnabled = meshEnabled;
    return this;
}

// class Cuboid : public SimpleShape

void Cuboid::generate() {
    // half measures
    width /= 2;
    length /= 2;
    height /= 2;

    // build vertices array
    vertices = {
        // front face
        -width, -height, +length,  // bottom left
        +width, -height, +length,  // bottom right
        +width, +height, +length,  // top right
        -width, +height, +length,  // top left
        // back face
        -width, +height, -length,  // top left
        +width, +height, -length,  // top right
        +width, -height, -length,  // bottom right
        -width, -height, -length,  // bottom left
        // left face
        -width, +height, +length,  // top left
        -width, +height, -length,  // top right
        -width, -height, -length,  // bottom right
        -width, -height, +length,  // bottom left
        // right face
        +width, -height, +length,  // bottom left
        +width, -height, -length,  // bottom right
        +width, +height, -length,  // top right
        +width, +height, +length,  // top left
        // top face
        +width, +height, +length,  // bottom left
        +width, +height, -length,  // bottom right
        -width, +height, -length,  // top right
        -width, +height, +length,  // top left
        // bottom face
        -width, -height, +length,  // top left
        -width, -height, -length,  // top right
        +width, -height, -length,  // bottom right
        +width, -height, +length   // bottom left
    };

    normals = {
        // front face
        0, 0, 1,  // bottom left
        0, 0, 1,  // bottom right
        0, 0, 1,  // top right
        0, 0, 1,  // top left
        // back face
        0, 0, -1,  // top left
        0, 0, -1,  // top right
        0, 0, -1,  // bottom right
        0, 0, -1,  // bottom left
        // left face
        -1, 0, 0,  // top left
        -1, 0, 0,  // top right
        -1, 0, 0,  // bottom right
        -1, 0, 0,  // bottom left
        // right face
        1, 0, 0,  // bottom left
        1, 0, 0,  // bottom right
        1, 0, 0,  // top right
        1, 0, 0,  // top left
        // top face
        0, 1, 0,  // bottom left
        0, 1, 0,  // bottom right
        0, 1, 0,  // top right
        0, 1, 0,  // top left
        // bottom face
        0, -1, 0,  // top left
        0, -1, 0,  // top right
        0, -1, 0,  // bottom right
        0, -1, 0   // bottom left
    };

    textureVertices = {
        // front face
        1, 0,  // bottom left
        1, 1,  // bottom right
        0, 1,  // top right
        0, 0,  // top left
        // back face
        0, 0,  // top left
        0, 1,  // top right
        1, 1,  // bottom right
        1, 0,  // bottom left
        // left face
        0, 0,  // top left
        0, 1,  // top right
        1, 1,  // bottom right
        1, 0,  // bottom left
        // right face
        1, 0,  // bottom left
        1, 1,  // bottom right
        0, 1,  // top right
        0, 0,  // top left
        // top face
        1, 0,  // bottom left
        1, 1,  // bottom right
        0, 1,  // top right
        0, 0,  // top left
        // bottom face
        0, 0,  // top left
        0, 1,  // top right
        1, 1,  // bottom right
        1, 0,  // bottom left
    };
}

Cuboid::Cuboid(GLfloat width, GLfloat height, GLfloat length) : width(width), height(height), length(length) {}

Shape *Cuboid::clone() const { return new Cuboid(*this); };

// class PrismWall : public SimpleShape

void PrismWall::generate() {
    int i, j, k;
    double theta, x, y;
    for (i = 0; i <= span; ++i) {
        theta = 2 * (i + offset) * M_PI / sides;
        x = cos(theta);
        y = sin(theta);
        if (i > 0) {
            j = (i - 1) * 12;
            vertices[j + 9] = vertices[j + 6] = radius * x;
            vertices[j + 10] = vertices[j + 7] = radius * y;
            vertices[j + 11] = height / 2;
            vertices[j + 8] = -height / 2;
            normals[j + 9] = normals[j + 6] = x;
            normals[j + 10] = normals[j + 7] = y;
            normals[j + 11] = 0;
            normals[j + 8] = 0;
        }
        if (i < span) {
            k = i * 12;
            vertices[k] = vertices[k + 3] = radius * x;
            vertices[k + 1] = vertices[k + 4] = radius * y;
            vertices[k + 2] = height / 2;
            vertices[k + 5] = -height / 2;
            normals[k] = normals[k + 3] = x;
            normals[k + 1] = normals[k + 4] = y;
            normals[k + 2] = 0;
            normals[k + 5] = 0;
        }
    }
}

PrismWall::PrismWall(GLfloat radius, GLfloat height, int sides) : PrismWall(radius, height, sides, 0, sides) {}

PrismWall::PrismWall(GLfloat radius, GLfloat height, int sides, float offset, int span) : radius(radius), height(height), sides(sides), offset(offset), span(span) {}

Shape *PrismWall::clone() const { return new PrismWall(*this); };

// class Sphere : public SimpleShape

void Sphere::generate() {
    int i, j;
    double theta, phi, x, y, z, xz, u, v;
    for (i = 0; i <= spanY; ++i) {
        theta = M_PI_2 - M_PI * (i + offsetY) / detail;
        xz = radius * cos(theta);
        y = radius * sin(theta);
        for (j = 0; j <= spanX; ++j) {
            phi = 2 * M_PI * (j + offsetX) / detail;
            x = xz * cos(phi);
            z = xz * sin(phi);
            u = (j + 0.) / spanX;
            v = (spanY - i + 0.) / spanY;
            // right down
            if (i < spanY && j > 0) {
                vertices[i * 12 * spanX + (j - 1) * 12] = normals[i * 12 * spanX + (j - 1) * 12] = x;
                vertices[i * 12 * spanX + (j - 1) * 12 + 1] = normals[i * 12 * spanX + (j - 1) * 12 + 1] = y;
                vertices[i * 12 * spanX + (j - 1) * 12 + 2] = normals[i * 12 * spanX + (j - 1) * 12 + 2] = z;
                textureVertices[i * 8 * spanX + (j - 1) * 8 + 0] = u;
                textureVertices[i * 8 * spanX + (j - 1) * 8 + 1] = v;
            }
            // right up
            if (i > 0 && j > 0) {
                vertices[(i - 1) * 12 * spanX + (j - 1) * 12 + 3] = normals[(i - 1) * 12 * spanX + (j - 1) * 12 + 3] = x;
                vertices[(i - 1) * 12 * spanX + (j - 1) * 12 + 4] = normals[(i - 1) * 12 * spanX + (j - 1) * 12 + 4] = y;
                vertices[(i - 1) * 12 * spanX + (j - 1) * 12 + 5] = normals[(i - 1) * 12 * spanX + (j - 1) * 12 + 5] = z;
                textureVertices[(i - 1) * 8 * spanX + (j - 1) * 8 + 2] = u;
                textureVertices[(i - 1) * 8 * spanX + (j - 1) * 8 + 3] = v;
            }
            // left up
            if (i > 0 && j < spanX) {
                vertices[(i - 1) * 12 * spanX + j * 12 + 6] = normals[(i - 1) * 12 * spanX + j * 12 + 6] = x;
                vertices[(i - 1) * 12 * spanX + j * 12 + 7] = normals[(i - 1) * 12 * spanX + j * 12 + 7] = y;
                vertices[(i - 1) * 12 * spanX + j * 12 + 8] = normals[(i - 1) * 12 * spanX + j * 12 + 8] = z;
                textureVertices[(i - 1) * 8 * spanX + j * 8 + 4] = u;
                textureVertices[(i - 1) * 8 * spanX + j * 8 + 5] = v;
            }
            // left down
            if (i < spanY && j < spanX) {
                vertices[i * 12 * spanX + j * 12 + 9] = normals[i * 12 * spanX + j * 12 + 9] = x;
                vertices[i * 12 * spanX + j * 12 + 10] = normals[i * 12 * spanX + j * 12 + 10] = y;
                vertices[i * 12 * spanX + j * 12 + 11] = normals[i * 12 * spanX + j * 12 + 11] = z;
                textureVertices[i * 8 * spanX + j * 8 + 6] = u;
                textureVertices[i * 8 * spanX + j * 8 + 7] = v;
            }
        }
    }
}

Sphere::Sphere(GLfloat radius, int detail)
    : Sphere(radius, detail, 0, detail, 0, detail) {}

Sphere::Sphere(GLfloat radius, int detail, float offsetX, int spanX, float offsetY, int spanY)
    : radius(radius), detail(detail), offsetX(offsetX), spanX(spanX), offsetY(offsetY), spanY(spanY) {}

Shape *Sphere::clone() const { return new Sphere(*this); };

// class Donut : public SimpleShape

void Donut::generate() {
    int i, j, direction;
    double theta, phi, x, y, z, dx, dy, dz, u, v;
    for (i = 0; i <= spanXY; ++i) {
        theta = 2 * M_PI * (i + offsetXY) / detailXY;
        for (j = 0; j <= spanZ; ++j) {
            direction = j + offsetZ <= detailZ / 4 || j + offsetZ >= 3 * detailZ / 4 ? 1 : -1;
            phi = 2 * M_PI * (j + offsetZ) / detailZ;
            x = (middleRadius + ringRadius * cos(phi)) * cos(theta);
            y = (middleRadius + ringRadius * cos(phi)) * sin(theta);
            z = ringRadius * sin(phi);
            dx = cos(phi) * cos(theta);
            dy = cos(phi) * sin(theta);
            dz = sin(phi);
            // left down
            if (i < spanXY && j < spanZ) {
                vertices[i * 12 * spanZ + j * 12] = x;
                vertices[i * 12 * spanZ + j * 12 + 1] = y;
                vertices[i * 12 * spanZ + j * 12 + 2] = z;
                normals[i * 12 * spanZ + j * 12] = dx;
                normals[i * 12 * spanZ + j * 12 + 1] = dy;
                normals[i * 12 * spanZ + j * 12 + 2] = dz;
                textureVertices[i * 8 * spanZ + j * 8] = u;
                textureVertices[i * 8 * spanZ + j * 8 + 1] = v;
            }
            // left up
            if (i > 0 && j < spanZ) {
                vertices[(i - 1) * 12 * spanZ + j * 12 + 3] = x;
                vertices[(i - 1) * 12 * spanZ + j * 12 + 4] = y;
                vertices[(i - 1) * 12 * spanZ + j * 12 + 5] = z;
                normals[(i - 1) * 12 * spanZ + j * 12 + 3] = dx;
                normals[(i - 1) * 12 * spanZ + j * 12 + 4] = dy;
                normals[(i - 1) * 12 * spanZ + j * 12 + 5] = dz;
                textureVertices[(i - 1) * 8 * spanZ + j * 8 + 2] = u;
                textureVertices[(i - 1) * 8 * spanZ + j * 8 + 3] = v;
            }
            // right up
            if (i > 0 && j > 0) {
                vertices[(i - 1) * 12 * spanZ + (j - 1) * 12 + 6] = x;
                vertices[(i - 1) * 12 * spanZ + (j - 1) * 12 + 7] = y;
                vertices[(i - 1) * 12 * spanZ + (j - 1) * 12 + 8] = z;
                normals[(i - 1) * 12 * spanZ + (j - 1) * 12 + 6] = dx;
                normals[(i - 1) * 12 * spanZ + (j - 1) * 12 + 7] = dy;
                normals[(i - 1) * 12 * spanZ + (j - 1) * 12 + 8] = dz;
                textureVertices[(i - 1) * 8 * spanZ + (j - 1) * 8 + 4] = u;
                textureVertices[(i - 1) * 8 * spanZ + (j - 1) * 8 + 5] = v;
            }
            // right down
            if (i < spanXY && j > 0) {
                vertices[i * 12 * spanZ + (j - 1) * 12 + 9] = x;
                vertices[i * 12 * spanZ + (j - 1) * 12 + 10] = y;
                vertices[i * 12 * spanZ + (j - 1) * 12 + 11] = z;
                normals[i * 12 * spanZ + (j - 1) * 12 + 9] = dx;
                normals[i * 12 * spanZ + (j - 1) * 12 + 10] = dy;
                normals[i * 12 * spanZ + (j - 1) * 12 + 11] = dz;
                textureVertices[i * 8 * spanZ + (j - 1) * 8 + 6] = u;
                textureVertices[i * 8 * spanZ + (j - 1) * 8 + 7] = v;
            }
        }
    }
}

Donut::Donut(GLfloat innerRadius, GLfloat outterRadius, int detailXY, int detailZ)
    : Donut(innerRadius, outterRadius, detailXY, 0, detailXY, detailZ, 0, detailZ) {}

Donut::Donut(GLfloat innerRadius, GLfloat outterRadius, int detailXY, float offsetXY, int spanXY, int detailZ, float offsetZ, int spanZ)
    : middleRadius((innerRadius + outterRadius) / 2),
      ringRadius((outterRadius - innerRadius) / 2),
      detailXY(detailXY),
      offsetXY(offsetXY),
      spanXY(spanXY),
      detailZ(detailZ),
      offsetZ(offsetZ),
      spanZ(spanZ) {}

Shape *Donut::clone() const { return new Donut(*this); };

// class Ring : public Donut

void Ring::generate() {
    GLfloat side = outterRadius - innerRadius;
    GLfloat hypotenuse = sqrt(2 * pow(side, 2));
    innerRadius -= (hypotenuse - side) / 2;
    outterRadius = innerRadius + hypotenuse;
    GLfloat middleRadius((innerRadius + outterRadius) / 2);
    GLfloat ringRadius((outterRadius - innerRadius) / 2);
    int i, j, directionXY, directionZ;
    double theta, phi, x, y, z, u, v;
    bool parity;
    for (i = 0; i <= span; ++i) {
        theta = 2 * M_PI * (i + offset) / detail;
        for (j = 0; j <= 4; ++j) {
            directionXY = j == 0 || j >= 3 ? 1 : -1;
            directionZ = j <= 1 || j == 4 ? 1 : -1;
            parity = j % 2;
            phi = 2 * M_PI * (j + 0.5) / 4;
            x = (middleRadius + ringRadius * cos(phi)) * cos(theta);
            y = (middleRadius + ringRadius * cos(phi)) * sin(theta);
            z = ringRadius * sin(phi);
            // left down
            if (i < span && j < 4) {
                vertices[i * 12 * 4 + j * 12] = x;
                vertices[i * 12 * 4 + j * 12 + 1] = y;
                vertices[i * 12 * 4 + j * 12 + 2] = directionZ * height / 2;
                normals[i * 12 * 4 + j * 12] = directionXY * x * (parity == 1);
                normals[i * 12 * 4 + j * 12 + 1] = directionXY * y * (parity == 1);
                normals[i * 12 * 4 + j * 12 + 2] = directionZ * (parity == 0);
                textureVertices[i * 8 * 4 + j * 8] = u;
                textureVertices[i * 8 * 4 + j * 8 + 1] = v;
            }
            // left up
            if (i > 0 && j < 4) {
                vertices[(i - 1) * 12 * 4 + j * 12 + 3] = x;
                vertices[(i - 1) * 12 * 4 + j * 12 + 4] = y;
                vertices[(i - 1) * 12 * 4 + j * 12 + 5] = directionZ * height / 2;
                normals[(i - 1) * 12 * 4 + j * 12 + 3] = directionXY * x * (parity == 1);
                normals[(i - 1) * 12 * 4 + j * 12 + 4] = directionXY * y * (parity == 1);
                normals[(i - 1) * 12 * 4 + j * 12 + 5] = directionZ * (parity == 0);
                textureVertices[(i - 1) * 8 * 4 + j * 8 + 2] = u;
                textureVertices[(i - 1) * 8 * 4 + j * 8 + 3] = v;
            }
            // right up
            if (i > 0 && j > 0) {
                vertices[(i - 1) * 12 * 4 + (j - 1) * 12 + 6] = x;
                vertices[(i - 1) * 12 * 4 + (j - 1) * 12 + 7] = y;
                vertices[(i - 1) * 12 * 4 + (j - 1) * 12 + 8] = directionZ * height / 2;
                normals[(i - 1) * 12 * 4 + (j - 1) * 12 + 6] = directionXY * x * (parity == 0);
                normals[(i - 1) * 12 * 4 + (j - 1) * 12 + 7] = directionXY * y * (parity == 0);
                normals[(i - 1) * 12 * 4 + (j - 1) * 12 + 8] = directionZ * (parity == 1);
                textureVertices[(i - 1) * 8 * 4 + (j - 1) * 8 + 4] = u;
                textureVertices[(i - 1) * 8 * 4 + (j - 1) * 8 + 5] = v;
            }
            // right down
            if (i < span && j > 0) {
                vertices[i * 12 * 4 + (j - 1) * 12 + 9] = x;
                vertices[i * 12 * 4 + (j - 1) * 12 + 10] = y;
                vertices[i * 12 * 4 + (j - 1) * 12 + 11] = directionZ * height / 2;
                normals[i * 12 * 4 + (j - 1) * 12 + 9] = directionXY * x * (parity == 0);
                normals[i * 12 * 4 + (j - 1) * 12 + 10] = directionXY * y * (parity == 0);
                normals[i * 12 * 4 + (j - 1) * 12 + 11] = directionZ * (parity == 1);
                textureVertices[i * 8 * 4 + (j - 1) * 8 + 6] = u;
                textureVertices[i * 8 * 4 + (j - 1) * 8 + 7] = v;
            }
        }
    }
}

Ring::Ring(GLfloat innerRadius, GLfloat outterRadius, GLfloat height, int detail)
    : Ring(innerRadius, outterRadius, height, detail, 0, detail) {}

Ring::Ring(GLfloat innerRadius, GLfloat outterRadius, GLfloat height, int detail, float offset, int span)
    : innerRadius(innerRadius),
      outterRadius(outterRadius),
      height(height),
      detail(detail),
      offset(offset),
      span(span) {}

Shape *Ring::clone() const { return new Ring(*this); };

// class CompoundShape : public Shape

void CompoundShape::renderRaw() {
    std::for_each(shapes.begin(), shapes.end(), [](auto &shape) { shape->render(); });
}

CompoundShape::CompoundShape(std::initializer_list<Shape *> shapes) : shapes(shapes.begin(), shapes.end()) {}

CompoundShape::CompoundShape(std::vector<Shape *> shapes) : shapes(shapes.begin(), shapes.end()) {}

Shape *CompoundShape::clone() const { return new CompoundShape(*this); }

CompoundShape *CompoundShape::clone(int times, Shape *(*transform)(int, Shape *) ) { return Shape::clone(times, transform); }