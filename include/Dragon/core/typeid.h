// --------------------------------------------------------
// Dragon
// Copyright(c) 2017 SeetaTech
// Written by Ting Pan
// --------------------------------------------------------

#ifndef DRAGON_CORE_TYPEID_H_
#define DRAGON_CORE_TYPEID_H_

#include <cstdlib>
#include <iostream>
#include <map>

namespace dragon {

typedef intptr_t TypeId;

class TypeMeta {
 public:
    template <typename T>
    struct TypeRegister {
        static TypeId id() {
            static bool type_id_bit[1];
            return (TypeId)(type_id_bit);
        }
    };
    typedef void(*PlacementNew)(void*, size_t);
    typedef void(*TypedCopy)(const void*, void*, size_t);
    typedef void(*TypedDestructor)(void*, size_t);

    TypeMeta()
        : id_(0), itemsize_(0),
          ctor_(nullptr), copy_(nullptr), dtor_(nullptr) {}

    TypeMeta(const TypeMeta& src) 
        : id_(src.id_), itemsize_(src.itemsize_),
        ctor_(src.ctor_), copy_(src.copy_), dtor_(src.dtor_) {}

    TypeMeta& operator = (const TypeMeta& src) {
        if (this == &src) return *this;
        id_ = src.id_;
        itemsize_ = src.itemsize_;
        ctor_ = src.ctor_;
        copy_ = src.copy_;
        dtor_ = src.dtor_;
        return *this;
    }

    bool operator == (const TypeMeta& other) const { 
        return (id_ == other.id_); 
    }

    bool operator != (const TypeMeta& other) const { 
        return (id_ != other.id_); 
    }

    const TypeId& id() const { return id_; }
    const size_t& itemsize() const { return itemsize_; }

    PlacementNew ctor() const { return ctor_; }
    TypedCopy copy() const { return copy_; }
    TypedDestructor dtor() const { return dtor_; }

    template <typename T>
    static TypeId Id() {
        //  return T's id
        //  using a intptr_t as hash key
        return TypeRegister<T>::id();
    }

    template <typename T>
    static size_t Itemsize() { return sizeof(T); }

    template <typename T>
    bool Match() const { return (id_ == Id<T>()); } 

    template <typename T>
    static void Ctor(void* ptr, size_t n) {
        T* typed_ptr = static_cast<T*>(ptr);
        for (unsigned int i = 0; i < n; i++) new(typed_ptr + i) T;
    }

    template <typename T>
    static void Copy(const void* src, void* dst, size_t n) {
        const T* typed_src = static_cast<const T*>(src);
        T* typed_dst = static_cast<T*>(dst);
        for (unsigned int i = 0; i < n; i++) typed_dst[i] = typed_src[i];
    }

    template <typename T>
    static void Dtor(void* ptr, size_t n) {
        T* typed_ptr = static_cast<T*>(ptr);
        for (unsigned int i = 0; i < n; i++) typed_ptr[i].~T();
    }

#define FundMeta std::enable_if<std::is_fundamental<T>::value,TypeMeta>::type
    template <typename T>
    static typename FundMeta Make() {
        return TypeMeta(Id<T>(), Itemsize<T>(), nullptr, nullptr, nullptr);
    }

#define StructMeta std::enable_if<!std::is_fundamental<T>::value && std::is_copy_assignable<T>::value, TypeMeta>::type 

    template<typename T>
    static typename StructMeta Make() {
        return TypeMeta(Id<T>(), Itemsize<T>(), Ctor<T>, Copy<T>, Dtor<T>);
    }

 private:
    TypeMeta(TypeId id, size_t itemsize, 
             PlacementNew ctor, TypedCopy copy, TypedDestructor dtor) 
        : id_(id), itemsize_(itemsize), 
          ctor_(ctor), copy_(copy), dtor_(dtor) {}

 private:
    TypeId id_;
    size_t itemsize_;
    PlacementNew ctor_;
    TypedCopy copy_;
    TypedDestructor dtor_;
};

}    // namespace dragon

#endif    // DRAGON_CORE_TYPEID_H_