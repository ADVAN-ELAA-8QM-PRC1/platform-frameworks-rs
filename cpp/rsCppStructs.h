/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_RSCPPSTRUCTS_H
#define ANDROID_RSCPPSTRUCTS_H

#include "rsDefines.h"
#include "util/RefBase.h"
#include "rsDispatch.h"

#include <vector>
#include <string>

// Every row in an RS allocation is guaranteed to be aligned by this amount
// Every row in a user-backed allocation must be aligned by this amount
#define RS_CPU_ALLOCATION_ALIGNMENT 16

namespace android {
namespace RSC {

typedef void (*ErrorHandlerFunc_t)(uint32_t errorNum, const char *errorText);
typedef void (*MessageHandlerFunc_t)(uint32_t msgNum, const void *msgData, size_t msgLen);

class RS;
class BaseObj;
class Element;
class Type;
class Allocation;
class Script;
class ScriptC;
class Sampler;

 enum RSError {
     RS_SUCCESS = 0,
     RS_ERROR_INVALID_PARAMETER = 1,
     RS_ERROR_RUNTIME_ERROR = 2,
     RS_ERROR_INVALID_ELEMENT = 3,
     RS_ERROR_MAX = 9999

 };

 enum RSYuvFormat {
     RS_YUV_NONE = 0,
     RS_YUV_YV12 = 1,
     RS_YUV_NV21 = 2,
     RS_YUV_MAX = 3
 };

 enum RSInitFlags {
     RS_INIT_SYNCHRONOUS = 1,
     RS_INIT_LOW_LATENCY = 2,
     RS_INIT_MAX = 4
 };


 class RS : public android::RSC::LightRefBase<RS> {

 public:
    RS();
    virtual ~RS();

    bool init(uint32_t flags = 0);

    void setErrorHandler(ErrorHandlerFunc_t func);
    ErrorHandlerFunc_t getErrorHandler() { return mErrorFunc; }

    void setMessageHandler(MessageHandlerFunc_t func);
    MessageHandlerFunc_t getMessageHandler() { return mMessageFunc; }

    void throwError(RSError error, const char *errMsg);
    RSError getError();

    RsContext getContext() { return mContext; }

    void finish();

    static dispatchTable* dispatch;

 private:
    static bool usingNative;
    static bool initDispatch(int targetApi);

    bool init(int targetApi, uint32_t flags);
    static void * threadProc(void *);

    static bool gInitialized;
    static pthread_mutex_t gInitMutex;

    pthread_t mMessageThreadId;
    pid_t mNativeMessageThreadId;
    bool mMessageRun;

    RsDevice mDev;
    RsContext mContext;
    RSError mCurrentError;

    ErrorHandlerFunc_t mErrorFunc;
    MessageHandlerFunc_t mMessageFunc;
    bool mInit;

    struct {
        sp<const Element> U8;
        sp<const Element> U8_2;
        sp<const Element> U8_3;
        sp<const Element> U8_4;
        sp<const Element> I8;
        sp<const Element> I8_2;
        sp<const Element> I8_3;
        sp<const Element> I8_4;
        sp<const Element> U16;
        sp<const Element> U16_2;
        sp<const Element> U16_3;
        sp<const Element> U16_4;
        sp<const Element> I16;
        sp<const Element> I16_2;
        sp<const Element> I16_3;
        sp<const Element> I16_4;
        sp<const Element> U32;
        sp<const Element> U32_2;
        sp<const Element> U32_3;
        sp<const Element> U32_4;
        sp<const Element> I32;
        sp<const Element> I32_2;
        sp<const Element> I32_3;
        sp<const Element> I32_4;
        sp<const Element> U64;
        sp<const Element> U64_2;
        sp<const Element> U64_3;
        sp<const Element> U64_4;
        sp<const Element> I64;
        sp<const Element> I64_2;
        sp<const Element> I64_3;
        sp<const Element> I64_4;
        sp<const Element> F32;
        sp<const Element> F32_2;
        sp<const Element> F32_3;
        sp<const Element> F32_4;
        sp<const Element> F64;
        sp<const Element> F64_2;
        sp<const Element> F64_3;
        sp<const Element> F64_4;
        sp<const Element> BOOLEAN;

        sp<const Element> ELEMENT;
        sp<const Element> TYPE;
        sp<const Element> ALLOCATION;
        sp<const Element> SAMPLER;
        sp<const Element> SCRIPT;
        sp<const Element> MESH;
        sp<const Element> PROGRAM_FRAGMENT;
        sp<const Element> PROGRAM_VERTEX;
        sp<const Element> PROGRAM_RASTER;
        sp<const Element> PROGRAM_STORE;

