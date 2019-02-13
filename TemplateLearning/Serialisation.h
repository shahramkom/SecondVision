#pragma once
// could be found here: https://gist.github.com/VictorLaskin/1fb078d7f4ac78857f48
#include "MVJSON.h"


// Immutable serialisable data structures in C++11
// Example code from http://vitiy.info/immutable-data-and-serialisation-in-cpp11/
// Written by Victor Laskin (victor.laskin@gmail.com)

#define SERIALIZE_PRIVATE_KEY(NAME,VAL) #NAME "." #VAL
#define SERIALIZE_PRIVATE_SET(NAME,VAL) fields->set(SERIALIZE_PRIVATE_KEY(NAME,VAL), VAL);
#define SERIALIZE_PRIVATE_GET(NAME,VAL) fields->get(SERIALIZE_PRIVATE_KEY(NAME,VAL), VAL);

#define SERIALIZE_PRIVATE_DUP1(M,NAME,A) M(NAME,A)
#define SERIALIZE_PRIVATE_DUP2(M,NAME,A,B) M(NAME,A) M(NAME,B)
#define SERIALIZE_PRIVATE_DUP3(M,NAME,A,B,C) M(NAME,A) SERIALIZE_PRIVATE_DUP2(M,NAME,B,C)
#define SERIALIZE_PRIVATE_DUP4(M,NAME,A,B,C,D) M(NAME,A) SERIALIZE_PRIVATE_DUP3(M,NAME,B,C,D)
#define SERIALIZE_PRIVATE_DUP5(M,NAME,A,B,C,D,E) M(NAME,A) SERIALIZE_PRIVATE_DUP4(M,NAME,B,C,D,E)
#define SERIALIZE_PRIVATE_DUP6(M,NAME,A,B,C,D,E,F) M(NAME,A) SERIALIZE_PRIVATE_DUP5(M,NAME,B,C,D,E,F)
#define SERIALIZE_PRIVATE_DUP7(M,NAME,A,B,C,D,E,F,G) M(NAME,A) SERIALIZE_PRIVATE_DUP6(M,NAME,B,C,D,E,F,G)
#define SERIALIZE_PRIVATE_DUP8(M,NAME,A,B,C,D,E,F,G,H) M(NAME,A) SERIALIZE_PRIVATE_DUP7(M,NAME,B,C,D,E,F,G,H)
#define SERIALIZE_PRIVATE_DUP9(M,NAME,A,B,C,D,E,F,G,H,I) M(NAME,A) SERIALIZE_PRIVATE_DUP8(M,NAME,B,C,D,E,F,G,H,I)
#define SERIALIZE_PRIVATE_DUP10(ME,NAME,A,B,C,D,E,F,G,H,I,K) ME(NAME,A) SERIALIZE_PRIVATE_DUP9(ME,NAME,B,C,D,E,F,G,H,I,K)
#define SERIALIZE_PRIVATE_DUP11(ME,NAME,A,B,C,D,E,F,G,H,I,K,L) ME(NAME,A) SERIALIZE_PRIVATE_DUP10(ME,NAME,B,C,D,E,F,G,H,I,K,L)
#define SERIALIZE_PRIVATE_DUP12(ME,NAME,A,B,C,D,E,F,G,H,I,K,L,M) ME(NAME,A) SERIALIZE_PRIVATE_DUP11(ME,NAME,B,C,D,E,F,G,H,I,K,L,M)
#define SERIALIZE_PRIVATE_DUP13(ME,NAME,A,B,C,D,E,F,G,H,I,K,L,M,N) ME(NAME,A) SERIALIZE_PRIVATE_DUP12(ME,NAME,B,C,D,E,F,G,H,I,K,L,M,N)
#define SERIALIZE_PRIVATE_DUP14(ME,NAME,A,B,C,D,E,F,G,H,I,K,L,M,N,O) ME(NAME,A) SERIALIZE_PRIVATE_DUP13(ME,NAME,B,C,D,E,F,G,H,I,K,L,M,N,O)
#define SERIALIZE_PRIVATE_DUP15(ME,NAME,A,B,C,D,E,F,G,H,I,K,L,M,N,O,P) ME(NAME,A) SERIALIZE_PRIVATE_DUP14(ME,NAME,B,C,D,E,F,G,H,I,K,L,M,N,O,P)
#define SERIALIZE_PRIVATE_DUP16(ME,NAME,A,B,C,D,E,F,G,H,I,K,L,M,N,O,P,R) ME(NAME,A) SERIALIZE_PRIVATE_DUP15(ME,NAME,B,C,D,E,F,G,H,I,K,L,M,N,O,P,R)
#define SERIALIZE_PRIVATE_DUP17(ME,NAME,A,B,C,D,E,F,G,H,I,K,L,M,N,O,P,R,S) ME(NAME,A) SERIALIZE_PRIVATE_DUP16(ME,NAME,B,C,D,E,F,G,H,I,K,L,M,N,O,P,R,S)
#define SERIALIZE_PRIVATE_DUP18(ME,NAME,A,B,C,D,E,F,G,H,I,K,L,M,N,O,P,R,S,T) ME(NAME,A) SERIALIZE_PRIVATE_DUP17(ME,NAME,B,C,D,E,F,G,H,I,K,L,M,N,O,P,R,S,T)
#define SERIALIZE_PRIVATE_DUP19(ME,NAME,A,B,C,D,E,F,G,H,I,K,L,M,N,O,P,R,S,T,Q) ME(NAME,A) SERIALIZE_PRIVATE_DUP18(ME,NAME,B,C,D,E,F,G,H,I,K,L,M,N,O,P,R,S,T,Q)
#define SERIALIZE_PRIVATE_DUP20(ME,NAME,A,B,C,D,E,F,G,H,I,K,L,M,N,O,P,R,S,T,Q,Y) ME(NAME,A) SERIALIZE_PRIVATE_DUP19(ME,NAME,B,C,D,E,F,G,H,I,K,L,M,N,O,P,R,S,T,Q,Y)

