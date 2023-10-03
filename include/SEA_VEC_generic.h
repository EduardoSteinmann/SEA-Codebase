#ifndef SEA_VEC_generic
#define SEA_VEC_generic

//////////////////////////////////////////////////////////////////////
//*                    SEA VEC declarations                        *//
#define SEA_VEC_declr(type) \
    typedef struct SEA_VEC_##type { \
        size_t _len; \
        size_t _capacity; \
        type *_buffer; \
    } SEA_VEC_##type; \
    SEA_VEC_##type get_SEA_VEC_##type(); \
    type SEA_VEC_##type##_at_idx(const SEA_VEC_##type *const restrict _v, const size_t _idx); \
    void SEA_VEC_##type##_push_back(SEA_VEC_##type *const restrict _v, type _elm); \
    void SEA_VEC_##type##_pop_back(SEA_VEC_##type *const restrict _v); \
    const size_t SEA_VEC_##type##_size(const SEA_VEC_##type *const restrict _v); \
    const type *const SEA_VEC_##type##_begin(const SEA_VEC_##type *const restrict _v); \
    const type *const SEA_VEC_##type##_end(const SEA_VEC_##type *const restrict _v); \
    void SEA_VEC_##type##_append(SEA_VEC_##type *_dest, SEA_VEC_##type *_src); \

//////////////////////////////////////////////////////////////////////
//*                    SEA VEC implementaions                      *//
#define SEA_VEC_impl(type) \
    SEA_VEC_##type get_SEA_VEC_##type() { \
        type *buffer = (type *) malloc(sizeof(type) * 2); \
        if (buffer == NULL) __builtin_trap(); \
        return (SEA_VEC_##type) { \
            ._len = 0, \
            ._capacity = 2, \
            ._buffer = buffer, \
        }; \
    } \
    type SEA_VEC_##type##_at_idx(const SEA_VEC_##type *const restrict _v, const size_t _idx) { \
        if (_idx >= _v->_len) __builtin_trap(); \
        return _v->_buffer[_idx]; \
    } \
    void SEA_VEC_##type##_push_back(SEA_VEC_##type *const restrict _v, type _elm) { \
        if (_v->_len + 1 > _v->_capacity) { \
            _v->_capacity = (size_t) (_v->_capacity * 2); \
            _v->_buffer = (type *) realloc(_v->_buffer, sizeof(type) * _v->_capacity); \
        } \
        _v->_buffer[_v->_len] = _elm; \
        _v->_len++; \
    } \
    void SEA_VEC_##type##_pop_back(SEA_VEC_##type *const restrict _v) { \
        if (_v->_len <= 0) { _v->_len = 0; return; } \
        _v->_len--; \
    } \
    const size_t SEA_VEC_##type##_size(const SEA_VEC_##type *const restrict _v) { return _v->_len; } \
    const type *const SEA_VEC_##type##_begin(const SEA_VEC_##type *const restrict _v) { return _v->_buffer; } \
    const type *const SEA_VEC_##type##_end(const SEA_VEC_##type *const restrict _v) { return _v->_buffer + _v->_len; } \
    void SEA_VEC_##type##_append(SEA_VEC_##type *_dest, SEA_VEC_##type *_src) { \
            if (_dest->_capacity < _dest->_len + _src->_len) { \
            _dest->_buffer = (type *) realloc(_dest->_buffer , (_dest->_len + _src->_len) * sizeof(type)); \
            _dest->_capacity = _dest->_len + _src->_len; \
        } \
        memcpy(_dest->_buffer + _dest->_len, _src->_buffer, _src->_len * sizeof(type)); \
        _dest->_len += _src->_len; \
    }

#endif