        sp<const Element> A_8;
        sp<const Element> RGB_565;
        sp<const Element> RGB_888;
        sp<const Element> RGBA_5551;
        sp<const Element> RGBA_4444;
        sp<const Element> RGBA_8888;

        sp<const Element> YUV;

        sp<const Element> MATRIX_4X4;
        sp<const Element> MATRIX_3X3;
        sp<const Element> MATRIX_2X2;
    } mElements;

    struct {
        sp<const Sampler> CLAMP_NEAREST;
        sp<const Sampler> CLAMP_LINEAR;
        sp<const Sampler> CLAMP_LINEAR_MIP_LINEAR;
        sp<const Sampler> WRAP_NEAREST;
        sp<const Sampler> WRAP_LINEAR;
        sp<const Sampler> WRAP_LINEAR_MIP_LINEAR;
        sp<const Sampler> MIRRORED_REPEAT_NEAREST;
        sp<const Sampler> MIRRORED_REPEAT_LINEAR;
        sp<const Sampler> MIRRORED_REPEAT_LINEAR_MIP_LINEAR;
    } mSamplers;
    friend class Sampler;
    friend class Element;
};

class BaseObj : public android::RSC::LightRefBase<BaseObj> {
public:
    void * getID() const;
    virtual ~BaseObj();
    virtual void updateFromNative();
    virtual bool equals(sp<const BaseObj> obj);

protected:
    void *mID;
    sp<RS> mRS;
    std::string mName;

    BaseObj(void *id, sp<RS> rs);
    void checkValid();

    static void * getObjID(sp<const BaseObj> o);

};


class Allocation : public BaseObj {
protected:
    sp<const Type> mType;
    uint32_t mUsage;
    sp<Allocation> mAdaptedAllocation;

    bool mConstrainedLOD;
    bool mConstrainedFace;
    bool mConstrainedY;
    bool mConstrainedZ;
    bool mReadAllowed;
    bool mWriteAllowed;
    uint32_t mSelectedY;
    uint32_t mSelectedZ;
    uint32_t mSelectedLOD;
    RsAllocationCubemapFace mSelectedFace;

    uint32_t mCurrentDimX;
    uint32_t mCurrentDimY;
    uint32_t mCurrentDimZ;
    uint32_t mCurrentCount;

    void * getIDSafe() const;
    void updateCacheInfo(sp<const Type> t);

    Allocation(void *id, sp<RS> rs, sp<const Type> t, uint32_t usage);

    void validateIsInt32();
    void validateIsInt16();
    void validateIsInt8();
    void validateIsFloat32();
    void validateIsObject();

    virtual void updateFromNative();

    void validate2DRange(uint32_t xoff, uint32_t yoff, uint32_t w, uint32_t h);
    void validate3DRange(uint32_t xoff, uint32_t yoff, uint32_t zoff,
                         uint32_t w, uint32_t h, uint32_t d);

public:
    sp<const Type> getType() const {
        return mType;
    }

    void syncAll(RsAllocationUsageType srcLocation);
    void ioSendOutput();
    void ioGetInput();

    void generateMipmaps();

    void copy1DRangeFrom(uint32_t off, size_t count, const void *data);
    void copy1DRangeFrom(uint32_t off, size_t count, sp<const Allocation> data, uint32_t dataOff);

    void copy1DRangeTo(uint32_t off, size_t count, void *data);

    void copy1DFrom(const void* data);
    void copy1DTo(void* data);

    void copy2DRangeFrom(uint32_t xoff, uint32_t yoff, uint32_t w, uint32_t h,
                         const void *data);

    void copy2DRangeTo(uint32_t xoff, uint32_t yoff, uint32_t w, uint32_t h,
                       void *data);

    void copy2DRangeFrom(uint32_t xoff, uint32_t yoff, uint32_t w, uint32_t h,
                         sp<const Allocation> data, uint32_t dataXoff, uint32_t dataYoff);

    void copy2DStridedFrom(uint32_t xoff, uint32_t yoff, uint32_t w, uint32_t h,
                           const void *data, size_t stride);
    void copy2DStridedFrom(const void *data, size_t stride);

    void copy2DStridedTo(uint32_t xoff, uint32_t yoff, uint32_t w, uint32_t h,
                         void *data, size_t stride);
    void copy2DStridedTo(void *data, size_t stride);

