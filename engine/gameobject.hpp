#ifndef _FLUX_GAMEOBJECT_HPP_
#define _FLUX_GAMEOBJECT_HPP_

#include "config.hpp"
#include "raylib.h"
#include "raymath.h"

#ifdef FLUX_SCRIPT_IMPLEMENTATION
#define SCRIPT_END {}
#else
#define SCRIPT_END ;
#endif

namespace Flux{

    class GameObject;

    class Script{
        private:
            GameObject* gameobject;

        public:
            Script(GameObject* gameobject_)
            #ifdef FLUX_SCRIPT_IMPLEMENTATION
            : gameobject(gameobject_)
            #endif
            SCRIPT_END

            ~Script() SCRIPT_END

            // called on GameObject initialization
            FLUXAPI void onInit() SCRIPT_END

            // called on GameObject destruction
            FLUXAPI void onDestroy() SCRIPT_END

            // called every frame
            FLUXAPI void onUpdate() SCRIPT_END

            // called after every GameObject is done updating
            FLUXAPI void afterUpdate() SCRIPT_END

    };

    class GameObject{
        private:
            Script* scripts;
            int nscripts;
            Transform transform;

        public:
            GameObject();
            ~GameObject();
    };

};

#endif

