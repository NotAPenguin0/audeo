#ifndef AUDEO_EXPORT_IMPORT_HPP_
#    define AUDEO_EXPORT_IMPORT_HPP_

#    ifdef _WIN32
#        ifdef AUDEO_EXPORT
#            define AUDEO_API __declspec(dllexport)
#        else
#            define AUDEO_API __declspec(dllimport)
#        endif // AUDEO_EXPORT
#    else
#        define AUDEO_API

#    endif // _WIN32

// Build static lib. No need for and dll stuff
#    undef AUDEO_API
#    define AUDEO_API

#endif // AUDEO_EXPORT_IMPORT_HPP_