    void copy3DRangeFrom(uint32_t xoff, uint32_t yoff, uint32_t zoff, uint32_t w,
                         uint32_t h, uint32_t d, const void* data);

    void copy3DRangeFrom(uint32_t xoff, uint32_t yoff, uint32_t zoff,
                         uint32_t w, uint32_t h, uint32_t d,
                         sp<const Allocation> data,
                         uint32_t dataXoff, uint32_t dataYoff, uint32_t dataZoff);

    static sp<Allocation> createTyped(sp<RS> rs, sp<const Type> type,
                                   RsAllocationMipmapControl mips, uint32_t usage);
    static sp<Allocation> createTyped(sp<RS> rs, sp<const Type> type,
                                   RsAllocationMipmapControl mips, uint32_t usage, void * pointer);

    static sp<Allocation> createTyped(sp<RS> rs, sp<const Type> type,
                                   uint32_t usage = RS_ALLOCATION_USAGE_SCRIPT);
    static sp<Allocation> createSized(sp<RS> rs, sp<const Element> e, size_t count,
                                   uint32_t usage = RS_ALLOCATION_USAGE_SCRIPT);
    static sp<Allocation> createSized2D(sp<RS> rs, sp<const Element> e,
                                        size_t x, size_t y,
                                        uint32_t usage = RS_ALLOCATION_USAGE_SCRIPT);


};

class Element : public BaseObj {
public:
    bool isComplex();
    size_t getSubElementCount() {
        return mVisibleElementMap.size();
    }

    sp<const Element> getSubElement(uint32_t index);
    const char * getSubElementName(uint32_t index);
    size_t getSubElementArraySize(uint32_t index);
    uint32_t getSubElementOffsetBytes(uint32_t index);
    RsDataType getDataType() const {
        return mType;
    }

    RsDataKind getDataKind() const {
        return mKind;
    }

    size_t getSizeBytes() const {
        return mSizeBytes;
    }

    uint32_t getVectorSize() const {
        return mVectorSize;
    }

    static sp<const Element> BOOLEAN(sp<RS> rs);
    static sp<const Element> U8(sp<RS> rs);
    static sp<const Element> I8(sp<RS> rs);
    static sp<const Element> U16(sp<RS> rs);
    static sp<const Element> I16(sp<RS> rs);
    static sp<const Element> U32(sp<RS> rs);
    static sp<const Element> I32(sp<RS> rs);
    static sp<const Element> U64(sp<RS> rs);
    static sp<const Element> I64(sp<RS> rs);
    static sp<const Element> F32(sp<RS> rs);
    static sp<const Element> F64(sp<RS> rs);
    static sp<const Element> ELEMENT(sp<RS> rs);
    static sp<const Element> TYPE(sp<RS> rs);
    static sp<const Element> ALLOCATION(sp<RS> rs);
    static sp<const Element> SAMPLER(sp<RS> rs);
    static sp<const Element> SCRIPT(sp<RS> rs);
    static sp<const Element> MESH(sp<RS> rs);
    static sp<const Element> PROGRAM_FRAGMENT(sp<RS> rs);
    static sp<const Element> PROGRAM_VERTEX(sp<RS> rs);
    static sp<const Element> PROGRAM_RASTER(sp<RS> rs);
    static sp<const Element> PROGRAM_STORE(sp<RS> rs);

    static sp<const Element> A_8(sp<RS> rs);
    static sp<const Element> RGB_565(sp<RS> rs);
    static sp<const Element> RGB_888(sp<RS> rs);
    static sp<const Element> RGBA_5551(sp<RS> rs);
    static sp<const Element> RGBA_4444(sp<RS> rs);
    static sp<const Element> RGBA_8888(sp<RS> rs);

