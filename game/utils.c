#include "utils.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Include stb_image implementation (single-header library)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Include cgltf implementation (single-header library for GLB parsing)
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

void loadOBJ(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Error: Cannot open OBJ file %s\n", filename);
        return;
    }

    float vertices[100000][3];
    float normals[100000][3];
    int numVertices = 0;
    int numNormals = 0;
    int faces[100000][6];
    int numFaces = 0;

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "v ", 2) == 0)
        {
            if (numVertices >= 100000)
            {
                fprintf(stderr, "Error: Too many vertices (max 100000)\n");
                fclose(file);
                return;
            }
            if (sscanf(line + 2, "%f %f %f", &vertices[numVertices][0], &vertices[numVertices][1], &vertices[numVertices][2]) == 3)
            {
                numVertices++;
            }
            else
            {
                fprintf(stderr, "Error: Invalid vertex format: %s\n", line);
            }
        }
        else if (strncmp(line, "vn ", 3) == 0)
        {
            if (numNormals >= 100000)
            {
                fprintf(stderr, "Error: Too many normals (max 100000)\n");
                fclose(file);
                return;
            }
            if (sscanf(line + 3, "%f %f %f", &normals[numNormals][0], &normals[numNormals][1], &normals[numNormals][2]) == 3)
            {
                numNormals++;
            }
            else
            {
                fprintf(stderr, "Error: Invalid normal format: %s\n", line);
            }
        }
        else if (strncmp(line, "f ", 2) == 0)
        {
            if (numFaces >= 100000)
            {
                fprintf(stderr, "Error: Too many faces (max 10000)\n");
                fclose(file);
                return;
            }
            int v1, n1, v2, n2, v3, n3;
            if (sscanf(line + 2, "%d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3) == 6)
            {
                if (v1 < 1 || v1 > numVertices || n1 < 1 || n1 > numNormals ||
                    v2 < 1 || v2 > numVertices || n2 < 1 || n2 > numNormals ||
                    v3 < 1 || v3 > numVertices || n3 < 1 || n3 > numNormals)
                {
                    fprintf(stderr, "Error: Invalid face indices: %s\n", line);
                    continue;
                }
                faces[numFaces][0] = v1 - 1;
                faces[numFaces][1] = n1 - 1;
                faces[numFaces][2] = v2 - 1;
                faces[numFaces][3] = n2 - 1;
                faces[numFaces][4] = v3 - 1;
                faces[numFaces][5] = n3 - 1;
                numFaces++;
            }
            else if (sscanf(line + 2, "%d %d %d", &v1, &v2, &v3) == 3)
            {
                if (v1 < 1 || v1 > numVertices || v2 < 1 || v2 > numVertices || v3 < 1 || v3 > numVertices)
                {
                    fprintf(stderr, "Error: Invalid face indices (no normals): %s\n", line);
                    continue;
                }
                faces[numFaces][0] = v1 - 1;
                faces[numFaces][1] = 0;
                faces[numFaces][2] = v2 - 1;
                faces[numFaces][3] = 0;
                faces[numFaces][4] = v3 - 1;
                faces[numFaces][5] = 0;
                numFaces++;
            }
            else
            {
                fprintf(stderr, "Error: Unsupported face format: %s\n", line);
            }
        }
    }
    fclose(file);

    if (numVertices == 0 || numFaces == 0)
    {
        fprintf(stderr, "Error: Invalid OBJ file %s (no vertices or faces)\n", filename);
        return;
    }

    gameState.objList = glGenLists(1);
    if (gameState.objList == 0)
    {
        fprintf(stderr, "Error: Failed to create display list\n");
        return;
    }
    glNewList(gameState.objList, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < numFaces; i++)
    {
        int v1 = faces[i][0], n1 = faces[i][1];
        int v2 = faces[i][2], n2 = faces[i][3];
        int v3 = faces[i][4], n3 = faces[i][5];
        if (n1 > 0)
            glNormal3f(normals[n1][0], normals[n1][1], normals[n1][2]);
        glVertex3f(vertices[v1][0], vertices[v1][1], vertices[v1][2]);
        if (n2 > 0)
            glNormal3f(normals[n2][0], normals[n2][1], normals[n2][2]);
        glVertex3f(vertices[v2][0], vertices[v2][1], vertices[v2][2]);
        if (n3 > 0)
            glNormal3f(normals[n3][0], normals[n3][1], normals[n3][2]);
        glVertex3f(vertices[v3][0], vertices[v3][1], vertices[v3][2]);
    }
    glEnd();
    glEndList();

    printf("Loaded OBJ: %d vertices, %d normals, %d faces\n", numVertices, numNormals, numFaces);
}