#define SERIALIZE_PRIVATE_EXPAND(x) x
#define SERIALIZE_PRIVATE_DUPCALL(N,M,NAME,...) SERIALIZE_PRIVATE_DUP ## N (M,NAME,__VA_ARGS__)

// counter of macro arguments + actual call
#define SERIALIZE_PRIVATE_VA_NARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10, _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, N, ...) N
#define SERIALIZE_PRIVATE_VA_NARGS(...) SERIALIZE_PRIVATE_EXPAND(SERIALIZE_PRIVATE_VA_NARGS_IMPL(__VA_ARGS__, 20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1))
#define SERIALIZE_PRIVATE_VARARG_IMPL2(M,NAME,base, count, ...) SERIALIZE_PRIVATE_EXPAND(base##count(M,NAME,__VA_ARGS__))
#define SERIALIZE_PRIVATE_VARARG_IMPL(M,NAME,base, count, ...) SERIALIZE_PRIVATE_EXPAND(SERIALIZE_PRIVATE_VARARG_IMPL2(M,NAME,base, count, __VA_ARGS__))
#define SERIALIZE_PRIVATE_VARARG(M,NAME,base, ...) SERIALIZE_PRIVATE_EXPAND(SERIALIZE_PRIVATE_VARARG_IMPL(M,NAME, base, SERIALIZE_PRIVATE_VA_NARGS(__VA_ARGS__), __VA_ARGS__))

#define SERIALIZE_PRIVATE_DUPAUTO(M,NAME,...) SERIALIZE_PRIVATE_EXPAND(SERIALIZE_PRIVATE_VARARG(M,NAME,SERIALIZE_PRIVATE_DUP, __VA_ARGS__))

  // ------------------------ IMMUTABLE SERIALIZATION ADDITION -------------------------->


#define SERIALIZE_PRIVATE_APPENDTOJSON(NAME,VAL) w.add(#VAL,VAL);

#define SERIALIZE_PRIVATE_FROMJSON(NAME,VAL) node->getValue<decltype(VAL)>(#VAL),


#define SERIALIZE_PRIVATE_CTORIMMUTABLEDECL(NAME,VAL) decltype(VAL) VAL,

#define SERIALIZE_PRIVATE_CTORIMMUTABLEDECLTYPENONCONST(NAME,VAL) typename std::remove_const<decltype(VAL)>::type,

#define SERIALIZE_PRIVATE_CTORIMMUTABLEPARAM(NAME,VAL) VAL(VAL),

#define SERIALIZE_PRIVATE_CTORIMMUTABLEVAL(NAME,VAL) VAL,

#define SERIALIZE_PRIVATE_CTORIMMUTABLECOPY(NAME,VAL) VAL(other.VAL),

#define SERIALIZE_PRIVATE_COMPAREIMMUTABLE(NAME,VAL) if (other.VAL==VAL)

#define SERIALIZE_PRIVATE_GETINDEX(NAME,VAL) NAME::_index_of_##VAL

#define SERIALIZE_PRIVATE_CTORFROMTUPLE(NAME,VAL) VAL(std::get<SERIALIZE_PRIVATE_GETINDEX(NAME,VAL)>(vars)),

#define SERIALIZE_PRIVATE_CHECKINDEX(NAME,VAL) (name == #VAL) ? __COUNTER__ - offset - 1 :