    static sp<const Element> F32_2(sp<RS> rs);
    static sp<const Element> F32_3(sp<RS> rs);
    static sp<const Element> F32_4(sp<RS> rs);
    static sp<const Element> F64_2(sp<RS> rs);
    static sp<const Element> F64_3(sp<RS> rs);
    static sp<const Element> F64_4(sp<RS> rs);
    static sp<const Element> U8_2(sp<RS> rs);
    static sp<const Element> U8_3(sp<RS> rs);
    static sp<const Element> U8_4(sp<RS> rs);
    static sp<const Element> I8_2(sp<RS> rs);
    static sp<const Element> I8_3(sp<RS> rs);
    static sp<const Element> I8_4(sp<RS> rs);
    static sp<const Element> U16_2(sp<RS> rs);
    static sp<const Element> U16_3(sp<RS> rs);
    static sp<const Element> U16_4(sp<RS> rs);
    static sp<const Element> I16_2(sp<RS> rs);
    static sp<const Element> I16_3(sp<RS> rs);
    static sp<const Element> I16_4(sp<RS> rs);
    static sp<const Element> U32_2(sp<RS> rs);
    static sp<const Element> U32_3(sp<RS> rs);
    static sp<const Element> U32_4(sp<RS> rs);
    static sp<const Element> I32_2(sp<RS> rs);
    static sp<const Element> I32_3(sp<RS> rs);
    static sp<const Element> I32_4(sp<RS> rs);
    static sp<const Element> U64_2(sp<RS> rs);
    static sp<const Element> U64_3(sp<RS> rs);
    static sp<const Element> U64_4(sp<RS> rs);
    static sp<const Element> I64_2(sp<RS> rs);
    static sp<const Element> I64_3(sp<RS> rs);
    static sp<const Element> I64_4(sp<RS> rs);
    static sp<const Element> YUV(sp<RS> rs);
    static sp<const Element> MATRIX_4X4(sp<RS> rs);
    static sp<const Element> MATRIX_3X3(sp<RS> rs);
    static sp<const Element> MATRIX_2X2(sp<RS> rs);

    void updateFromNative();
    static sp<const Element> createUser(sp<RS> rs, RsDataType dt);
    static sp<const Element> createVector(sp<RS> rs, RsDataType dt, uint32_t size);
    static sp<const Element> createPixel(sp<RS> rs, RsDataType dt, RsDataKind dk);
    bool isCompatible(sp<const Element>e) const;

    class Builder {
    private:
        sp<RS> mRS;
        std::vector<sp<Element> > mElements;
        std::vector<std::string> mElementNames;
        std::vector<uint32_t> mArraySizes;
        bool mSkipPadding;

    public:
        Builder(sp<RS> rs);
        ~Builder();
        void add(sp<Element> e, std::string &name, uint32_t arraySize = 1);
        sp<const Element> create();
    };

protected:
    Element(void *id, sp<RS> rs,
            std::vector<sp<Element> > &elements,
            std::vector<std::string> &elementNames,
            std::vector<uint32_t> &arraySizes);
    Element(void *id, sp<RS> rs, RsDataType dt, RsDataKind dk, bool norm, uint32_t size);
    Element(sp<RS> rs);
    virtual ~Element();

private:
    void updateVisibleSubElements();

    std::vector<sp<Element> > mElements;
    std::vector<std::string> mElementNames;
    std::vector<uint32_t> mArraySizes;
    std::vector<uint32_t> mVisibleElementMap;
    std::vector<uint32_t> mOffsetInBytes;

    RsDataType mType;
    RsDataKind mKind;
    bool mNormalized;
    size_t mSizeBytes;
    size_t mVectorSize;
};

class FieldPacker {
protected:
    unsigned char* mData;
    size_t mPos;
    size_t mLen;

public:
    FieldPacker(size_t len)
        : mPos(0), mLen(len) {
            mData = new unsigned char[len];
        }

    virtual ~FieldPacker() {
        delete [] mData;
    }

    void align(size_t v) {
        if ((v & (v - 1)) != 0) {
            //            ALOGE("Non-power-of-two alignment: %zu", v);
            return;
        }

        while ((mPos & (v - 1)) != 0) {
            mData[mPos++] = 0;
        }
    }

    void reset() {
        mPos = 0;
    }

    void reset(size_t i) {
        if (i >= mLen) {
            //            ALOGE("Out of bounds: i (%zu) >= len (%zu)", i, mLen);
            return;
        }
        mPos = i;
    }

    void skip(size_t i) {
        size_t res = mPos + i;
        if (res > mLen) {
            //            ALOGE("Exceeded buffer length: i (%zu) > len (%zu)", i, mLen);
            return;
        }
        mPos = res;
    }

    void* getData() const {
        return mData;
    }

    size_t getLength() const {
        return mLen;
    }

    template <typename T>
        void add(T t) {
        align(sizeof(t));
        if (mPos + sizeof(t) <= mLen) {
            memcpy(&mData[mPos], &t, sizeof(t));
            mPos += sizeof(t);
        }
    }

    /*
      void add(rs_matrix4x4 m) {
      for (size_t i = 0; i < 16; i++) {
      add(m.m[i]);
      }
      }

      void add(rs_matrix3x3 m) {
      for (size_t i = 0; i < 9; i++) {
      add(m.m[i]);
      }
      }

      void add(rs_matrix2x2 m) {
      for (size_t i = 0; i < 4; i++) {
      add(m.m[i]);
      }
      }
    */