void loadOBJ2(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Error: Cannot open OBJ file %s\n", filename);
        return;
    }

    float vertices[100000][3];
    float normals[100000][3];
    int numVertices = 0;
    int numNormals = 0;
    int faces[100000][6];
    int numFaces = 0;

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "v ", 2) == 0)
        {
            if (numVertices >= 100000)
            {
                fprintf(stderr, "Error: Too many vertices (max 100000)\n");
                fclose(file);
                return;
            }
            if (sscanf(line + 2, "%f %f %f", &vertices[numVertices][0], &vertices[numVertices][1], &vertices[numVertices][2]) == 3)
            {
                numVertices++;
            }
            else
            {
                fprintf(stderr, "Error: Invalid vertex format: %s\n", line);
            }
        }
        else if (strncmp(line, "vn ", 3) == 0)
        {
            if (numNormals >= 100000)
            {
                fprintf(stderr, "Error: Too many normals (max 100000)\n");
                fclose(file);
                return;
            }
            if (sscanf(line + 3, "%f %f %f", &normals[numNormals][0], &normals[numNormals][1], &normals[numNormals][2]) == 3)
            {
                numNormals++;
            }
            else
            {
                fprintf(stderr, "Error: Invalid normal format: %s\n", line);
            }
        }
        else if (strncmp(line, "f ", 2) == 0)
        {
            if (numFaces >= 100000)
            {
                fprintf(stderr, "Error: Too many faces (max 10000)\n");
                fclose(file);
                return;
            }
            int v1, n1, v2, n2, v3, n3;
            if (sscanf(line + 2, "%d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3) == 6)
            {
                if (v1 < 1 || v1 > numVertices || n1 < 1 || n1 > numNormals ||
                    v2 < 1 || v2 > numVertices || n2 < 1 || n2 > numNormals ||
                    v3 < 1 || v3 > numVertices || n3 < 1 || n3 > numNormals)
                {
                    fprintf(stderr, "Error: Invalid face indices: %s\n", line);
                    continue;
                }
                faces[numFaces][0] = v1 - 1;
                faces[numFaces][1] = n1 - 1;
                faces[numFaces][2] = v2 - 1;
                faces[numFaces][3] = n2 - 1;
                faces[numFaces][4] = v3 - 1;
                faces[numFaces][5] = n3 - 1;
                numFaces++;
            }
            else if (sscanf(line + 2, "%d %d %d", &v1, &v2, &v3) == 3)
            {
                if (v1 < 1 || v1 > numVertices || v2 < 1 || v2 > numVertices || v3 < 1 || v3 > numVertices)
                {
                    fprintf(stderr, "Error: Invalid face indices (no normals): %s\n", line);
                    continue;
                }
                faces[numFaces][0] = v1 - 1;
                faces[numFaces][1] = 0;
                faces[numFaces][2] = v2 - 1;
                faces[numFaces][3] = 0;
                faces[numFaces][4] = v3 - 1;
                faces[numFaces][5] = 0;
                numFaces++;
            }
            else
            {
                fprintf(stderr, "Error: Unsupported face format: %s\n", line);
            }
        }
    }
    fclose(file);

    if (numVertices == 0 || numFaces == 0)
    {
        fprintf(stderr, "Error: Invalid OBJ file %s (no vertices or faces)\n", filename);
        return;
    }

    gameState.objList2 = glGenLists(1);
    if (gameState.objList2 == 0)
    {
        fprintf(stderr, "Error: Failed to create display list\n");
        return;
    }
    glNewList(gameState.objList2, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < numFaces; i++)
    {
        int v1 = faces[i][0], n1 = faces[i][1];
        int v2 = faces[i][2], n2 = faces[i][3];
        int v3 = faces[i][4], n3 = faces[i][5];
        if (n1 > 0)
            glNormal3f(normals[n1][0], normals[n1][1], normals[n1][2]);
        glVertex3f(vertices[v1][0], vertices[v1][1], vertices[v1][2]);
        if (n2 > 0)
            glNormal3f(normals[n2][0], normals[n2][1], normals[n2][2]);
        glVertex3f(vertices[v2][0], vertices[v2][1], vertices[v2][2]);
        if (n3 > 0)
            glNormal3f(normals[n3][0], normals[n3][1], normals[n3][2]);
        glVertex3f(vertices[v3][0], vertices[v3][1], vertices[v3][2]);
    }
    glEnd();
    glEndList();

    printf("Loaded OBJ: %d vertices, %d normals, %d faces\n", numVertices, numNormals, numFaces);
}

void renderText(float x, float y, void *font, const char *text)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    glutBitmapString(font, (unsigned char *)text);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / h, 0.1f, RENDER_DIST);
    glMatrixMode(GL_MODELVIEW);
}

