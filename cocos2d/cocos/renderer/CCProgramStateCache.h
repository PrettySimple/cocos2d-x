//
//  ProgramStateCache.h
//  cocos2d
//
//  Created by Christophe Genova on 08/11/2019.
//

#pragma once
#include <cocos/base/ccTypes.h>
#include <cocos/renderer/backend/Types.h>
#include <map>

namespace cocos2d {
    namespace backend {
        class Program;
        class ProgramState;
        class TextureBackend;
    } // namespace backend
    class CallbackCommand;
    class Texture2D;
    class Renderer;

    class ProgramStateCache {
        public:
            using MaterialId = uint32_t;
            using NewProgramStateCbk = std::function<void(backend::ProgramState*)>;
        
            static MaterialId hash(const void* input, int len);
        
            static MaterialId computeMaterialId(cocos2d::backend::Program *       program,
                                                cocos2d::backend::TextureBackend *texture,
                                                cocos2d::BlendFunc                blendType,
                                                MaterialId                        uniformId = 0);
            
            static backend::ProgramState *getOrCreateProgramState(backend::ProgramType type,
                                                                  cocos2d::Texture2D * texture,
                                                                  cocos2d::BlendFunc   blendType,
                                                                  MaterialId           uniformId = 0,
                                                                  NewProgramStateCbk   newProgramStateCbk = nullptr);
            
            static backend::ProgramState *getOrCreateProgramState(const std::string & vertexSh,
                                                                  const std::string & fragSh,
                                                                  cocos2d::Texture2D *texture,
                                                                  cocos2d::BlendFunc  blendType,
                                                                  MaterialId          uniformId = 0,
                                                                  NewProgramStateCbk  newProgramStateCbk = nullptr);
            
            static void clear();
            
            static void setUpStandardAttributeLayout(cocos2d::backend::ProgramState *programState);
  
            static void addStandardUniformRenderCommand(cocos2d::CallbackCommand *      cmd,
                                                        cocos2d::Renderer *             renderer,
                                                        cocos2d::backend::ProgramState *programState,
                                                        float                           globalOrder,
                                                        cocos2d::Texture2D *            texture,
                                                        bool                            isUsingMVP);
        protected:
            std::map<MaterialId, cocos2d::backend::ProgramState *> _programStateCache;
            
            static ProgramStateCache &_getInstance();
            
            backend::ProgramState *_getOrCreateProgramState(cocos2d::backend::ProgramState *templateProgram,
                                                            cocos2d::Texture2D *            texture,
                                                            cocos2d::BlendFunc              blendType,
                                                            MaterialId                      uniformId          = 0,
                                                            NewProgramStateCbk              newProgramStateCbk = nullptr);
        
            void _cleanUnusued();
            
        private:
            ProgramStateCache() {}
            ~ProgramStateCache() {}
            ProgramStateCache(ProgramStateCache const &src)     = delete;
            ProgramStateCache(ProgramStateCache &&src) noexcept = delete;
            ProgramStateCache &operator=(ProgramStateCache const &) = delete;
            ProgramStateCache &operator=(ProgramStateCache &&) = delete;
    };
} // namespace cocos2d