    void add(sp<BaseObj> obj) {
        if (obj != NULL) {
            add((uint32_t) (uintptr_t) obj->getID());
        } else {
            add((uint32_t) 0);
        }
    }
};


class Type : public BaseObj {
protected:
    friend class Allocation;

    uint32_t mDimX;
    uint32_t mDimY;
    uint32_t mDimZ;
    RSYuvFormat mYuvFormat;
    bool mDimMipmaps;
    bool mDimFaces;
    size_t mElementCount;
    sp<const Element> mElement;

    Type(void *id, sp<RS> rs);

    void calcElementCount();
    virtual void updateFromNative();

public:

    RSYuvFormat getYuvFormat() const {
        return mYuvFormat;
    }

    sp<const Element> getElement() const {
        return mElement;
    }

    uint32_t getX() const {
        return mDimX;
    }

    uint32_t getY() const {
        return mDimY;
    }

    uint32_t getZ() const {
        return mDimZ;
    }

    bool hasMipmaps() const {
        return mDimMipmaps;
    }

    bool hasFaces() const {
        return mDimFaces;
    }

    size_t getCount() const {
        return mElementCount;
    }

    size_t getSizeBytes() const {
        return mElementCount * mElement->getSizeBytes();
    }

    static sp<const Type> create(sp<RS> rs, sp<const Element> e, uint32_t dimX, uint32_t dimY, uint32_t dimZ);

    class Builder {
    protected:
        sp<RS> mRS;
        uint32_t mDimX;
        uint32_t mDimY;
        uint32_t mDimZ;
        RSYuvFormat mYuvFormat;
        bool mDimMipmaps;
        bool mDimFaces;
        sp<const Element> mElement;

    public:
        Builder(sp<RS> rs, sp<const Element> e);

        void setX(uint32_t value);
        void setY(uint32_t value);
        void setZ(uint32_t value);
        void setYuvFormat(RSYuvFormat format);
        void setMipmaps(bool value);
        void setFaces(bool value);
        sp<const Type> create();
    };

};

class Script : public BaseObj {
private:

protected:
    Script(void *id, sp<RS> rs);
    void forEach(uint32_t slot, sp<const Allocation> in, sp<const Allocation> out,
            const void *v, size_t) const;
    void bindAllocation(sp<Allocation> va, uint32_t slot) const;
    void setVar(uint32_t index, const void *, size_t len) const;
    void setVar(uint32_t index, sp<const BaseObj> o) const;
    void invoke(uint32_t slot, const void *v, size_t len) const;


    void invoke(uint32_t slot) const {
        invoke(slot, NULL, 0);
    }
    void setVar(uint32_t index, float v) const {
        setVar(index, &v, sizeof(v));
    }
    void setVar(uint32_t index, double v) const {
        setVar(index, &v, sizeof(v));
    }
    void setVar(uint32_t index, int32_t v) const {
        setVar(index, &v, sizeof(v));
    }
    void setVar(uint32_t index, int64_t v) const {
        setVar(index, &v, sizeof(v));
    }
    void setVar(uint32_t index, bool v) const {
        setVar(index, &v, sizeof(v));
    }

public:
    class FieldBase {
    protected:
        sp<const Element> mElement;
        sp<Allocation> mAllocation;

        void init(sp<RS> rs, uint32_t dimx, uint32_t usages = 0);

    public:
        sp<const Element> getElement() {
            return mElement;
        }

        sp<const Type> getType() {
            return mAllocation->getType();
        }

        sp<const Allocation> getAllocation() {
            return mAllocation;
        }

