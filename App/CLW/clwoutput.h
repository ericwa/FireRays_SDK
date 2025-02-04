#pragma once

#include "Core/output.h"
#include "CLW.h"

namespace Baikal
{
    class ClwOutput : public Output
    {
    public:
        ClwOutput(std::uint32_t w, std::uint32_t h, CLWContext context)
        : Output(w, h)
        , m_context(context)
        , m_data(context.CreateBuffer<FireRays::float3>(w*h, CL_MEM_READ_WRITE))
        {
        }

        void GetData(FireRays::float3* data) const
        {
            m_context.ReadBuffer(0, m_data, data, m_data.GetElementCount()).Wait();
        }

        void Clear(FireRays::float3 const& val)
        {
            m_context.FillBuffer(0, m_data, val, m_data.GetElementCount()).Wait();
        }

        CLWBuffer<FireRays::float3> data() const { return m_data; }

    private:
        CLWBuffer<FireRays::float3> m_data;
        CLWContext m_context;
    };
}
