#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "../VoxelEngine/Face.hpp"
#include <iostream>

Vec3 calcNormalOfFace( std::vector<Vec3> pPositions, Vec3 pNormals[3] )
{
    Vec3 p0 = pPositions[1] - pPositions[0];
    Vec3 p1 = pPositions[2] - pPositions[0];
    Vec3 faceNormal = p0.cross(p1);

    Vec3 vertexNormal = pNormals[0];
    float dot = faceNormal.dot(vertexNormal);

    return ( dot < 0.0f ) ? -faceNormal : faceNormal;
}

std::vector<std::vector<Face>> loadObj(std::string input_file) {
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "../models/obj_files/";
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(input_file, reader_config)) {
        if (reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    std::vector<std::vector<Face>> shapeVec;

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        std::vector<Face> faces;
        auto face_mat_id = -1;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            std::vector<Vec3> face_vertices;
            Vec3 vertex_normals[3];
            Vec3 vertex_colors[3];
            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

                face_vertices.push_back(Vec3(vx, vy, vz));
                
                // !! Save these vertices for this face somewhere !!

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                    tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                    tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                    // !! Save these for each vertices for each face !!
                    vertex_normals[v] = Vec3(nx, ny, nz);
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0) {
                    tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                    tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                }

                // Optional: vertex colors
                tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
                vertex_colors[v] = Vec3(red, green, blue);
            }
            index_offset += fv;

            // per-face material
            face_mat_id = shapes[s].mesh.material_ids[f];
            Vec3 face_normal = calcNormalOfFace(face_vertices, vertex_normals);
            // Vec3 face_color = (vertex_colors[0] + vertex_colors[1] + vertex_colors[2]) / 3;
            faces.push_back(Face(face_vertices, face_normal, Vec3(0,1,0)));
        }
        shapeVec.push_back(faces);
    }
    std::cout << "Model loaded!\n";
    return shapeVec;
}


#endif