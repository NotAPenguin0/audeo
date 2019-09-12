#ifndef AUDEO_EXCEPTION_HPP_
#define AUDEO_EXCEPTION_HPP_

#ifndef AUDEO_NO_EXCEPTIONS
#    define AUDEO_THROW(except) throw except
#    define AUDEO_CATCH(except) catch (except)
#    define AUDEO_TRY try
#    include <stdexcept>
#	 include <string_view>

namespace audeo {

class exception : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
    inline exception(std::string_view message) : std::runtime_error(message.data()) {}
};

} // namespace audeo

#else
#    define AUDEO_THROW(...)
#    define AUDEO_CATCH(...) if (false)
#    define AUDEO_TRY

#endif

#endif