        //void updateAllocation();
    };
};

class ScriptC : public Script {
protected:
    ScriptC(sp<RS> rs,
            const void *codeTxt, size_t codeLength,
            const char *cachedName, size_t cachedNameLength,
            const char *cacheDir, size_t cacheDirLength);

};

class ScriptIntrinsic : public Script {
 protected:
    sp<const Element> mElement;
    ScriptIntrinsic(sp<RS> rs, int id, sp<const Element> e);
    virtual ~ScriptIntrinsic();
};

class ScriptIntrinsic3DLUT : public ScriptIntrinsic {
 private:
    ScriptIntrinsic3DLUT(sp<RS> rs, sp<const Element> e);
 public:
    static sp<ScriptIntrinsic3DLUT> create(sp<RS> rs, sp<const Element> e);
    void forEach(sp<Allocation> ain, sp<Allocation> aout);
    void setLUT(sp<Allocation> lut);
};

class ScriptIntrinsicBlend : public ScriptIntrinsic {
 private:
    ScriptIntrinsicBlend(sp<RS> rs, sp<const Element> e);
 public:
    static sp<ScriptIntrinsicBlend> create(sp<RS> rs, sp<const Element> e);
    void blendClear(sp<Allocation> in, sp<Allocation> out);
    void blendSrc(sp<Allocation> in, sp<Allocation> out);
    void blendDst(sp<Allocation> in, sp<Allocation> out);
    void blendSrcOver(sp<Allocation> in, sp<Allocation> out);
    void blendDstOver(sp<Allocation> in, sp<Allocation> out);
    void blendSrcIn(sp<Allocation> in, sp<Allocation> out);
    void blendDstIn(sp<Allocation> in, sp<Allocation> out);
    void blendSrcOut(sp<Allocation> in, sp<Allocation> out);
    void blendDstOut(sp<Allocation> in, sp<Allocation> out);
    void blendSrcAtop(sp<Allocation> in, sp<Allocation> out);
    void blendDstAtop(sp<Allocation> in, sp<Allocation> out);
    void blendXor(sp<Allocation> in, sp<Allocation> out);
    void blendMultiply(sp<Allocation> in, sp<Allocation> out);
    void blendAdd(sp<Allocation> in, sp<Allocation> out);
    void blendSubtract(sp<Allocation> in, sp<Allocation> out);
};

class ScriptIntrinsicBlur : public ScriptIntrinsic {
 private:
    ScriptIntrinsicBlur(sp<RS> rs, sp<const Element> e);
 public:
    static sp<ScriptIntrinsicBlur> create(sp<RS> rs, sp<const Element> e);
    void setInput(sp<Allocation> in);
    void forEach(sp<Allocation> out);
    void setRadius(float radius);
};

class ScriptIntrinsicColorMatrix : public ScriptIntrinsic {
 private:
    ScriptIntrinsicColorMatrix(sp<RS> rs, sp<const Element> e);
 public:
    static sp<ScriptIntrinsicColorMatrix> create(sp<RS> rs);
    void forEach(sp<Allocation> in, sp<Allocation> out);
    void setAdd(float* add);
    void setColorMatrix3(float* m);
    void setColorMatrix4(float* m);
    void setGreyscale();
    void setRGBtoYUV();
    void setYUVtoRGB();
};

class ScriptIntrinsicConvolve3x3 : public ScriptIntrinsic {
 private:
    ScriptIntrinsicConvolve3x3(sp<RS> rs, sp<const Element> e);
 public:
    static sp<ScriptIntrinsicConvolve3x3> create(sp<RS> rs, sp<const Element> e);
    void setInput(sp<Allocation> in);
    void forEach(sp<Allocation> out);
    void setCoefficients(float* v);
};

class ScriptIntrinsicConvolve5x5 : public ScriptIntrinsic {
 private:
    ScriptIntrinsicConvolve5x5(sp<RS> rs, sp<const Element> e);
 public:
    static sp<ScriptIntrinsicConvolve5x5> create(sp<RS> rs, sp<const Element> e);
    void setInput(sp<Allocation> in);
    void forEach(sp<Allocation> out);
    void setCoefficients(float* v);
};

class ScriptIntrinsicHistogram : public ScriptIntrinsic {
 private:
    ScriptIntrinsicHistogram(sp<RS> rs, sp<const Element> e);
    sp<Allocation> mOut;
 public:
    static sp<ScriptIntrinsicHistogram> create(sp<RS> rs);
    void setOutput(sp<Allocation> aout);
    void setDotCoefficients(float r, float g, float b, float a);
    void forEach(sp<Allocation> ain);
    void forEach_dot(sp<Allocation> ain);
};

class ScriptIntrinsicLUT : public ScriptIntrinsic {
 private:
    sp<Allocation> LUT;
    bool mDirty;
    unsigned char mCache[1024];
    void setTable(unsigned int offset, unsigned char base, unsigned int length, unsigned char* lutValues);
    ScriptIntrinsicLUT(sp<RS> rs, sp<const Element> e);

