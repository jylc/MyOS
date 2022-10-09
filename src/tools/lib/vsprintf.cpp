#include "lib/stdarg.h"
#include "lib/stdio.h"

static size_t _ntoa_long(out_func_type out, char* buffer, size_t idx,
	size_t maxlen, unsigned long value, bool negative,
	unsigned long base, unsigned int prec,
	unsigned int width, unsigned int flags) {
	char   buf[PRINTF_NTOA_BUFFER_SIZE];
	size_t len = 0U;

	// no hash for 0 values
	if (!value) {
		flags &= ~FLAGS_HASH;
	}

	// write if precision != 0 and value is != 0
	if (!(flags & FLAGS_PRECISION) || value) {
		do {
			const char digit = (char)(value % base);
			buf[len++] =
				digit < 10 ? '0' + digit
				: (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10;
			value /= base;
		} while (value && (len < PRINTF_NTOA_BUFFER_SIZE));
	}

	return _ntoa_format(out, buffer, idx, maxlen, buf, len, negative,
		(unsigned int)base, prec, width, flags);
}

static size_t _ntoa_format(out_func_type out, char* buffer, size_t idx,
	size_t maxlen, char* buf, size_t len, bool negative,
	unsigned int base, unsigned int prec,
	unsigned int width, unsigned int flags) {
	// pad leading zeros
	if (!(flags & FLAGS_LEFT)) {
		if (width && (flags & FLAGS_ZEROPAD) &&
			(negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
			width--;
		}
		while ((len < prec) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
			buf[len++] = '0';
		}
		while ((flags & FLAGS_ZEROPAD) && (len < width) &&
			(len < PRINTF_NTOA_BUFFER_SIZE)) {
			buf[len++] = '0';
		}
	}

	// handle hash
	if (flags & FLAGS_HASH) {
		if (!(flags & FLAGS_PRECISION) && len &&
			((len == prec) || (len == width))) {
			len--;
			if (len && (base == 16U)) {
				len--;
			}
		}
		if ((base == 16U) && !(flags & FLAGS_UPPERCASE) &&
			(len < PRINTF_NTOA_BUFFER_SIZE)) {
			buf[len++] = 'x';
		}
		else if ((base == 16U) && (flags & FLAGS_UPPERCASE) &&
			(len < PRINTF_NTOA_BUFFER_SIZE)) {
			buf[len++] = 'X';
		}
		else if ((base == 2U) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
			buf[len++] = 'b';
		}
		if (len < PRINTF_NTOA_BUFFER_SIZE) {
			buf[len++] = '0';
		}
	}

	if (len < PRINTF_NTOA_BUFFER_SIZE) {
		if (negative) {
			buf[len++] = '-';
		}
		else if (flags & FLAGS_PLUS) {
			buf[len++] = '+'; // ignore the space if the '+' exists
		}
		else if (flags & FLAGS_SPACE) {
			buf[len++] = ' ';
		}
	}

	return _out_rev(out, buffer, idx, maxlen, buf, len, width, flags);
}

static size_t _out_rev(out_func_type out, char* buffer, size_t idx,
	size_t maxlen, const char* buf, size_t len,
	unsigned int width, unsigned int flags) {
	const size_t start_idx = idx;

	// pad spaces up to given width
	if (!(flags & FLAGS_LEFT) && !(flags & FLAGS_ZEROPAD)) {
		for (size_t i = len; i < width; i++) {
			out(' ', buffer, idx++, maxlen);
		}
	}

	// reverse string
	while (len) {
		out(buf[--len], buffer, idx++, maxlen);
	}

	// append pad spaces up to given width
	if (flags & FLAGS_LEFT) {
		while (idx - start_idx < width) {
			out(' ', buffer, idx++, maxlen);
		}
	}

	return idx;
}

static inline unsigned int _strnlen_s(const char* str, size_t maxsize) {
	const char* s;
	for (s = str; *s && maxsize--; ++s)
		;
	return (unsigned int)(s - str);
}


// internal vsnprintf
int _vsnprintf(char* buffer, const size_t maxlen, const char* format,
	va_list va) {
	out_func_type out_func = _out_buffer;
	size_t idx = 0U;
	size_t flags, width, n, precision;
	if (!buffer) {
		out_func = nullptr;
	}

	while (*format)
	{
		// format specifier?  %[flags][width][.precision][length]
		if (*format != '%') {
			out_func(*format, buffer, idx++, maxlen);
			format++;
			continue;
		}
		else {
			// skip '%'
			format++;
		}

		// 分析flag参数
		flags = 0U;
		do {
			switch (*format)
			{
			case '0':
				flags |= FLAGS_ZEROPAD;
				format++;
				n = 1U;
				break;
			case '-':
				flags |= FLAGS_LEFT;
				format++;
				n = 1U;
				break;
			case '+':
				flags |= FLAGS_PLUS;
				format++;
				n = 1U;
				break;
			case ' ':
				flags |= FLAGS_SPACE;
				format++;
				n = 1U;
				break;
			case '#':
				flags |= FLAGS_HASH;
				format++;
				n = 1U;
				break;
			default:
				n = 0U;
				break;
			}
		} while (n);

		width = 0U;
		if (_is_digit(*format)) {
			// 同时改变指针
			width = _atoi(&format);
		}
		else if (*format == '*') {
			const int w = va_arg(va, int32_t);
			if (w < 0) {
				flags |= FLAGS_LEFT;
				width = (uint32_t)-w;
			}
			else {
				width = (uint32_t)w;
			}
			format++;
		}
		precision = 0U;
		if (*format == '.') {
			flags |= FLAGS_PRECISION;
			format++;
			if (_is_digit(*format)) {
				precision = _atoi(&format);
			}
			else if (*format == '*') {
				const int prec = (int)va_arg(va, int);
				precision = prec > 0 ? (unsigned int)prec : 0U;
				format++;
			}
		}

		// evaluate length field
		switch (*format) {
		case 'l':
			flags |= FLAGS_LONG;
			format++;
			if (*format == 'l') {
				flags |= FLAGS_LONG_LONG;
				format++;
			}
			break;
		case 'h':
			flags |= FLAGS_SHORT;
			format++;
			if (*format == 'h') {
				flags |= FLAGS_CHAR;
				format++;
			}
			break;
#if defined(PRINTF_SUPPORT_PTRDIFF_T)
		case 't':
			flags |= (sizeof(ptrdiff_t) == sizeof(long) ? FLAGS_LONG
				: FLAGS_LONG_LONG);
			format++;
			break;
#endif
		case 'j':
			flags |= (sizeof(intmax_t) == sizeof(long) ? FLAGS_LONG
				: FLAGS_LONG_LONG);
			format++;
			break;
		case 'z':
			flags |= (sizeof(size_t) == sizeof(long) ? FLAGS_LONG
				: FLAGS_LONG_LONG);
			format++;
			break;
		default:
			break;
		}

		// evaluate specifier
		switch (*format) {
		case 'd':
		case 'i':
		case 'u':
		case 'x':
		case 'X':
		case 'o':
		case 'b': {
			// set the base
			unsigned int base;
			if (*format == 'x' || *format == 'X') {
				base = 16U;
			}
			else if (*format == 'o') {
				base = 8U;
			}
			else if (*format == 'b') {
				base = 2U;
			}
			else {
				base = 10U;
				flags &= ~FLAGS_HASH; // no hash for dec format
			}
			// uppercase
			if (*format == 'X') {
				flags |= FLAGS_UPPERCASE;
			}

			// no plus or space flag for u, x, X, o, b
			if ((*format != 'i') && (*format != 'd')) {
				flags &= ~(FLAGS_PLUS | FLAGS_SPACE);
			}

			// ignore '0' flag when precision is given
			if (flags & FLAGS_PRECISION) {
				flags &= ~FLAGS_ZEROPAD;
			}

			// convert the integer
			if ((*format == 'i') || (*format == 'd')) {
				// signed
				if (flags & FLAGS_LONG_LONG) {
#if defined(PRINTF_SUPPORT_LONG_LONG)
					const long long value = va_arg(va, long long);
					idx = _ntoa_long_long(
						out, buffer, idx, maxlen,
						(unsigned long long)(value > 0 ? value : 0 - value),
						value < 0, base, precision, width, flags);
#endif
				}
				else if (flags & FLAGS_LONG) {
					const long value = va_arg(va, long);
					idx = _ntoa_long(
						out_func, buffer, idx, maxlen,
						(unsigned long)(value > 0 ? value : 0 - value),
						value < 0, base, precision, width, flags);
				}
				else {
					const int value =
						(flags & FLAGS_CHAR) ? (char)va_arg(va, int)
						: (flags & FLAGS_SHORT) ? (short int)va_arg(va, int)
						: va_arg(va, int);
					idx = _ntoa_long(
						out_func, buffer, idx, maxlen,
						(unsigned int)(value > 0 ? value : 0 - value),
						value < 0, base, precision, width, flags);
				}
			}
			else {
				// unsigned
				if (flags & FLAGS_LONG_LONG) {
#if defined(PRINTF_SUPPORT_LONG_LONG)
					idx = _ntoa_long_long(out, buffer, idx, maxlen,
						va_arg(va, unsigned long long),
						false, base, precision, width,
						flags);
#endif
				}
				else if (flags & FLAGS_LONG) {
					idx = _ntoa_long(out_func, buffer, idx, maxlen,
						va_arg(va, unsigned long), false, base,
						precision, width, flags);
				}
				else {
					const unsigned int value =
						(flags & FLAGS_CHAR)
						? (unsigned char)va_arg(va, unsigned int)
						: (flags & FLAGS_SHORT)
						? (unsigned short int)va_arg(va, unsigned int)
						: va_arg(va, unsigned int);
					idx = _ntoa_long(out_func, buffer, idx, maxlen, value, false,
						base, precision, width, flags);
				}
			}
			format++;
			break;
		}
#if defined(PRINTF_SUPPORT_FLOAT)
		case 'f':
		case 'F':
			if (*format == 'F')
				flags |= FLAGS_UPPERCASE;
			idx = _ftoa(out, buffer, idx, maxlen, va_arg(va, double),
				precision, width, flags);
			format++;
			break;
#if defined(PRINTF_SUPPORT_EXPONENTIAL)
		case 'e':
		case 'E':
		case 'g':
		case 'G':
			if ((*format == 'g') || (*format == 'G'))
				flags |= FLAGS_ADAPT_EXP;
			if ((*format == 'E') || (*format == 'G'))
				flags |= FLAGS_UPPERCASE;
			idx = _etoa(out, buffer, idx, maxlen, va_arg(va, double),
				precision, width, flags);
			format++;
			break;
#endif // PRINTF_SUPPORT_EXPONENTIAL
#endif // PRINTF_SUPPORT_FLOAT
		case 'c': {
			unsigned int l = 1U;
			// pre padding
			if (!(flags & FLAGS_LEFT)) {
				while (l++ < width) {
					out_func(' ', buffer, idx++, maxlen);
				}
			}
			// char output
			out_func((char)va_arg(va, int), buffer, idx++, maxlen);
			// post padding
			if (flags & FLAGS_LEFT) {
				while (l++ < width) {
					out_func(' ', buffer, idx++, maxlen);
				}
			}
			format++;
			break;
		}

		case 's': {
			const char* p = va_arg(va, char*);
			unsigned int l =
				_strnlen_s(p, precision ? precision : (size_t)-1);
			// pre padding
			if (flags & FLAGS_PRECISION) {
				l = (l < precision ? l : precision);
			}
			if (!(flags & FLAGS_LEFT)) {
				while (l++ < width) {
					out_func(' ', buffer, idx++, maxlen);
				}
			}
			// string output
			while ((*p != 0) &&
				(!(flags & FLAGS_PRECISION) || precision--)) {
				out_func(*(p++), buffer, idx++, maxlen);
			}
			// post padding
			if (flags & FLAGS_LEFT) {
				while (l++ < width) {
					out_func(' ', buffer, idx++, maxlen);
				}
			}
			format++;
			break;
		}

		case 'p': {
			width = sizeof(void*) * 2U;
			flags |= FLAGS_ZEROPAD | FLAGS_UPPERCASE;
#if defined(PRINTF_SUPPORT_LONG_LONG)
			const bool is_ll = sizeof(uintptr_t) == sizeof(long long);
			if (is_ll) {
				idx = _ntoa_long_long(out, buffer, idx, maxlen,
					(unsigned long)va_arg(va, void*),
					false, 16U, precision, width, flags);
			}
			else {
#endif
				idx = _ntoa_long(
					out_func, buffer, idx, maxlen,
					(unsigned long)((unsigned long)va_arg(va, void*)),
					false, 16U, precision, width, flags);
#if defined(PRINTF_SUPPORT_LONG_LONG)
			}
#endif
			format++;
			break;
		}

		case '%':
			out_func('%', buffer, idx++, maxlen);
			format++;
			break;

		default:
			out_func(*format, buffer, idx++, maxlen);
			format++;
			break;
		}

	}
	// termination
	out_func((char)0, buffer, idx < maxlen ? idx : maxlen - 1U, maxlen);

	// return written chars without terminating \0
	return (int)idx;
}