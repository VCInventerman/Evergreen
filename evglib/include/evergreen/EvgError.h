#ifndef EVG_ERROR_H
#define EVG_ERROR_H
namespace evg
{



	class EvgError : public std::exception
	{
	public:
		using This = EvgError;

	protected:
		std::string reason_;

	public:
		EvgError() : reason_("No reason", sizeof("No reason")) {}
		EvgError(const std::string& reason_) : reason_(reason_) {}
		EvgError(const char* const reason_) : reason_(reason_) {}
		EvgError(std::string&& reason_) : reason_(std::move(reason_)) {}

		const char* what() const noexcept override
		{
			return reason_.c_str();
		}
	};
};
#endif // EVG_ERROR_H