 public:
    static sp<ScriptIntrinsicLUT> create(sp<RS> rs, sp<const Element> e);
    void forEach(sp<Allocation> ain, sp<Allocation> aout);
    void setRed(unsigned char base, unsigned int length, unsigned char* lutValues);
    void setGreen(unsigned char base, unsigned int length, unsigned char* lutValues);
    void setBlue(unsigned char base, unsigned int length, unsigned char* lutValues);
    void setAlpha(unsigned char base, unsigned int length, unsigned char* lutValues);
    virtual ~ScriptIntrinsicLUT();
};

class ScriptIntrinsicYuvToRGB : public ScriptIntrinsic {
 private:
    ScriptIntrinsicYuvToRGB(sp<RS> rs, sp<const Element> e);
 public:
    static sp<ScriptIntrinsicYuvToRGB> create(sp<RS> rs, sp<const Element> e);
    void setInput(sp<Allocation> in);
    void forEach(sp<Allocation> out);

};


 class Sampler : public BaseObj {
 private:
    Sampler(sp<RS> rs, void* id);
    RsSamplerValue mMin;
    RsSamplerValue mMag;
    RsSamplerValue mWrapS;
    RsSamplerValue mWrapT;
    RsSamplerValue mWrapR;
    float mAniso;

 public:
    static sp<Sampler> create(sp<RS> rs, RsSamplerValue min, RsSamplerValue mag, RsSamplerValue wrapS, RsSamplerValue wrapT, float anisotropy);

    RsSamplerValue getMinification();
    RsSamplerValue getMagnification();
    RsSamplerValue getWrapS();
    RsSamplerValue getWrapT();
    float getAnisotropy();

    sp<const Sampler> CLAMP_NEAREST(sp<RS> rs);
    sp<const Sampler> CLAMP_LINEAR(sp<RS> rs);
    sp<const Sampler> CLAMP_LINEAR_MIP_LINEAR(sp<RS> rs);
    sp<const Sampler> WRAP_NEAREST(sp<RS> rs);
    sp<const Sampler> WRAP_LINEAR(sp<RS> rs);
    sp<const Sampler> WRAP_LINEAR_MIP_LINEAR(sp<RS> rs);
    sp<const Sampler> MIRRORED_REPEAT_NEAREST(sp<RS> rs);
    sp<const Sampler> MIRRORED_REPEAT_LINEAR(sp<RS> rs);
    sp<const Sampler> MIRRORED_REPEAT_LINEAR_MIP_LINEAR(sp<RS> rs);

};

class Byte2 {
 public:
  int8_t x, y;

  Byte2(int8_t initX, int8_t initY)
    : x(initX), y(initY) {}
  Byte2() : x(0), y(0) {}
};

class Byte3 {
 public:
  int8_t x, y, z;

  Byte3(int8_t initX, int8_t initY, int8_t initZ)
    : x(initX), y(initY), z(initZ) {}
  Byte3() : x(0), y(0), z(0) {}
};

class Byte4 {
 public:
  int8_t x, y, z, w;

  Byte4(int8_t initX, int8_t initY, int8_t initZ, int8_t initW)
    : x(initX), y(initY), z(initZ), w(initW) {}
  Byte4() : x(0), y(0), z(0), w(0) {}
};

class UByte2 {
 public:
  uint8_t x, y;

  UByte2(uint8_t initX, uint8_t initY)
    : x(initX), y(initY) {}
  UByte2() : x(0), y(0) {}
};

class UByte3 {
 public:
  uint8_t x, y, z;

  UByte3(uint8_t initX, uint8_t initY, uint8_t initZ)
    : x(initX), y(initY), z(initZ) {}
  UByte3() : x(0), y(0), z(0) {}
};

class UByte4 {
 public:
  uint8_t x, y, z, w;

  UByte4(uint8_t initX, uint8_t initY, uint8_t initZ, uint8_t initW)
    : x(initX), y(initY), z(initZ), w(initW) {}
  UByte4() : x(0), y(0), z(0), w(0) {}
};

class Short2 {
 public:
  short x, y;

  Short2(short initX, short initY)
    : x(initX), y(initY) {}
  Short2() : x(0), y(0) {}
};

class Short3 {
 public:
  short x, y, z;

  Short3(short initX, short initY, short initZ)
    : x(initX), y(initY), z(initZ) {}
  Short3() : x(0), y(0), z(0) {}
};

class Short4 {
 public:
  short x, y, z, w;

  Short4(short initX, short initY, short initZ, short initW)
    : x(initX), y(initY), z(initZ), w(initW) {}
  Short4() : x(0), y(0), z(0), w(0) {}
};

class UShort2 {
 public:
  uint16_t x, y;

  UShort2(uint16_t initX, uint16_t initY)
    : x(initX), y(initY) {}
  UShort2() : x(0), y(0) {}
};

class UShort3 {
 public:
  uint16_t x, y, z;

