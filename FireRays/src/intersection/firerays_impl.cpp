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
#include "firerays_impl.h"
#include "../primitive/mesh.h"
#include "../primitive/instance.h"
#include "../except/except.h"

#include "../device/intersection_device.h"
#include "../device/calc_intersection_device_cl.h"

#include <vector>
#include <algorithm>

namespace FireRays
{
    IntersectionApiImpl::IntersectionApiImpl(IntersectionDevice* device)
        : nextid_(1)
    , m_device(device)
    {
        world_.hint_ = 0;
    }

    void IntersectionApiImpl::SetOption(char const* name, char const* value)
    {
        world_.options_.SetValue(name, value);
    }

    void IntersectionApiImpl::SetOption(char const* name, float value)
    {
        world_.options_.SetValue(name, value);
    }

    Buffer* IntersectionApiImpl::CreateFromOpenClBuffer(cl_mem buffer) const
    {
        auto cldev = dynamic_cast<CalcIntersectionDeviceCl*>(m_device.get());
        
        if (cldev)
        {
            return cldev->CreateBuffer(buffer);
        }
        
        Throw("CL interop not supported");
        
        return nullptr;
    }

    IntersectionApiImpl::~IntersectionApiImpl()
    {

    }

    Shape* IntersectionApiImpl::CreateMesh(
        // Position data
        float* vertices, int vnum, int vstride,
        // Index data for vertices
        int* indices, int istride,
        // Numbers of vertices per face
        int* numfacevertices,
        // Number of faces
        int  numface
        ) const
    {
        Mesh* mesh = new Mesh(vertices, vnum, vstride, indices, istride, numfacevertices, numface);

        mesh->SetId(nextid_++);

        return mesh;
    }


    Shape* IntersectionApiImpl::CreateInstance(Shape const* shape) const
    {
        Mesh const* mesh = static_cast<Mesh const*>(shape);

        Instance* instance = new Instance(mesh);

        instance->SetId(nextid_++);

        return instance;
    }

    void IntersectionApiImpl::DeleteShape(Shape const* shape)
    {
        delete shape;
    }

    void IntersectionApiImpl::AttachShape(Shape const* shape)
    {
        world_.AttachShape(shape);
    }

    void IntersectionApiImpl::DetachShape(Shape const* shape)
    {
        world_.DetachShape(shape);
    }
    
    void IntersectionApiImpl::DetachAll()
    {
        world_.DetachAll();
    }


    void IntersectionApiImpl::Commit()
    {
        ThrowIf(world_.shapes_.empty(), "Scene is empty.");
        m_device->Preprocess(world_);

        world_.OnCommit();
    }

    void IntersectionApiImpl::DeleteBuffer(Buffer* buffer) const
    {
        m_device->DeleteBuffer(buffer);
    }

    void IntersectionApiImpl::QueryIntersection(Buffer const* rays, int numrays, Buffer* hitinfos, Event const* waitevent, Event** event) const
    {
        m_device->QueryIntersection(rays, numrays, hitinfos, waitevent, event);
    }

    void IntersectionApiImpl::QueryOcclusion(Buffer const* rays, int numrays, Buffer* hitresults, Event const* waitevent, Event** event) const
    {
        m_device->QueryOcclusion(rays, numrays, hitresults, waitevent, event);
    }

    void IntersectionApiImpl::QueryIntersection(Buffer const* rays, Buffer const* numrays, int maxrays, Buffer* hitinfos, Event const* waitevent, Event** event) const
    {
        m_device->QueryIntersection(rays, numrays, maxrays, hitinfos, waitevent, event);
    }

    void IntersectionApiImpl::QueryOcclusion(Buffer const* rays, Buffer const* numrays, int maxrays, Buffer* hitresults, Event const* waitevent, Event** event) const
    {
        m_device->QueryOcclusion(rays, numrays, maxrays, hitresults, waitevent, event);
    }

    void IntersectionApiImpl::DeleteEvent(Event* event) const
    {
        m_device->DeleteEvent(event);
    }
    
    Buffer* IntersectionApiImpl::CreateBuffer(size_t size, void* initdata) const
    {
        return m_device->CreateBuffer(size, initdata);
    }
    
    void IntersectionApiImpl::MapBuffer(Buffer* buffer, MapType type, size_t offset, size_t size, void** data, Event** event) const
    {
        return m_device->MapBuffer(buffer, type, offset, size, data, event);
    }
    
    void IntersectionApiImpl::UnmapBuffer(Buffer* buffer, void* ptr, Event** event) const
    {
        return m_device->UnmapBuffer(buffer, ptr, event);
    }

}

