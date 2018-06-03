// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#ifndef __I_SCENE_NODE_FACTORY_H_INCLUDED__
#define __I_SCENE_NODE_FACTORY_H_INCLUDED__
class ISceneNode;
//! Interface for dynamic creation of scene nodes
/** To be able to add custom scene nodes to Irrlicht and to make it possible for the
scene manager to save and load those external scene nodes, simply implement this
interface and register it in you scene manager via ISceneManager::registerSceneNodeFactory.
Note: When implementing your own scene node factory, don't call ISceneNodeManager::grab() to
increase the reference counter of the scene node manager. This is not necessary because the
scene node manager will grab() the factory anyway, and otherwise cyclic references will
be created and the scene manager and all its nodes won't get deallocated.
*/
class ISceneNodeFactory
{
public:
  //! adds a scene node to the scene graph based on its type id
  /** \param type: Type of the scene node to add.
  \param parent: Parent scene node of the new node, can be null to add the scene node to the root.
  \return Returns pointer to the new scene node or null if not successful.
  This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
  ISceneNode* addSceneNode(ESCENE_NODE_TYPE type, ISceneNode* parent = 0) = 0;
  //! adds a scene node to the scene graph based on its type name
  /** \param typeName: Type name of the scene node to add.
  \param parent: Parent scene node of the new node, can be null to add the scene node to the root.
  \return Returns pointer to the new scene node or null if not successful.
  This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
  ISceneNode* addSceneNode(const c8* typeName, ISceneNode* parent = 0) = 0;
  //! returns amount of scene node types this factory is able to create
  u32 getCreatableSceneNodeTypeCount() = 0;
  //! returns type of a createable scene node type
  /** \param idx: Index of scene node type in this factory. Must be a value between 0 and
  getCreatableSceneNodeTypeCount() */
  ESCENE_NODE_TYPE getCreateableSceneNodeType(u32 idx) = 0;
  //! returns type name of a createable scene node type by index
  /** \param idx: Index of scene node type in this factory. Must be a value between 0 and
  getCreatableSceneNodeTypeCount() */
  const c8* getCreateableSceneNodeTypeName(u32 idx) = 0;
  //! returns type name of a createable scene node type
  /** \param type: Type of scene node.
  \return: Returns name of scene node type if this factory can create the type, otherwise 0. */
  const c8* getCreateableSceneNodeTypeName(ESCENE_NODE_TYPE type) = 0;
};
#endif
