/**********************************************************************
Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
********************************************************************/
#pragma once

#ifndef BVH_H
#define BVH_H

#include <memory>
#include <vector>
#include <list>
#include <atomic>
#include <iostream>
#include "bbox.h"

namespace RadeonRays
{
    ///< The class represents bounding volume hierarachy
    ///< intersection accelerator
    ///<
    class Bvh
    {
    public:
        Bvh(float traversal_cost, int num_bins = 64, bool usesah = false)
            : m_root(nullptr)
            , m_num_bins(num_bins)
            , m_usesah(usesah)
            , m_height(0)
            , m_traversal_cost(traversal_cost)
        {
        }

        ~Bvh() = default;

        // World space bounding box
        //应该是返回当前BVH树在世界坐标下的包围盒
        bbox const& Bounds() const;

        // Build function
        // bounds is an array of bounding boxes
        //根据一组bbox构建他们的BVH
        void Build(bbox const* bounds, int numbounds);

        // Get tree height
        int GetHeight() const;

        // Get reordered prim indices Nodes are pointing to
        virtual int const* GetIndices() const;

        // Get number of indices.
        // This number can differ from numbounds passed to Build function for
        // some BVH implementations (like SBVH)
        virtual size_t GetNumIndices() const;

        // Print BVH statistics
        virtual void PrintStatistics(std::ostream& os) const;
    protected:
        // Build function
        //Build函数的具体实现，利用虚函数特性，拥有不同的实现细节
        virtual void BuildImpl(bbox const* bounds, int numbounds);
        // BVH node
        struct Node;
        // Node allocation
        //从之前分配的m_nodes数组中获取最后一个未赋值的Node地址
        virtual Node* AllocateNode();
        //可能是初始化m_nodecnt，并保证m_nodes有足够空间保存所有Node
        virtual void  InitNodeAllocator(size_t maxnum);
        //可能包含
        //1)请求的起始索引
        //2)图元数量
        //3)根节点地址的地址
        //4)包围盒
        //5)包围盒中心点的包围盒
        //6)Level
        //7)Node索引
        struct SplitRequest
        {
            // Starting index of a request
            int startidx;
            // Number of primitives
            int numprims;
            // Root node
            Node** ptr;
            // Bounding box
            bbox bounds;
            // Centroid bounds
            bbox centroid_bounds;
            // Level
            int level;
            // Node index
            int index;
        };

        struct SahSplit
        {
            int dim;
            float split;
            float sah;
            float overlap;
        };

        //可能是根据以下信息构建BVH的Node
        //1)SplitRequest
        //2)所有包围盒
        //3)所有包围盒的中心
        //4)所有图元的索引
        void BuildNode(SplitRequest const& req, bbox const* bounds, Vec3 const* centroids, int* primindices);

        SahSplit FindSahSplit(SplitRequest const& req, bbox const* bounds, Vec3 const* centroids, int* primindices) const;

        // Enum for node type
        enum NodeType
        {
            kInternal,
            kLeaf
        };

        // Bvh nodes
        std::vector<Node> m_nodes;
        // Identifiers of leaf primitives
        //可能是存储叶节点图元的索引
        std::vector<int> m_indices;
        // Node allocator counter, atomic for thread safety
        std::atomic<int> m_nodecnt;

        // Identifiers of leaf primitives
        //可能是存储BVH构建过程中已经遍历到的叶节点图元的索引
        //即按照BVH构造后图元索引的重排列
        std::vector<int> m_packed_indices;

        // Bounding box containing all primitives
        bbox m_bounds;
        // Root node
        Node* m_root;
        // SAH flag
        bool m_usesah;
        // Tree height
        //应该是当前节点在BVH树中的高度
        int m_height;
        // Node traversal cost
        //应该是当前节点的遍历成本
        float m_traversal_cost;
        // Number of spatial bins to use for SAH
        int m_num_bins;


    private:
        Bvh(Bvh const&) = delete;
        Bvh& operator = (Bvh const&) = delete;

		friend class BvhTranslator;
    };
    //可能包含
    //1)Node的世界坐标下包围盒
    //2)Node的类型(内部节点/外部节点)
    //3)Node在整个BVH树中的索引
    //4)union(对于内部节点：左右子树的指针，对于叶节点：图元的起始索引和图元数量)
    struct Bvh::Node
    {
        // Node bounds in world space
        bbox bounds;
        // Type of the node
        NodeType type;
        // Node index in a complete tree
        int index;

        union
        {
            // For internal nodes: left and right children
            struct
            {
                Node* lc;
                Node* rc;
            };

            // For leaves: starting primitive index and number of primitives
            struct
            {
                int startidx;
                int numprims;
            };
        };
    };

    inline int const* Bvh::GetIndices() const
    {
        return &m_packed_indices[0];
    }

    inline size_t Bvh::GetNumIndices() const
    {
        return m_packed_indices.size();
    }

    inline int Bvh::GetHeight() const
    {
        return m_height;
    }
}

#endif // BVH_H