  UShort3(uint16_t initX, uint16_t initY, uint16_t initZ)
    : x(initX), y(initY), z(initZ) {}
  UShort3() : x(0), y(0), z(0) {}
};

class UShort4 {
 public:
  uint16_t x, y, z, w;

  UShort4(uint16_t initX, uint16_t initY, uint16_t initZ, uint16_t initW)
    : x(initX), y(initY), z(initZ), w(initW) {}
  UShort4() : x(0), y(0), z(0), w(0) {}
};

class Int2 {
 public:
  int x, y;

  Int2(int initX, int initY)
    : x(initX), y(initY) {}
  Int2() : x(0), y(0) {}
};

class Int3 {
 public:
  int x, y, z;

  Int3(int initX, int initY, int initZ)
    : x(initX), y(initY), z(initZ) {}
  Int3() : x(0), y(0), z(0) {}
};

class Int4 {
 public:
  int x, y, z, w;

  Int4(int initX, int initY, int initZ, int initW)
    : x(initX), y(initY), z(initZ), w(initW) {}
  Int4() : x(0), y(0), z(0), w(0) {}
};

class UInt2 {
 public:
  uint32_t x, y;

  UInt2(uint32_t initX, uint32_t initY)
    : x(initX), y(initY) {}
  UInt2() : x(0), y(0) {}
};

class UInt3 {
 public:
  uint32_t x, y, z;

  UInt3(uint32_t initX, uint32_t initY, uint32_t initZ)
    : x(initX), y(initY), z(initZ) {}
  UInt3() : x(0), y(0), z(0) {}
};

class UInt4 {
 public:
  uint32_t x, y, z, w;

  UInt4(uint32_t initX, uint32_t initY, uint32_t initZ, uint32_t initW)
    : x(initX), y(initY), z(initZ), w(initW) {}
  UInt4() : x(0), y(0), z(0), w(0) {}
};

class Long2 {
 public:
  int64_t x, y;

  Long2(int64_t initX, int64_t initY)
    : x(initX), y(initY) {}
  Long2() : x(0), y(0) {}
};

class Long3 {
 public:
  int64_t x, y, z;

  Long3(int64_t initX, int64_t initY, int64_t initZ)
    : x(initX), y(initY), z(initZ) {}
  Long3() : x(0), y(0), z(0) {}
};

class Long4 {
 public:
  int64_t x, y, z, w;

  Long4(int64_t initX, int64_t initY, int64_t initZ, int64_t initW)
    : x(initX), y(initY), z(initZ), w(initW) {}
  Long4() : x(0), y(0), z(0), w(0) {}
};

class ULong2 {
 public:
  uint64_t x, y;

  ULong2(uint64_t initX, uint64_t initY)
    : x(initX), y(initY) {}
  ULong2() : x(0), y(0) {}
};

class ULong3 {
 public:
  uint64_t x, y, z;

  ULong3(uint64_t initX, uint64_t initY, uint64_t initZ)
    : x(initX), y(initY), z(initZ) {}
  ULong3() : x(0), y(0), z(0) {}
};

class ULong4 {
 public:
  uint64_t x, y, z, w;

  ULong4(uint64_t initX, uint64_t initY, uint64_t initZ, uint64_t initW)
    : x(initX), y(initY), z(initZ), w(initW) {}
  ULong4() : x(0), y(0), z(0), w(0) {}
};

class Float2 {
 public:
  float x, y;

  Float2(float initX, float initY)
    : x(initX), y(initY) {}
  Float2() : x(0), y(0) {}
};

class Float3 {
 public:
  float x, y, z;

  Float3(float initX, float initY, float initZ)
    : x(initX), y(initY), z(initZ) {}
  Float3() : x(0.f), y(0.f), z(0.f) {}
};

class Float4 {
 public:
  float x, y, z, w;

  Float4(float initX, float initY, float initZ, float initW)
    : x(initX), y(initY), z(initZ), w(initW) {}
  Float4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
};

class Double2 {
 public:
  double x, y;

  Double2(double initX, double initY)
    : x(initX), y(initY) {}
  Double2() : x(0), y(0) {}
};

class Double3 {
 public:
  double x, y, z;

  Double3(double initX, double initY, double initZ)
    : x(initX), y(initY), z(initZ) {}
  Double3() : x(0), y(0), z(0) {}
};

class Double4 {
 public:
  double x, y, z, w;

  Double4(double initX, double initY, double initZ, double initW)
    : x(initX), y(initY), z(initZ), w(initW) {}
  Double4() : x(0), y(0), z(0), w(0) {}
};

}

}

#endif