#define SERIALIZE_PRIVATE_FIELDINDEX(NAME,VAL) static const int _index_of_##VAL = __COUNTER__ - _index_offset - 1;

#define SERIALIZE_PRIVATE_CLONEANDSET2(NAME,VAL) NAME::Ptr set_##VAL(decltype(VAL) VAL) const noexcept {     \
        auto t = toTuple();                                                                                     \
        std::get<SERIALIZE_PRIVATE_GETINDEX(NAME,VAL)>(t) = VAL;                                                \
        return std::make_shared<NAME>(NAME(t));                                                                 \
    }

#define SERIALIZE_JSON(NAME,...)														\
                                                                                        \
string toJSON() const noexcept {                                                        \
    JSON::MVJSONWriter w;                                                               \
    w.begin();                                                                          \
    SERIALIZE_PRIVATE_DUPAUTO(SERIALIZE_PRIVATE_APPENDTOJSON,NAME,__VA_ARGS__)          \
    w.end();                                                                            \
    return w.result;                                                                    \
}                                                                                       \
                                                                                        \
static NAME fromJSON(string json)                                                       \
{                                                                                       \
    JSON::MVJSONReader reader(json);                                                    \
    return fromJSON(reader.root);                                                       \
}                                                                                       \
                                                                                        \
static NAME fromJSON(JSON::MVJSONNode* node)                                            \
{                                                                                       \
    return NAME( make_tuple(SERIALIZE_PRIVATE_DUPAUTO(SERIALIZE_PRIVATE_FROMJSON,NAME,__VA_ARGS__) 0));     \
}                                                                                       \
                                                                                        \
                                                                                        \
                                                                                                            \
NAME(SERIALIZE_PRIVATE_DUPAUTO(SERIALIZE_PRIVATE_CTORIMMUTABLEDECL,NAME,__VA_ARGS__) int finisher = 0) :    \
SERIALIZE_PRIVATE_DUPAUTO(SERIALIZE_PRIVATE_CTORIMMUTABLEPARAM,NAME,__VA_ARGS__)IImmutable()                \
{}                                                                                                          \
                                                                                                            \
NAME(NAME&& other) noexcept :                                                                               \
SERIALIZE_PRIVATE_DUPAUTO(SERIALIZE_PRIVATE_CTORIMMUTABLECOPY,NAME,__VA_ARGS__)IImmutable()                 \
{}                                                                                                          \
NAME(const NAME& other) noexcept :                                                                          \
SERIALIZE_PRIVATE_DUPAUTO(SERIALIZE_PRIVATE_CTORIMMUTABLECOPY,NAME,__VA_ARGS__)IImmutable()                 \
{}                                                                                                          \
                                                                                                            \
bool operator== (const NAME& other) const noexcept {                                                        \
    SERIALIZE_PRIVATE_DUPAUTO(SERIALIZE_PRIVATE_COMPAREIMMUTABLE,NAME,__VA_ARGS__) return true;             \
    return false;                                                                                           \
}                                                                                                           \
                                                                                                            \
typedef std::shared_ptr<NAME> Ptr;                                                                          \
                                                                                                            \
operator Ptr() { return std::make_shared<NAME>(*this); }                                                    \
                                                                                                            \
                                                                                                            \
std::tuple< SERIALIZE_PRIVATE_DUPAUTO(SERIALIZE_PRIVATE_CTORIMMUTABLEDECLTYPENONCONST,NAME,__VA_ARGS__) int> toTuple() const noexcept {                            \
    return make_tuple(SERIALIZE_PRIVATE_DUPAUTO(SERIALIZE_PRIVATE_CTORIMMUTABLEVAL,NAME,__VA_ARGS__) 0);    \
}                                                                                                           \
                                                                                                            \
                                                                                                            \
static const int _index_offset = __COUNTER__;                                                               \
                                                                                                            \
SERIALIZE_PRIVATE_DUPAUTO(SERIALIZE_PRIVATE_FIELDINDEX,NAME,__VA_ARGS__)                                    \
                                                                                                            \
                                                                                                            \
                                                                                                            \
SERIALIZE_PRIVATE_DUPAUTO(SERIALIZE_PRIVATE_CLONEANDSET2,NAME,__VA_ARGS__)                                  \
                                                                                                            \
NAME(std::tuple< SERIALIZE_PRIVATE_DUPAUTO(SERIALIZE_PRIVATE_CTORIMMUTABLEDECLTYPENONCONST,NAME,__VA_ARGS__) int> vars) : SERIALIZE_PRIVATE_DUPAUTO(SERIALIZE_PRIVATE_CTORFROMTUPLE,NAME,__VA_ARGS__)IImmutable()                                                                                \
{}                                                                                                          \
                                                                                                            \



