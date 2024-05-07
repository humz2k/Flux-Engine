/**
 * @file shadow_map_texture.c
 * @brief This file contains functions to manage shadow map textures using
 * Raylib and RLGL.
 */

#include "hqtools/hqtools.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Loads a shadow map render texture.
 *
 * This function creates a framebuffer object and an associated depth texture
 * for use as a shadow map. The depth texture is attached to the framebuffer
 * without a color buffer, as color data is not needed for shadow mapping.
 *
 * @param width The width of the shadow map texture.
 * @param height The height of the shadow map texture.
 * @return A RenderTexture2D structure containing the IDs and properties of the
 * depth texture and the framebuffer.
 */
RenderTexture2D LoadShadowmapRenderTexture(int width, int height) {
    RenderTexture2D target = {0};

    target.id = rlLoadFramebuffer(width, height); // Load an empty framebuffer
    target.texture.width = width;
    target.texture.height = height;

    if (target.id > 0) {
        rlEnableFramebuffer(target.id);

        // Create depth texture
        // We don't need a color texture for the shadowmap
        target.depth.id = rlLoadTextureDepth(width, height, false);
        target.depth.width = width;
        target.depth.height = height;
        target.depth.format = 19; // DEPTH_COMPONENT_24BIT?
        target.depth.mipmaps = 1;

        // Attach depth texture to FBO
        rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH,
                            RL_ATTACHMENT_TEXTURE2D, 0);

        // Check if fbo is complete with attachments (valid)
        if (rlFramebufferComplete(target.id))
            TRACELOG(LOG_INFO,
                     "FBO: [ID %i] Framebuffer object created successfully",
                     target.id);

        rlDisableFramebuffer();
    } else
        TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");

    return target;
}

/**
 * @brief Unloads a shadow map render texture from GPU memory (VRAM).
 *
 * This function unloads the framebuffer associated with the render texture. It
 * ensures that any depth texture or renderbuffer attached to the framebuffer is
 * automatically queried and deleted before deleting the framebuffer itself.
 *
 * @param target The render texture to unload, containing the framebuffer to be
 * deleted.
 */
void UnloadShadowmapRenderTexture(RenderTexture2D target) {
    if (target.id > 0) {
        // NOTE: Depth texture/renderbuffer is automatically
        // queried and deleted before deleting framebuffer
        rlUnloadFramebuffer(target.id);
    }
}