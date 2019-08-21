#ifndef __XENTAX_XNALARA_H
#define __XENTAX_XNALARA_H

/*
** XNALARA MESH FORMAT (ModelLoader.cs)
**  Models contains:
**  1. Armature
**  2. Mesh Count
**  3. Mesh Descriptors
**  3. Mesh Dictionary
**  4. Group (List of of Meshes) Dictionary
**  6. Visible Group Dictionary
*/

/*
** ARMATURE
**  Bone Count (uint32)
**  Bones
**   Name Length (uint08)
**   Name (variable)
**   Parent Index (uint16)
**   Absolute X (real32)
**   Absolute Y (real32)
**   Absolute Z (real32)
** MESH SECTION
**  Number of Meshes (uint32)
**  Meshes
**    Mesh Name Length (uint08)
**    Mesh Name (variable)
**    Number of UV Layers (uint32)
**    Number of Textures (uint32)
**    Textures
**      Texture Filename Length (uint08)
**      Texture Filename (variable)
**      UV Layer Index (uint32)
**    Vertex Count (uint32)
**    Vertices
**      X (real32)
**      Y (real32)
**      Z (real32)
**      Normal X (real32)
**      Normal Y (real32)
**      Normal Z (real32)
**      R (uint08)
**      G (uint08)
**      B (uint08)
**      A (uint08)
**      Texture Coordinates
**        U (real32)
**        V (real32)
**      Tangents
**        X (real32)
**        Y (real32)
**        Z (real32)
**        W (real32)
**      Global Blend Index #1 (uint16)
**      Global Blend Index #2 (uint16)
**      Global Blend Index #3 (uint16)
**      Global Blend Index #4 (uint16)
**      Blend Weight #1 (real32)
**      Blend Weight #2 (real32)
**      Blend Weight #3 (real32)
**      Blend Weight #4 (real32)
**    Number of Triangles (uint32)
**    Triangles
**      A (uint32)
**      B (uint32)
**      C (uint32)
*/

bool LightWaveToXNALara(const char* srcfile, const char* dstfile);

#endif
