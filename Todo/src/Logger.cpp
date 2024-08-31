//
// Created by ianpo on 31/08/2024.
//

#include "Todo/Core/Logger.hpp"


#ifdef TODO_LOG
#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
using namespace std::literals;
using namespace std::chrono;
#endif

// Reduce verbosity but let you know what is in what namespace

namespace Todo {
#ifndef TODO_DISABLE_LOG
	static LogFuncPtr s_LogFunc{nullptr};

#ifdef TODO_LOG
	static std::string GetLogTypeStr(LogType logType)
	{
		switch (logType) {
			case LogType::Trace: return "Trace"s;
			case LogType::Info: return "Info"s;
			case LogType::Warning: return "Warning"s;
			case LogType::Error: return "Error"s;
		}
		return "Unknown"s;
	}

// Thanks to Howard Hinnant, here are the function to get the std::tm offline.

// Returns number of days since civil 1970-01-01.  Negative values indicate
//    days prior to 1970-01-01.
// Preconditions:  y-m-d represents a date in the civil (Gregorian) calendar
//                 m is in [1, 12]
//                 d is in [1, last_day_of_month(y, m)]
//                 y is "approximately" in
//                   [numeric_limits<Int>::min()/366, numeric_limits<Int>::max()/366]
//                 Exact range of validity is:
//                 [civil_from_days(numeric_limits<Int>::min()),
//                  civil_from_days(numeric_limits<Int>::max()-719468)]
	template <class Int>
	static constexpr
	Int
	days_from_civil(Int y, unsigned m, unsigned d) noexcept
	{
		static_assert(std::numeric_limits<unsigned>::digits >= 18,
					  "This algorithm has not been ported to a 16 bit unsigned integer");
		static_assert(std::numeric_limits<Int>::digits >= 20,
					  "This algorithm has not been ported to a 16 bit signed integer");
		y -= m <= 2;
		const Int era = (y >= 0 ? y : y-399) / 400;
		const unsigned yoe = static_cast<unsigned>(y - era * 400);      // [0, 399]
		const unsigned doy = (153*(m + (m > 2 ? -3 : 9)) + 2)/5 + d-1;  // [0, 365]
		const unsigned doe = yoe * 365 + yoe/4 - yoe/100 + doy;         // [0, 146096]
		return era * 146097 + static_cast<Int>(doe) - 719468;
	}

// Returns year/month/day triple in civil calendar
// Preconditions:  z is number of days since 1970-01-01 and is in the range:
//                   [numeric_limits<Int>::min(), numeric_limits<Int>::max()-719468].
	template <class Int>
	static constexpr
	std::tuple<Int, unsigned, unsigned>
	civil_from_days(Int z) noexcept
	{
		static_assert(std::numeric_limits<unsigned>::digits >= 18,
					  "This algorithm has not been ported to a 16 bit unsigned integer");
		static_assert(std::numeric_limits<Int>::digits >= 20,
					  "This algorithm has not been ported to a 16 bit signed integer");
		z += 719468;
		const Int era = (z >= 0 ? z : z - 146096) / 146097;
		const unsigned doe = static_cast<unsigned>(z - era * 146097);          // [0, 146096]
		const unsigned yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;  // [0, 399]
		const Int y = static_cast<Int>(yoe) + era * 400;
		const unsigned doy = doe - (365*yoe + yoe/4 - yoe/100);                // [0, 365]
		const unsigned mp = (5*doy + 2)/153;                                   // [0, 11]
		const unsigned d = doy - (153*mp+2)/5 + 1;                             // [1, 31]
		const unsigned m = mp + (mp < 10 ? 3 : -9);                            // [1, 12]
		return std::tuple<Int, unsigned, unsigned>(y + (m <= 2), m, d);
	}

	template <class Int>
	static constexpr
	unsigned
	weekday_from_days(Int z) noexcept
	{
		return static_cast<unsigned>(z >= -4 ? (z+4) % 7 : (z+5) % 7 + 6);
	}

	template <class To, class Rep, class Period>
	static To
	round_down(const std::chrono::duration<Rep, Period>& d)
	{
		To t = std::chrono::duration_cast<To>(d);
		if (t > d)
			--t;
		return t;
	}

	template <class Duration>
	static std::tm make_utc_tm(std::chrono::time_point<std::chrono::system_clock, Duration> tp)
	{
		using namespace std;
		using namespace std::chrono;
		typedef duration<int, ratio_multiply<hours::period, ratio<24>>> days;
		// t is time duration since 1970-01-01
		Duration t = tp.time_since_epoch();
		// d is days since 1970-01-01
		days d = round_down<days>(t);
		// t is now time duration since midnight of day d
		t -= d;
		// break d down into year/month/day
		int year;
		unsigned month;
		unsigned day;
		std::tie(year, month, day) = civil_from_days(d.count());
		// start filling in the tm with calendar info
		std::tm tm = {0};
		tm.tm_year = year - 1900;
		tm.tm_mon = month - 1;
		tm.tm_mday = day;
		tm.tm_wday = weekday_from_days(d.count());
		tm.tm_yday = d.count() - days_from_civil(year, 1, 1);
		// Fill in the time
		tm.tm_hour = duration_cast<hours>(t).count();
		t -= hours(tm.tm_hour);
		tm.tm_min = duration_cast<minutes>(t).count();
		t -= minutes(tm.tm_min);
		tm.tm_sec = duration_cast<seconds>(t).count();
		return tm;
	}

#endif
#endif

	void Logger::Log(LogType logType, std::string_view message, std::source_location source, Clock::time_point timelog)
	{

#ifndef TODO_DISABLE_LOG
		if (s_LogFunc) {
			s_LogFunc(source, timelog, logType, std::string{message});
		}
#ifdef TODO_LOG
		else {
			std::tm gm = make_utc_tm(timelog);
			auto& tp = timelog;
			auto dp = floor<days>(tp);
			year_month_day ymd{dp};
			hh_mm_ss time{floor<milliseconds >(tp-dp)};
			auto ms = (time.subseconds()).count();
			std::cout << "[" << std::put_time(&gm, "%F %R:%S")<<"."<<ms<< " UTC] [" << "TODO" << "] [" << std::filesystem::path(source.file_name()).filename().string() << ":" << source.line() << "] - " << message << std::endl;
		}
#endif
#endif
	}

	void Logger::SetupLogger(LogFuncPtr logfunction)
	{

#ifndef TODO_DISABLE_LOG
		s_LogFunc = logfunction;
#endif
	}

	bool Logger::HasLogger()
	{
#ifndef TODO_DISABLE_LOG
#ifdef TODO_LOG
		return true;
#else
		return HasCustomLogger();
#endif
#else
		return false;
#endif
	}

	bool Logger::HasCustomLogger()
	{
#ifndef TODO_DISABLE_LOG
		return s_LogFunc;
#else
		return false;
#endif
	}
}