find_path(liquidfun_INCLUDE_DIR Box2D/Box2D.h)
find_library(liquidfun_LIBRARY NAMES liquidfun)

set(liquidfun_INCLUDE_DIRS ${liquidfun_INCLUDE_DIR})
set(liquidfun_LIBRARIES ${liquidfun_LIBRARY})

find_package_handle_standard_args(liquidfun DEFAULT_MSG liquidfun_LIBRARY liquidfun_INCLUDE_DIR)
mark_as_advanced(liquidfun_FOUND liquidfun_INCLUDE_DIR liquidfun_LIBRARY)
