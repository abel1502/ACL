/// #define FMT_TYPE_(NAME, TYPE)

#ifndef FMT_TYPE_
#error "This file should only be used as DSL"
#endif

#ifndef FMT_TYPE_LAST_
#define FMT_TYPE_LAST_ FMT_TYPE_
#endif


FMT_TYPE_     (at_none               , std::monostate                       )
FMT_TYPE_     (at_bool               , bool                                 )
FMT_TYPE_     (at_char               , char                                 )
FMT_TYPE_     (at_int                , int                                  )
FMT_TYPE_     (at_unsigned           , unsigned                             )
FMT_TYPE_     (at_long_long          , long long                            )
FMT_TYPE_     (at_unsigned_long_long , unsigned long long                   )
FMT_TYPE_     (at_float              , float                                )
FMT_TYPE_     (at_double             , double                               )
FMT_TYPE_     (at_long_double        , long double                          )
FMT_TYPE_     (at_const_void_p       , const void *                         )
FMT_TYPE_     (at_const_char_p       , const char *                         )
FMT_TYPE_     (at_string_view        , std::string_view                     )
FMT_TYPE_LAST_(at_handle             , format_arg<Context>::handle          )

#undef FMT_TYPE_LAST_
#undef FMT_TYPE_
