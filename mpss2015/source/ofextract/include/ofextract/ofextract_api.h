#pragma once

#ifdef _MSC_VER
#   define OFEXTRACT_API_EXPORT_DECLARATION __declspec(dllexport)
#   define OFEXTRACT_API_IMPORT_DECLARATION __declspec(dllimport)
#elif __GNUC__
#	define OFEXTRACT_API_EXPORT_DECLARATION __attribute__ ((visibility ("default")))
#	define OFEXTRACT_API_IMPORT_DECLARATION __attribute__ ((visibility ("default")))
#else
#   define OFEXTRACT_API_EXPORT_DECLARATION
#   define OFEXTRACT_API_IMPORT_DECLARATION
#endif 

#ifndef MPSS2015_STATIC
#ifdef OFEXTRACT_EXPORTS
#   define OFEXTRACT_API OFEXTRACT_API_EXPORT_DECLARATION
#else
#   define OFEXTRACT_API OFEXTRACT_API_IMPORT_DECLARATION
#endif
#else
#   define OFEXTRACT_API
#endif

#ifdef N_DEBUG
#   define IF_DEBUG(statement)
#   define IF_NDEBUG(statement) statement
#else
#   define IF_DEBUG(statement) statement
#   define IF_NDEBUG(statement)
#endif // N_DEBUG

// http://stackoverflow.com/questions/18387640/how-to-deal-with-noexcept-in-visual-studio
#ifndef NOEXCEPT
#   ifdef _MSC_VER
#       define NOEXCEPT
#   else
#       define NOEXCEPT noexcept
#   endif
#endif


/**
 * \namespace fiblib
 *
 * \brief Contains all the core classes that ... functionality.
 *
 * TODO: Detailed documentation for fiblib here.
 */