// New function: Load GLB model (e.g., toad) and create display list + texture
void loadGLB(const char *filename, GLuint *displayList, GLuint *textureID)
{
    // Parse the GLB file using cgltf
    cgltf_options options = {0};
    cgltf_data *data = NULL;
    cgltf_result result = cgltf_parse_file(&options, filename, &data);
    if (result != cgltf_result_success)
    {
        fprintf(stderr, "Error: Failed to parse GLB file %s\n", filename);
        return;
    }

    // Load binary buffers from the GLB
    result = cgltf_load_buffers(&options, data, filename);
    if (result != cgltf_result_success)
    {
        fprintf(stderr, "Error: Failed to load GLB buffers\n");
        cgltf_free(data);
        return;
    }

    // Check for meshes
    if (data->meshes_count == 0)
    {
        fprintf(stderr, "Error: No meshes found in GLB\n");
        cgltf_free(data);
        return;
    }

    // Get the first mesh and primitive (assuming simple model like toad)
    cgltf_mesh *mesh = &data->meshes[0];
    cgltf_primitive *prim = &mesh->primitives[0];

    // Find accessors for positions, normals, texture coords, and indices
    cgltf_accessor *pos_accessor = NULL;
    cgltf_accessor *norm_accessor = NULL;
    cgltf_accessor *tex_accessor = NULL;
    cgltf_accessor *index_accessor = prim->indices;

    for (cgltf_size i = 0; i < prim->attributes_count; i++)
    {
        cgltf_attribute *attr = &prim->attributes[i];
        if (attr->type == cgltf_attribute_type_position)
            pos_accessor = attr->data;
        else if (attr->type == cgltf_attribute_type_normal)
            norm_accessor = attr->data;
        else if (attr->type == cgltf_attribute_type_texcoord)
            tex_accessor = attr->data;
    }

    if (!pos_accessor || !norm_accessor)
    {
        fprintf(stderr, "Error: GLB missing required attributes (positions or normals)\n");
        cgltf_free(data);
        return;
    }

    // Load embedded texture if available (for toad's colors)
    if (data->textures_count > 0 && data->images_count > 0 && prim->material)
    {
        cgltf_image *image = &data->images[0];
        if (image->buffer_view)
        {
            int width, height, channels;
            unsigned char *image_data = (unsigned char *)image->buffer_view->buffer->data + image->buffer_view->offset;
            unsigned char *pixels = stbi_load_from_memory(image_data, (int)image->buffer_view->size, &width, &height, &channels, 0);
            if (pixels)
            {
                glGenTextures(1, textureID);
                glBindTexture(GL_TEXTURE_2D, *textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, channels == 4 ? GL_RGBA : GL_RGB, width, height, 0, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                stbi_image_free(pixels);
                printf("Loaded texture: %d x %d, %d channels\n", width, height, channels);
            }
            else
            {
                fprintf(stderr, "Error: Failed to load texture from GLB with stb_image\n");
            }
        }
    }
    else
    {
        fprintf(stderr, "No texture found in GLB; model will render without texture\n");
    }

    // Create OpenGL display list for rendering
    *displayList = glGenLists(1);
    if (*displayList == 0)
    {
        fprintf(stderr, "Error: Failed to create display list\n");
        cgltf_free(data);
        return;
    }

    glNewList(*displayList, GL_COMPILE);
    glBegin(GL_TRIANGLES);

    if (index_accessor)
    {
        // Use indexed rendering if indices are present
        for (cgltf_size i = 0; i < index_accessor->count; i++)
        {
            cgltf_uint index;
            cgltf_accessor_read_uint(index_accessor, i, &index, 1);

            // Texture coordinates
            if (tex_accessor)
            {
                float tex[2];
                cgltf_accessor_read_float(tex_accessor, index, tex, 2);
                glTexCoord2f(tex[0], tex[1]);
            }

            // Normals
            float norm[3];
            cgltf_accessor_read_float(norm_accessor, index, norm, 3);
            glNormal3f(norm[0], norm[1], norm[2]);

            // Positions
            float pos[3];
            cgltf_accessor_read_float(pos_accessor, index, pos, 3);
            glVertex3f(pos[0], pos[1], pos[2]);
        }
    }
    else
    {
        // Sequential rendering if no indices
        for (cgltf_size i = 0; i < pos_accessor->count; i++)
        {
            // Texture coordinates
            if (tex_accessor)
            {
                float tex[2];
                cgltf_accessor_read_float(tex_accessor, i, tex, 2);
                glTexCoord2f(tex[0], tex[1]);
            }

            // Normals
            float norm[3];
            cgltf_accessor_read_float(norm_accessor, i, norm, 3);
            glNormal3f(norm[0], norm[1], norm[2]);

            // Positions
            float pos[3];
            cgltf_accessor_read_float(pos_accessor, i, pos, 3);
            glVertex3f(pos[0], pos[1], pos[2]);
        }
    }

    glEnd();
    glEndList();

    printf("Loaded GLB: %zu vertices, %zu indices\n", pos_accessor->count, index_accessor ? index_accessor->count : 0);

    // Clean up cgltf data
    cgltf_free(data);
}