#pragma once
#include "Falcor.h"

namespace Falcor
{
using whitelist_t = std::set<std::string>;

// returns true if at least one material was whitelisted (or scene was invalid)
// pTransparencyWhitelistBuffer will always be updated
inline bool updateWhitelist(const ref<Device>& pDevice, const ref<Scene>& pScene, const whitelist_t& transparencyWhitelist, ref<Buffer>& pTransparencyWhitelistBuffer)
{
    if (!pScene) return true;
    bool any = false;

    // Calculate the number of uint32_t elements needed to store all bits
    uint32_t materialCount = pScene->getMaterialCount();
    uint32_t uintCount = (materialCount + 31) / 32; // Round up to the nearest uint32_t

    std::vector<uint32_t> whitelist(uintCount, 0); // Initialize all bits to 0

    // Pack the boolean values into bits
    for (uint32_t mat = 0; mat < materialCount; ++mat)
    {
        std::string name = pScene->getMaterial(MaterialID(mat))->getName();
        bool isTransparent = transparencyWhitelist.find(name) != transparencyWhitelist.end();
        any |= isTransparent;

        if (isTransparent)
        {
            // Set the corresponding bit in the whitelist
            uint32_t uintIndex = mat / 32;
            uint32_t bitIndex = mat % 32;
            whitelist[uintIndex] |= (1 << bitIndex);
        }
    }

    pTransparencyWhitelistBuffer = Buffer::createStructured(pDevice, sizeof(uint32_t), uintCount, ResourceBindFlags::ShaderResource, Buffer::CpuAccess::None, whitelist.data(), false);

    return any;
}

}
