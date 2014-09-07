//------------------------------------------------------------------------------
//  glRenderMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glRenderMgr.h"
#include "Render/Core/displayMgr.h"
#include "Render/Core/stateWrapper.h"
#include "Render/Core/texture.h"
#include "Render/Core/programBundle.h"
#include "Render/gl/glTypes.h"
#include "Render/gl/gl_impl.h"
#include "Render/gl/glExt.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat2x2.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Oryol {
namespace Render {

//------------------------------------------------------------------------------
bool
glRenderMgr::Supports(Feature::Code feat) const {
    switch (feat) {
        case Feature::TextureCompressionDXT:
            return glExt::HasExtension(glExt::TextureCompressionDXT);
        case Feature::TextureCompressionPVRTC:
            return glExt::HasExtension(glExt::TextureCompressionPVRTC);
        case Feature::TextureCompressionATC:
            return glExt::HasExtension(glExt::TextureCompressionATC);
        case Feature::TextureFloat:
            return glExt::HasExtension(glExt::TextureFloat);
        case Feature::TextureHalfFloat:
            return glExt::HasExtension(glExt::TextureHalfFloat);
        case Feature::Instancing:
            return glExt::HasExtension(glExt::InstancedArrays);
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
void
glRenderMgr::ApplyRenderTarget(texture* rt) {
    if (rt != this->curRenderTarget) {
        if (nullptr == rt) {
            // bind default render target
            this->displayManager->glBindDefaultFramebuffer();
        }
        else {
            ::glBindFramebuffer(GL_FRAMEBUFFER, rt->glGetFramebuffer());
            ORYOL_GL_CHECK_ERROR();
        }
    }
    renderMgrBase::ApplyRenderTarget(rt);
}

//------------------------------------------------------------------------------
void
glRenderMgr::ApplyDrawState(drawState* ds) {
    this->stateWrapper->ApplyDrawState(ds);
    renderMgrBase::ApplyDrawState(ds);
}

//------------------------------------------------------------------------------
/**
*/
void
glRenderMgr::ApplyTexture(int32 index, const texture* tex) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    int32 samplerIndex = this->curProgramBundle->getSamplerIndex(index);
    GLuint glTexture = tex->glGetTexture();
    GLenum glTarget = tex->glGetTarget();
    this->stateWrapper->BindTexture(samplerIndex, glTarget, glTexture);
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const float32& val) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform1f(glLoc, val);
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::vec2& val) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform2f(glLoc, val.x, val.y);
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::vec3& val) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform3f(glLoc, val.x, val.y, val.z);
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::vec4& val) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform4f(glLoc, val.x, val.y, val.z, val.w);
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const int32& val) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform1i(glLoc, val);
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::ivec2& val) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform2i(glLoc, val.x, val.y);
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::ivec3& val) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform3i(glLoc, val.x, val.y, val.z);
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::ivec4& val) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform4i(glLoc, val.x, val.y, val.z, val.w);
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::mat4& val) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniformMatrix4fv(glLoc, 1, GL_FALSE, glm::value_ptr(val));
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::mat3& val) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniformMatrix3fv(glLoc, 1, GL_FALSE, glm::value_ptr(val));
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::mat2& val) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniformMatrix2fv(glLoc, 1, GL_FALSE, glm::value_ptr(val));
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const float32* values, int32 numValues) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform1fv(glLoc, numValues, values);
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::vec2* values, int32 numValues) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform2fv(glLoc, numValues, glm::value_ptr(*values));
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::vec3* values, int32 numValues) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform3fv(glLoc, numValues, glm::value_ptr(*values));
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::vec4* values, int32 numValues) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform4fv(glLoc, numValues, glm::value_ptr(*values));
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const int32* values, int32 numValues) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform1iv(glLoc, numValues, values);
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::ivec2* values, int32 numValues) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform2iv(glLoc, numValues, glm::value_ptr(*values));
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::ivec3* values, int32 numValues) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform3iv(glLoc, numValues, glm::value_ptr(*values));
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::ivec4* values, int32 numValues) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform4iv(glLoc, numValues, glm::value_ptr(*values));
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::mat4* values, int32 numValues) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniformMatrix4fv(glLoc, numValues, GL_FALSE, glm::value_ptr(*values));
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::mat3* values, int32 numValues) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniformMatrix3fv(glLoc, numValues, GL_FALSE, glm::value_ptr(*values));
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::mat2* values, int32 numValues) {
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniformMatrix2fv(glLoc, numValues, GL_FALSE, glm::value_ptr(*values));
}

//------------------------------------------------------------------------------
void
glRenderMgr::Clear(Channel::Mask channels, const glm::vec4& color, float32 depth, uint8 stencil) {
    o_assert_dbg(this->isValid);
    o_assert2_dbg(this->renderTargetValid, "No render target set!");
    this->stateWrapper->Clear(channels, color, depth, stencil);
}

