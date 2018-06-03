// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#ifndef __I_MESH_BUFFER_H_INCLUDED__
#define __I_MESH_BUFFER_H_INCLUDED__

//! Struct for holding a mesh with a single material.
/** A part of an IMesh which has the same material on each face of that
group. Logical groups of an IMesh need not be put into separate mesh
buffers, but can be. Separately animated parts of the mesh must be put
into separate mesh buffers.
Some mesh buffer implementations have limitations on the number of
vertices the buffer can hold. In that case, logical grouping can help.
Moreover, the number of vertices should be optimized for the GPU upload,
which often depends on the type of gfx card. Typial figures are
1000-10000 vertices per buffer.
SMeshBuffer is a simple implementation of a MeshBuffer, which supports
up to 65535 vertices.
Since meshbuffers are used for drawing, and hence will be exposed
to the driver, chances are high that they are grab()'ed from somewhere.
It's therefore required to dynamically allocate meshbuffers which are
passed to a video driver and only drop the buffer once it's not used in
the current code block anymore.
*/
class IMeshBuffer
{
public:
  //! Get the material of this meshbuffer
  /** \return Material of this buffer. */
  SMaterial& getMaterial() = 0;
  //! Get the material of this meshbuffer
  /** \return Material of this buffer. */
  const SMaterial& getMaterial() = 0;
  //! Get type of vertex data which is stored in this meshbuffer.
  /** \return Vertex type of this buffer. */
  E_VERTEX_TYPE getVertexType() = 0;
  //! Get access to vertex data. The data is an array of vertices.
  /** Which vertex type is used can be determined by getVertexType().
  \return Pointer to array of vertices. */
  const void* getVertices() = 0;
  //! Get access to vertex data. The data is an array of vertices.
  /** Which vertex type is used can be determined by getVertexType().
  \return Pointer to array of vertices. */
  void* getVertices() = 0;
  //! Get amount of vertices in meshbuffer.
  /** \return Number of vertices in this buffer. */
  u32 getVertexCount() = 0;
  //! Get type of index data which is stored in this meshbuffer.
  /** \return Index type of this buffer. */
  E_INDEX_TYPE getIndexType() = 0;
  //! Get access to Indices.
  /** \return Pointer to indices array. */
  const u16* getIndices() = 0;
  //! Get access to Indices.
  /** \return Pointer to indices array. */
  u16* getIndices() = 0;
  //! Get amount of indices in this meshbuffer.
  /** \return Number of indices in this buffer. */
  u32 getIndexCount() = 0;
  //! Get the axis aligned bounding box of this meshbuffer.
  /** \return Axis aligned bounding box of this buffer. */
  const aabbox3df& getBoundingBox() = 0;
  //! Set axis aligned bounding box
  /** \param box User defined axis aligned bounding box to use
  for this buffer. */
  void setBoundingBox(const aabbox3df& box) = 0;
  //! Recalculates the bounding box. Should be called if the mesh changed.
  void recalculateBoundingBox() = 0;
  //! returns position of vertex i
  const float3 getPosition(u32 i) = 0;
  //! returns position of vertex i
  float3 getPosition(u32 i) = 0;
  //! returns normal of vertex i
  const float3 getNormal(u32 i) = 0;
  //! returns normal of vertex i
  float3 getNormal(u32 i) = 0;
  //! returns texture coord of vertex i
  const vector2df& getTCoords(u32 i) = 0;
  //! returns texture coord of vertex i
  vector2df& getTCoords(u32 i) = 0;
  //! Append the vertices and indices to the current buffer
  /** Only works for compatible vertex types.
  \param vertices Pointer to a vertex array.
  \param numVertices Number of vertices in the array.
  \param indices Pointer to index array.
  \param numIndices Number of indices in array. */
  void append(const void* const vertices, u32 numVertices, const u16* const indices, u32 numIndices) = 0;
  //! Append the meshbuffer to the current buffer
  /** Only works for compatible vertex types
  \param other Buffer to append to this one. */
  void append(const IMeshBuffer* const other) = 0;
  //! get the current hardware mapping hint
  E_HARDWARE_MAPPING getHardwareMappingHint_Vertex() = 0;
  //! get the current hardware mapping hint
  E_HARDWARE_MAPPING getHardwareMappingHint_Index() = 0;
  //! set the hardware mapping hint, for driver
  void setHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint, E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX) = 0;
  //! flags the meshbuffer as changed, reloads hardware buffers
  void setDirty(E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX) = 0;
  //! Get the currently used ID for identification of changes.
  /** This shouldn't be used for anything outside the VideoDriver. */
  u32 getChangedID_Vertex() = 0;
  //! Get the currently used ID for identification of changes.
  /** This shouldn't be used for anything outside the VideoDriver. */
  u32 getChangedID_Index() = 0;
};


#endif