//------------------------------------------------------------------------------
void
glRenderMgr::Draw(const PrimitiveGroup& primGroup) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(nullptr != this->curMesh);
    o_assert2_dbg(this->renderTargetValid, "No render target set!");
    ORYOL_GL_CHECK_ERROR();

    const IndexType::Code indexType = this->curMesh->GetIndexBufferAttrs().Type;
    if (indexType != IndexType::None) {
        // indexed geometry
        const int32 indexByteSize = IndexType::ByteSize(indexType);
        const GLvoid* indices = (const GLvoid*) (GLintptr) (primGroup.BaseElement * indexByteSize);
        ::glDrawElements(primGroup.PrimType, primGroup.NumElements, indexType, indices);
    }
    else {
        // non-indexed geometry
        ::glDrawArrays(primGroup.PrimType, primGroup.BaseElement, primGroup.NumElements);
    }
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderMgr::Draw(int32 primGroupIndex) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(nullptr != this->curMesh);
    o_assert2_dbg(this->renderTargetValid, "No render target set!");
    
    if (primGroupIndex >= this->curMesh->GetNumPrimitiveGroups()) {
        // this may happen if trying to render a placeholder which doesn't
        // have as many materials as the original mesh, anyway, this isn't
        // a serious error
        return;
    }
    const PrimitiveGroup& primGroup = this->curMesh->GetPrimitiveGroup(primGroupIndex);
    this->Draw(primGroup);
}

//------------------------------------------------------------------------------
void
glRenderMgr::DrawInstanced(const PrimitiveGroup& primGroup, int32 numInstances) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(nullptr != this->curMesh);
    o_assert2_dbg(this->renderTargetValid, "No render target set!");

    ORYOL_GL_CHECK_ERROR();    
    const IndexType::Code indexType = this->curMesh->GetIndexBufferAttrs().Type;
    if (indexType != IndexType::None) {
        // indexed geometry
        const int32 indexByteSize = IndexType::ByteSize(indexType);
        const GLvoid* indices = (const GLvoid*) (GLintptr) (primGroup.BaseElement * indexByteSize);
        glExt::DrawElementsInstanced(primGroup.PrimType, primGroup.NumElements, indexType, indices, numInstances);
    }
    else {
        // non-indexed geometry
        glExt::DrawArraysInstanced(primGroup.PrimType, primGroup.BaseElement, primGroup.NumElements, numInstances);
    }
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderMgr::DrawInstanced(int32 primGroupIndex, int32 numInstances) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(nullptr != this->curMesh);
    o_assert2_dbg(this->renderTargetValid, "No render target set!");    
    
    if (primGroupIndex >= this->curMesh->GetNumPrimitiveGroups()) {
        // this may happen if trying to render a placeholder which doesn't
        // have as many materials as the original mesh, anyway, this isn't
        // a serious error
        return;
    }
    const PrimitiveGroup& primGroup = this->curMesh->GetPrimitiveGroup(primGroupIndex);
    this->DrawInstanced(primGroup, numInstances);
}

//------------------------------------------------------------------------------
void
glRenderMgr::UpdateVertices(mesh* msh, int32 numBytes, const void* data) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(nullptr != msh);
    o_assert(numBytes > 0);
    
    const VertexBufferAttrs& attrs = msh->GetVertexBufferAttrs();
    const Usage::Code vbUsage = attrs.BufferUsage;
    o_assert_dbg((numBytes > 0) && (numBytes <= attrs.ByteSize()));
    o_assert_dbg((vbUsage == Usage::Stream) || (vbUsage == Usage::Dynamic) || (vbUsage == Usage::Static));
    
    uint8 slotIndex = msh->getActiveVertexBufferSlot();
    if (Usage::Dynamic == vbUsage) {
        // if usage if dynamic, rotate slot index to next dynamic vertex buffer
        // to implement double/multi-buffering
        slotIndex++;
        if (slotIndex >= msh->getNumVertexBufferSlots()) {
            slotIndex = 0;
        }
        msh->setActiveVertexBufferSlot(slotIndex);
        msh->setActiveVAOSlot(slotIndex);
    }
    
    GLuint vb = msh->glGetVertexBuffer(slotIndex);
    this->stateWrapper->BindVertexBuffer(vb);
    ::glBufferSubData(GL_ARRAY_BUFFER, 0, numBytes, data);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderMgr::ReadPixels(void* buf, int32 bufNumBytes) {
    o_assert_dbg(this->isValid);
    o_assert_dbg((nullptr != buf) && (bufNumBytes > 0));
    
    GLsizei width, height;
    GLenum format, type;
    if (nullptr == this->curRenderTarget) {
        const DisplayAttrs& attrs = this->displayManager->GetDisplayAttrs();
        width  = attrs.FramebufferWidth;
        height = attrs.FramebufferHeight;
        format = glTypes::AsGLTexImageFormat(attrs.ColorPixelFormat);
        type   = glTypes::AsGLTexImageType(attrs.ColorPixelFormat);
        o_assert((width & 3) == 0);
        o_assert(bufNumBytes >= (width * height * PixelFormat::ByteSize(attrs.ColorPixelFormat)));
    }
    else {
        const TextureAttrs& attrs = this->curRenderTarget->GetTextureAttrs();
        width  = attrs.Width;
        height = attrs.Height;
        format = glTypes::AsGLTexImageFormat(attrs.ColorFormat);
        type   = glTypes::AsGLTexImageType(attrs.ColorFormat);
        o_assert((width & 3) == 0);
        o_assert(bufNumBytes >= (width * height * PixelFormat::ByteSize(attrs.ColorFormat)));
    }
    ::glReadPixels(0, 0, width, height, format, type, buf);
    ORYOL_GL_CHECK_ERROR();
}

} // namespace Render
} // namespace Oryol
